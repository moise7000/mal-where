import base64
import binascii
import math
from collections import Counter

def calculate_entropy(data: bytes) -> float:
    """Calcule l'entropie de Shannon des données."""
    if len(data) == 0:
        return 0.0

    counter = Counter(data)
    length = len(data)
    entropy = 0.0

    for count in counter.values():
        probability = count / length
        entropy -= probability * math.log2(probability)

    return entropy



def is_base64(data: bytes) -> bool:
    """Vérifie si les données sont du Base64 valide."""
    clean = b"".join(data.split())
    if len(clean) == 0:
        return False
    if len(clean) % 4 != 0:
        return False
    try:
        base64.b64decode(clean, validate=True)
        return True
    except binascii.Error:
        return False

def successive_decode(input_file, output_file):
    """Décode successivement le Base64 jusqu'à trouver du texte français."""
    with open(input_file, "rb") as f:
        data = f.read()

    iteration = 0

    while True:
        clean = b"".join(data.split())

        try:
            decoded = base64.b64decode(clean)
        except Exception:
            print(f"[!] Impossible de décoder à l'itération #{iteration + 1}")
            break

        iteration += 1
        entropy = calculate_entropy(decoded)
        is_french = entropy <= 4.1

        print(f"\n[+] Décodage #{iteration}")
        print(f"    Taille : {len(decoded)} octets")
        print(f"    Entropie : {entropy:.4f} bits/octet")
        print(f"    Texte français détecté : {'✓ OUI' if is_french else '✗ Non'}")

        # Affiche un aperçu si c'est du texte
        try:
            preview = decoded[:100].decode('utf-8', errors='ignore')
            if preview.isprintable() or any(c in preview for c in '\n\r\t'):
                print(f"    Aperçu : {preview[:80]}...")
        except:
            pass

        # Arrêt si texte français détecté
        if is_french:
            print(f"\n[✓] Texte français détecté ! Arrêt du décodage.")
            data = decoded
            break

        # Si ce n'est plus du base64, on arrête
        if not is_base64(decoded):
            print(f"[!] Le résultat n'est plus du Base64 valide")
            data = decoded
            break

        # Continue avec la valeur décodée
        data = decoded

    # Écriture du résultat final
    with open(output_file, "wb") as out:
        out.write(data)

    print(f"\n{'='*60}")
    print(f"[✓] Résultat final écrit dans : {output_file}")
    print(f"[✓] Nombre de décodages effectués : {iteration}")
    print(f"{'='*60}")


# Exécution
successive_decode("base64.txt", "final_output.bin")