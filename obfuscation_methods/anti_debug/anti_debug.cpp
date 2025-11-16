// Anti-debug utilities (C++03, GCC 4.4.7 compatible)
// Features:
//  - Detect debugger via /proc/self/status (TracerPid)
//  - Detect LD_PRELOAD presence
//  - Compute simple checksum (Adler-32) of the running executable (/proc/self/exe)
//
// Linux-only (uses /proc)

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h> // PATH_MAX
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <link.h>

namespace anti_debug {

// Return true if TracerPid > 0 in /proc/self/status
static bool is_debugger_present_proc() {
    FILE* f = std::fopen("/proc/self/status", "r");
    if (!f) return false;
    char line[512];
    bool traced = false;
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strncmp(line, "TracerPid:", 10) == 0) {
            // Format: "TracerPid:\t<id>\n"
            const char* p = line + 10;
            while (*p == ' ' || *p == '\t') ++p;
            long v = std::strtol(p, 0, 10);
            traced = (v > 0);
            break;
        }
    }
    std::fclose(f);
    return traced;
}

// Return true if LD_PRELOAD is set and non-empty
static bool has_ld_preload() {
    const char* v = std::getenv("LD_PRELOAD");
    return (v && *v);
}

// Adler-32 checksum implementation
static unsigned long adler32_update(unsigned long adler, const unsigned char* data, size_t len) {
    const unsigned long MOD_ADLER = 65521UL;
    unsigned long a = adler & 0xFFFFUL;
    unsigned long b = (adler >> 16) & 0xFFFFUL;
    for (size_t i = 0; i < len; ++i) {
        a = (a + data[i]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
    return (b << 16) | a;
}

static unsigned long adler32_file(const char* path, unsigned long* out_size) {
    if (out_size) *out_size = 0;
    FILE* f = std::fopen(path, "rb");
    if (!f) return 1UL; // Non-zero default
    unsigned long adler = 1UL; // initial value
    unsigned char buf[4096];
    while (true) {
        size_t n = std::fread(buf, 1, sizeof(buf), f);
        if (n > 0) {
            adler = adler32_update(adler, buf, n);
            if (out_size) *out_size += (unsigned long)n;
        }
        if (n < sizeof(buf)) {
            if (std::ferror(f)) {
                // error
                std::fclose(f);
                return adler;
            }
            break; // EOF
        }
    }
    std::fclose(f);
    return adler;
}

// Compute checksum of the running executable via /proc/self/exe symlink
static unsigned long self_checksum(unsigned long* out_size) {
    char path[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (n <= 0) {
        if (out_size) *out_size = 0;
        return 1UL;
    }
    path[n] = '\0';
    return adler32_file(path, out_size);
}

// ptrace-based detection: returns true if a debugger seems present
static bool is_debugger_present_ptrace_impl() {
    errno = 0;
    long r = ptrace(PTRACE_TRACEME, 0, 0, 0);
    if (r == -1 && errno == EPERM) {
        return true; // already being traced
    }
    // No debugger detected; nothing else to do.
    return false;
}

// Inspect parent process name/commandline for suspicious tools
static bool parent_process_suspicious_impl() {
    pid_t p = getppid();
    char path[64];
    std::snprintf(path, sizeof(path), "/proc/%ld/comm", (long)p);
    char buf[256];
    FILE* f = std::fopen(path, "r");
    if (f) {
        size_t n = std::fread(buf, 1, sizeof(buf)-1, f);
        std::fclose(f);
        buf[n] = '\0';
        if (std::strstr(buf, "gdb") || std::strstr(buf, "lldb") || std::strstr(buf, "strace") || std::strstr(buf, "ltrace") || std::strstr(buf, "rr") || std::strstr(buf, "valgrind"))
            return true;
    }
    std::snprintf(path, sizeof(path), "/proc/%ld/cmdline", (long)p);
    f = std::fopen(path, "rb");
    if (f) {
        size_t n = std::fread(buf, 1, sizeof(buf)-1, f);
        std::fclose(f);
        buf[n] = '\0';
        if (std::strstr(buf, "gdb") || std::strstr(buf, "lldb") || std::strstr(buf, "strace") || std::strstr(buf, "ltrace") || std::strstr(buf, "rr") || std::strstr(buf, "valgrind"))
            return true;
    }
    return false;
}

// Environment checks: look for LD_* variables commonly used for instrumentation
static bool env_suspicious_impl() {
    const char* vars[] = { "LD_AUDIT", "LD_DEBUG", "LD_PRELOAD", "LD_LIBRARY_PATH", 0 };
    const char** v = vars;
    while (*v) {
        const char* val = std::getenv(*v);
        if (val && *val) return true;
        ++v;
    }
    return false;
}

// dl_iterate_phdr: detect suspicious loaded objects
struct suspect_scan_ctx { bool found; };
static int phdr_suspect_cb(struct dl_phdr_info* info, size_t, void* data) {
    suspect_scan_ctx* ctx = (suspect_scan_ctx*)data;
    if (info->dlpi_name && *info->dlpi_name) {
        const char* s = info->dlpi_name;
        if (std::strstr(s, "frida") || std::strstr(s, "pin") || std::strstr(s, "valgrind") || std::strstr(s, "ltrace") || std::strstr(s, "strace"))
            ctx->found = true;
    }
    return 0;
}

static bool loaded_objects_suspicious_impl() {
    suspect_scan_ctx ctx; ctx.found = false;
    dl_iterate_phdr(phdr_suspect_cb, &ctx);
    return ctx.found;
}

// Compute checksum of all executable PT_LOAD segments
static unsigned long text_segments_checksum_impl(unsigned long* out_size) {
    struct ctx_t { unsigned long sum; unsigned long total; };
    struct ctx_t ctx; ctx.sum = 1UL; ctx.total = 0UL;
    struct Local {
        static int cb(struct dl_phdr_info* info, size_t, void* data) {
            ctx_t* c = (ctx_t*)data;
            for (int i = 0; i < (int)info->dlpi_phnum; ++i) {
                const ElfW(Phdr)& ph = info->dlpi_phdr[i];
                if (ph.p_type == PT_LOAD && (ph.p_flags & PF_X)) {
                    const unsigned char* base = (const unsigned char*)(info->dlpi_addr + ph.p_vaddr);
                    size_t len = (size_t)ph.p_memsz;
                    if (len > 0 && base)
                        c->sum = adler32_update(c->sum, base, len);
                    c->total += (unsigned long)len;
                }
            }
            return 0;
        }
    };
    dl_iterate_phdr(Local::cb, &ctx);
    if (out_size) *out_size = ctx.total;
    return ctx.sum;
}

// Public API wrappers
bool is_debugger_present() { return is_debugger_present_proc(); }
bool has_preload() { return has_ld_preload(); }
unsigned long checksum_self(unsigned long* out_size) { return self_checksum(out_size); }
bool is_debugger_present_ptrace() { return is_debugger_present_ptrace_impl(); }
bool parent_process_suspicious() { return parent_process_suspicious_impl(); }
bool env_suspicious() { return env_suspicious_impl(); }
bool loaded_objects_suspicious() { return loaded_objects_suspicious_impl(); }
unsigned long text_segments_checksum(unsigned long* out_size) { return text_segments_checksum_impl(out_size); }

} // namespace anti_debug
