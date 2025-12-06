import subprocess
import itertools
import string
import sys
import os
from concurrent.futures import ThreadPoolExecutor, as_completed

try:
    from tqdm import tqdm
except ImportError:
    print("Installe tqdm pour une barre de progression: pip install tqdm")
    tqdm = lambda x, **kwargs: x

# CONFIGURATION
# ---------------------------------------------------------
EXE_PATH = r"R:\ton_executable.exe"  # Mets le chemin vers ton RamDisk si possible
MAX_LEN = 3  # Taille max à tester (1, 2, 3)
WORKERS = 50  # Nombre de threads (commence à 20, monte si ton CPU tient)
ALPHABET = string.printable.strip()  # ASCII imprimables


# ---------------------------------------------------------

def test_candidate(candidate):
    """Lance l'exe avec le candidat et vérifie la sortie."""
    try:
        # NOTE : Si l'exe prend l'input via STDIN (clavier), utilise `input=candidate`
        # NOTE : Si l'exe prend l'input via ARGUMENT, utilise la liste `args` comme ci-dessous

        # Cas 1: Argument (ex: programme.exe "motdepasse")
        process = subprocess.run(
            [EXE_PATH, candidate],
            capture_output=True,
            text=True,
            timeout=0.5  # Timeout court pour aller vite
        )

        # Récupère la sortie (nettoyée des espaces vides autour)
        output = process.stdout.strip()

        # LOGIQUE DE SUCCÈS :
        # Si la sortie est différente de l'entrée (et non vide), c'est suspect.
        if output and output != candidate:
            return (candidate, output)

    except subprocess.TimeoutExpired:
        pass  # Le programme a planté ou bouclé, on ignore
    except Exception as e:
        pass

    return None


def main():
    if not os.path.exists(EXE_PATH):
        print(f"Erreur: L'exécutable n'est pas trouvé à : {EXE_PATH}")
        return

    print(f"[*] Démarrage du Brute-Force sur {EXE_PATH}")
    print(f"[*] Alphabet : {len(ALPHABET)} caractères")
    print(f"[*] Threads  : {WORKERS}")

    found = False

    # On boucle sur les tailles 1, 2, puis 3
    for length in range(1, MAX_LEN + 1):
        if found: break

        total_combos = len(ALPHABET) ** length
        print(f"\n--- Test Longueur {length} ({total_combos} combinaisons) ---")

        # Générateur de combinaisons
        combos = itertools.product(ALPHABET, repeat=length)

        with ThreadPoolExecutor(max_workers=WORKERS) as executor:
            # On prépare les tâches
            futures = {executor.submit(test_candidate, "".join(c)): c for c in combos}

            # Barre de progression
            for future in tqdm(as_completed(futures), total=total_combos, unit="tests"):
                result = future.result()
                if result:
                    key, flag = result
                    print(f"\n\n[!!!] BINGO TROUVÉ [!!!]")
                    print(f"KEY  : {key}")
                    print(f"FLAG : {flag}")

                    # On arrête tout proprement
                    found = True
                    # On annule les threads restants (méthode brutale pour sortir vite)
                    executor._threads.clear()
                    concurrent.futures.thread._threads_queues.clear()
                    sys.exit(0)


if __name__ == "__main__":
    main()