#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <syslog.h>

// On inclut directement l'interposeur pour tester sans LD_PRELOAD.
// Ne pas entourer avec extern "C" car le fichier contient du C++ (namespace etc.).
#include "../obfuscation_methods/swap_printf.cpp"

// Capture simple de stdout dans un fichier temporaire.
static bool capture_stdout_begin(const char *path, FILE **old_fp) {
    *old_fp = stdout;
    FILE *f = freopen(path, "w", stdout);
    return f != NULL;
}

static void capture_stdout_end(FILE *old_fp) {
    fflush(stdout);
    // Restaurer stdout sur /dev/tty si possible (simplifié).
    freopen("/dev/tty", "w", stdout); // Ignorer l'échec potentiel.
    stdout = old_fp;
}

int main() {
    std::cout << "=== Test swap printf <-> syslog ===" << std::endl;

    // 1) Test du retour de printf (redirigé vers syslog). On ne peut pas vérifier la sortie
    //    mais on peut vérifier la valeur de retour (nombre de caractères formatés).
    const char *fmt = "Message %d %s";
    int expected_len = std::strlen("Message ") + 1 /*digit*/ + 1 /*space*/ + std::strlen("test");
    int r = printf(fmt, 5, "test");
    std::cout << "Retour printf: " << r << " (attendu >= " << expected_len << ")" << std::endl;

    // 2) Capture de la sortie de syslog (redirigé vers printf). Ici on doit voir le texte.
    FILE *old_stdout = NULL;
    const char *outfile = "swap_printf_out.txt";
    if (!capture_stdout_begin(outfile, &old_stdout)) {
        std::cerr << "Echec capture stdout" << std::endl;
        return 1;
    }

    syslog(LOG_INFO, "Syslog vers printf: %s %d", "valeur", 99);

    capture_stdout_end(old_stdout);

    // Lire le fichier produit.
    FILE *rf = std::fopen(outfile, "r");
    if (!rf) {
        std::cerr << "Echec ouverture fichier sortie" << std::endl;
        return 1;
    }
    char buffer[256];
    std::memset(buffer, 0, sizeof(buffer));
    std::fread(buffer, 1, sizeof(buffer)-1, rf);
    std::fclose(rf);

    std::cout << "Contenu capture syslog->printf: [" << buffer << "]" << std::endl;

    // Vérification simple: présence du fragment attendu.
    if (std::strstr(buffer, "Syslog vers printf:") != NULL) {
        std::cout << "OK: redirection syslog -> printf fonctionne" << std::endl;
    } else {
        std::cout << "ECHEC: fragment non trouvé" << std::endl;
        return 1;
    }

    // 3) Cas limites: chaîne vide.
    int r_empty = printf("%s", "");
    std::cout << "printf(chaine vide) retour=" << r_empty << std::endl;

    std::cout << "=== Fin test swap_printf ===" << std::endl;
    return 0;
}
