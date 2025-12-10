import struct
import os
import shutil

def get_file_offset(pe_file, virtual_address):
    """
    Convertit une Virtual Address (VA) en File Offset (Raw Address)
    en analysant les sections du fichier PE.
    """
    pe_file.seek(0x3C)  # Position de l'entête PE
    pe_header_offset = struct.unpack('<I', pe_file.read(4))[0]
    
    # Vérification signature PE
    pe_file.seek(pe_header_offset)
    if pe_file.read(4) != b'PE\x00\x00':
        raise Exception("Ce n'est pas un fichier PE valide (.exe)")

    # Lecture du File Header pour avoir le nombre de sections
    pe_file.seek(pe_header_offset + 4 + 2) # Skip Machine
    num_sections = struct.unpack('<H', pe_file.read(2))[0]
    
    # Lecture de l'Optional Header pour avoir l'ImageBase (taille standard headers optionnels)
    pe_file.seek(pe_header_offset + 24) # Skip File Header
    magic = struct.unpack('<H', pe_file.read(2))[0] # 0x10b for PE32, 0x20b for PE32+
    
    # On lit l'ImageBase
    if magic == 0x10b:
        pe_file.seek(pe_header_offset + 24 + 28)
        image_base = struct.unpack('<I', pe_file.read(4))[0]
    elif magic == 0x20b:
        pe_file.seek(pe_header_offset + 24 + 24)
        image_base = struct.unpack('<Q', pe_file.read(8))[0]
    else:
        raise Exception("Format PE inconnu")

    # Calcul de la Relative Virtual Address (RVA) cible
    target_rva = virtual_address - image_base
    
    # Trouver la table des sections (après les headers)
    optional_header_size = struct.unpack('<H', pe_file.read(2))[0] # Read SizeOfOptionalHeader
    # Correction: il faut relire le SizeOfOptionalHeader au bon endroit
    pe_file.seek(pe_header_offset + 20)
    optional_header_size = struct.unpack('<H', pe_file.read(2))[0]
    
    section_table_offset = pe_header_offset + 24 + optional_header_size
    pe_file.seek(section_table_offset)

    # Parcourir les sections pour trouver celle qui contient notre adresse
    for i in range(num_sections):
        # Format Section Header: Name(8), VirtualSize(4), VirtualAddress(4), SizeOfRawData(4), PointerToRawData(4)...
        pe_file.seek(section_table_offset + (i * 40))
        sec_name = pe_file.read(8)
        virtual_size, virtual_addr, size_raw, ptr_raw = struct.unpack('<IIII', pe_file.read(16))
        
        # Est-ce que notre RVA est dans cette section ?
        if virtual_addr <= target_rva < (virtual_addr + max(virtual_size, size_raw)):
            print(f"[+] Adresse trouvée dans la section : {sec_name.strip(b'x00').decode()}")
            # Calcul : Offset = RVA - Section_VA + Section_Raw_Pointer
            file_offset = target_rva - virtual_addr + ptr_raw
            return file_offset

    return None

def patch_exe(filename, target_va):
    print(f"--- Patching {filename} ---")
    
    # 1. Sauvegarde
    backup_name = filename + ".bak"
    shutil.copyfile(filename, backup_name)
    print(f"[i] Sauvegarde créée : {backup_name}")

    try:
        with open(filename, 'r+b') as f:
            # 2. Calculer l'offset
            offset = get_file_offset(f, target_va)
            
            if offset is None:
                print("[-] Impossible de convertir l'adresse virtuelle en offset fichier.")
                return

            print(f"[i] Adresse Virtuelle : {hex(target_va)}")
            print(f"[i] Offset Fichier    : {hex(offset)}")

            # 3. Analyser l'instruction à cet endroit
            f.seek(offset)
            opcode = f.read(2)
            f.seek(offset) # Revenir

            # Identifier le type de saut (JZ)
            # 0F 84 = JZ Long (Near) -> 6 octets
            # 74    = JZ Court (Short) -> 2 octets
            
            patch_size = 0
            
            if opcode[0] == 0x0F and opcode[1] == 0x84:
                print("[+] Instruction identifiée : JZ NEAR (0F 84 ...)")
                patch_size = 6
            elif opcode[0] == 0x74:
                print("[+] Instruction identifiée : JZ SHORT (74 ...)")
                patch_size = 2
            else:
                print(f"[-] Attention ! Les octets trouvés ({opcode.hex()}) ne ressemblent pas à un JZ (0F84 ou 74).")
                confirm = input("Voulez-vous forcer le patch quand même avec 2 NOPs ? (o/n) : ")
                if confirm.lower() != 'o':
                    print("Annulation.")
                    return
                patch_size = 2 # Défaut risqué

            # 4. Appliquer le Patch (NOP = 0x90)
            print(f"[+] Application de {patch_size} NOPs...")
            f.write(b'\x90' * patch_size)
            print("[+] Patch terminé avec succès !")
            
    except Exception as e:
        print(f"[-] Erreur : {e}")



FILENAME = "main.exe"
TARGET_ADDRESS = 0x004053CF

if __name__ == "__main__":
    if not os.path.exists(FILENAME):
        print(f"Erreur: Le fichier {FILENAME} n'existe pas.")
        print("Modifiez la variable FILENAME dans le script.")
    else:
        patch_exe(FILENAME, TARGET_ADDRESS)
