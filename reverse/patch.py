import hashlib
import shutil
import os

# --- CONFIGURATION ---
FILENAME = "main.exe"  # Le fichier original
OUTPUT_NAME = "main_patched.exe"  # Le fichier modifié
OLD_HASH = "bea8e217036cb3b738e207fe5d40266828bc1969fd8538d533ea39f4e40ffc8f"

# Votre nouveau mot de passe désiré
NEW_PASSWORD = "1234"


def patch_binary():
    if not os.path.exists(FILENAME):
        print(f"[-] Erreur : {FILENAME} introuvable.")
        return

    # 1. Calculer le nouveau hash
    print(f"[*] Nouveau mot de passe choisi : '{NEW_PASSWORD}'")
    new_hash = hashlib.sha256(NEW_PASSWORD.encode()).hexdigest()
    print(f"[*] SHA-256 correspondant     : {new_hash}")

    # 2. Lire le fichier binaire
    print(f"[*] Lecture de {FILENAME}...")
    with open(FILENAME, "rb") as f:
        data = f.read()

    # 3. Chercher l'ancien hash (en bytes ASCII)
    old_hash_bytes = OLD_HASH.encode('ascii')
    new_hash_bytes = new_hash.encode('ascii')

    # Vérifier que le hash existe
    count = data.count(old_hash_bytes)
    if count == 0:
        print("[-] Erreur : L'ancien hash n'a pas été trouvé dans le fichier.")
        return
    elif count > 1:
        print(f"[!] Attention : L'ancien hash apparaît {count} fois. Tout sera remplacé.")

    # 4. Remplacer
    print("[*] Application du patch...")
    new_data = data.replace(old_hash_bytes, new_hash_bytes)

    # 5. Écrire le nouveau fichier
    with open(OUTPUT_NAME, "wb") as f:
        f.write(new_data)

    print(f"[+] Succès ! Fichier créé : {OUTPUT_NAME}")
    print(f"[i] Vous pouvez maintenant entrer '{NEW_PASSWORD}' pour valider le crackme.")


if __name__ == "__main__":
    patch_binary()