#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <dlfcn.h>

// Include the implementation directly for simplicity in this project layout
#include "../obfuscation_methods/anti_debug/anti_debug.h"
// Inclure l'implémentation pour ce projet mono-fichier (pas de build system dédié)
#include "../obfuscation_methods/anti_debug/anti_debug.cpp"

static void print_bool(const char* label, bool v) {
    std::cout << label << ": " << (v ? "YES" : "NO") << std::endl;
}

int main() {
    std::cout << "=== Anti-Debug checks ===" << std::endl;

    bool dbg = anti_debug::is_debugger_present();
    print_bool("Debugger present (proc)", dbg);

    bool dbg2 = anti_debug::is_debugger_present_ptrace();
    print_bool("Debugger present (ptrace)", dbg2);

    bool preload = anti_debug::has_preload();
    print_bool("LD_PRELOAD set", preload);
    // Toggle LD_PRELOAD
    setenv("LD_PRELOAD", "libX.so", 1);
    bool preload_on = anti_debug::has_preload();
    print_bool("LD_PRELOAD set (after setenv)", preload_on);
    unsetenv("LD_PRELOAD");
    bool preload_off = anti_debug::has_preload();
    print_bool("LD_PRELOAD set (after unsetenv)", preload_off);

    bool envsus = anti_debug::env_suspicious();
    print_bool("Env suspicious (LD_*)", envsus);
    // Toggle an LD_* var
    setenv("LD_DEBUG", "1", 1);
    bool envsus2 = anti_debug::env_suspicious();
    print_bool("Env suspicious (after LD_DEBUG=1)", envsus2);
    unsetenv("LD_DEBUG");
    bool envsus3 = anti_debug::env_suspicious();
    print_bool("Env suspicious (after LD_DEBUG unset)", envsus3);

    bool parentsus = anti_debug::parent_process_suspicious();
    print_bool("Parent suspicious", parentsus);

    bool loadsus = anti_debug::loaded_objects_suspicious();
    print_bool("Loaded objects suspicious", loadsus);

    unsigned long size1 = 0, size2 = 0;
    unsigned long c1 = anti_debug::checksum_self(&size1);
    unsigned long c2 = anti_debug::checksum_self(&size2);

    std::cout << "Self checksum #1: 0x" << std::hex << c1 << std::dec << " (size=" << size1 << ")" << std::endl;
    std::cout << "Self checksum #2: 0x" << std::hex << c2 << std::dec << " (size=" << size2 << ")" << std::endl;

    unsigned long tsize = 0;
    unsigned long tchk = anti_debug::text_segments_checksum(&tsize);
    std::cout << "Text segments checksum: 0x" << std::hex << tchk << std::dec << " (total=" << tsize << ")" << std::endl;
    // dlopen a common library to see if checksum/size changes (may or may not change depending on already loaded)
    void* h = dlopen("libm.so.6", RTLD_NOW);
    unsigned long tsize2 = 0;
    unsigned long tchk2 = anti_debug::text_segments_checksum(&tsize2);
    std::cout << "After dlopen(libm): text checksum: 0x" << std::hex << tchk2 << std::dec << " (total=" << tsize2 << ")" << std::endl;
    if (h) dlclose(h);

    // Optional: simulate a debugger by attaching to a child process
    const char* enable_attach = std::getenv("ENABLE_PTRACE_ATTACH");
    if (enable_attach && *enable_attach == '1') {
        pid_t pid = fork();
        if (pid == 0) {
            // Child: wait to be attached and detect
            for (int i = 0; i < 50; ++i) {
                if (anti_debug::is_debugger_present() || anti_debug::is_debugger_present_ptrace()) {
                    std::cout << "Child: debugger detected" << std::endl;
                    _exit(0);
                }
                usleep(100000); // 100ms
            }
            std::cout << "Child: debugger NOT detected" << std::endl;
            _exit(2);
        } else if (pid > 0) {
            // Parent: try to attach
            sleep(1);
            if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
                std::perror("PTRACE_ATTACH");
                kill(pid, SIGKILL);
                int st; waitpid(pid, &st, 0);
                std::cout << "ptrace attach not permitted on this system (skipping attach test)" << std::endl;
            } else {
                int st; waitpid(pid, &st, 0); // stopped
                ptrace(PTRACE_CONT, pid, 0, 0); // let it run under trace
                sleep(1); // give child time to detect
                ptrace(PTRACE_DETACH, pid, 0, 0); // detach to allow clean exit
                waitpid(pid, &st, 0);
                if (WIFEXITED(st)) {
                    int code = WEXITSTATUS(st);
                    std::cout << "Child exited with code " << code << (code==0?" (OK)":" (FAIL)") << std::endl;
                } else if (WIFSIGNALED(st)) {
                    std::cout << "Child killed by signal " << WTERMSIG(st) << std::endl;
                } else {
                    std::cout << "Child did not exit cleanly" << std::endl;
                }
            }
        } else {
            std::perror("fork");
        }
    } else {
        std::cout << "(ptrace attach test skipped, set ENABLE_PTRACE_ATTACH=1 to run it)" << std::endl;
    }

    if (c1 == c2 && size1 == size2) {
        std::cout << "Checksum stable: OK" << std::endl;
    } else {
        std::cout << "Checksum mismatch: FAIL" << std::endl;
    }

    std::cout << "=== End ===" << std::endl;
    return 0;
}
