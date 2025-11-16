// ============================================================================
//  Interposition printf <-> syslog (Version C++03 compatible GCC 4.4.7)
//  Objectif : appeler le vrai syslog quand l'application utilise printf
//             et appeler le vrai printf quand elle utilise syslog.
//  Cette technique fonctionne via LD_PRELOAD.
//  Remarques :
//   * Symbols doivent conserver le linkage C (extern "C").
//   * On évite toute fonctionnalité C++11 (auto, lambda, variadic templates...)
//   * On s'assure de la présence de va_copy (fallback si absent).
// ============================================================================

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <dlfcn.h>
#include <syslog.h>

#ifndef va_copy
#  ifdef __va_copy
#    define va_copy(dest, src) __va_copy(dest, src)
#  else
#    define va_copy(dest, src) ((dest) = (src))
#  endif
#endif

extern "C" {
    // Pointeurs vers les vraies fonctions libc
    static int (*real_printf)(const char *format, ...) = 0;
    static void (*real_syslog)(int priority, const char *format, ...) = 0;
}

namespace {
    // Résolution paresseuse des symboles pour éviter les soucis d'init ordre.
    static void resolve_symbols() {
        if (!real_printf) {
            real_printf = (int (*)(const char *, ...)) dlsym(RTLD_NEXT, "printf");
        }
        if (!real_syslog) {
            real_syslog = (void (*)(int, const char *, ...)) dlsym(RTLD_NEXT, "syslog");
        }
    }
}

// Interposition : printf => syslog(LOG_INFO, ...)
extern "C" int printf(const char *format, ...) {
    resolve_symbols();

    va_list ap;
    va_start(ap, format);
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int needed = vsnprintf(NULL, 0, format, ap_copy);
    va_end(ap_copy);

    if (needed < 0) {
        va_end(ap);
        return needed; // erreur format
    }

    size_t size = (size_t)needed + 1;
    char *buf = static_cast<char*>(std::malloc(size));
    if (!buf) {
        va_end(ap);
        return -1; // échec alloc
    }
    vsnprintf(buf, size, format, ap);
    va_end(ap);

    if (real_syslog) {
        real_syslog(LOG_INFO, "%s", buf);
    } else {
        // Fallback : écrire directement sur stdout
        std::fputs(buf, stdout);
    }

    std::free(buf);
    return needed; // Mimique du contrat printf
}

// Interposition : syslog => printf
extern "C" void syslog(int priority, const char *format, ...) {
    resolve_symbols();

    va_list ap;
    va_start(ap, format);
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int needed = vsnprintf(NULL, 0, format, ap_copy);
    va_end(ap_copy);

    if (needed < 0) {
        va_end(ap);
        return; // erreur format
    }

    size_t size = (size_t)needed + 1;
    char *buf = static_cast<char*>(std::malloc(size));
    if (!buf) {
        va_end(ap);
        return; // échec alloc
    }
    vsnprintf(buf, size, format, ap);
    va_end(ap);

    if (real_printf) {
        real_printf("%s", buf);
    } else {
        std::fputs(buf, stdout);
    }

    std::free(buf);
    (void)priority; // éviter warning param unused
}
