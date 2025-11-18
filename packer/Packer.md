# PE Packer

## Vue d'ensemble

PE Packer est un outil de compression et de chiffrement d'exécutables Windows (format PE). Il crée un stub auto-extractible qui décompresse et exécute automatiquement le programme original en mémoire via un fichier temporaire.

**Caractéristiques :**
- Compression RLE (Run-Length Encoding)
- Chiffrement XOR avec clé 128 bits
- Génération automatique d'un stub unpacker
- Support des arguments en ligne de commande
- Compatible Windows 7+

---

## 1. Utilisation

### 1.1 Compilation

Compilez le packer avec GCC (MinGW sur Windows) :

```bash
g++ -O2 -o packer.exe packer.cpp
```

### 1.2 Syntaxe de base

```bash
packer.exe <input.exe> [OPTIONS]
```

**Options disponibles :**
- `-o <file>` : Spécifie le nom du fichier de sortie

### 1.3 Exemples d'utilisation

**Exemple 1 : Pack avec nom automatique**
```bash
packer.exe program.exe
```
Crée `program_packed.exe`

**Exemple 2 : Pack avec nom personnalisé**
```bash
packer.exe program.exe -o compressed.exe
```
Crée `compressed.exe`

### 1.4 Exécution du fichier packé

Le fichier packé s'exécute comme l'original :

```bash
# Sans arguments
compressed.exe
```

---

## 2. Fonctionnement technique

### 2.1 Architecture globale

Le processus se déroule en 4 phases :

```
[Fichier original] → [Compression RLE] → [Chiffrement XOR] → [Injection dans stub] → [Exécutable packé]
```

### 2.2 Phase 1 : Lecture et validation

```cpp
bool processFile(const std::string& filePath)
```

**Actions :**
1. Lecture complète du fichier d'entrée en mémoire
2. Vérification de la signature MZ (0x5A4D) du format PE
3. Stockage dans le vecteur `inputFile`

**Validation :**
- Vérifie que le fichier est un exécutable Windows valide
- Rejette les fichiers non-PE

### 2.3 Phase 2 : Compression RLE

```cpp
std::vector<BYTE> compressRLE(const std::vector<BYTE>& input)
```

**Algorithme RLE (Run-Length Encoding) :**

La compression détecte les séquences répétitives de bytes identiques.

**Format de compression :**
- Séquence répétée (≥4 occurrences) : `[0xFF] [count] [byte]`
- Byte unique : `[byte]`
- Byte 0xFF isolé : `[0xFF] [0x00]`

**Exemple :**
```
Données originales : AA AA AA AA AA BB CC
Données compressées : [0xFF] [05] [AA] [BB] [CC]
```

**Optimisation :**
- Seulement les séquences de 4+ bytes identiques sont compressées
- Les séquences plus courtes restent non compressées
- Maximum 255 répétitions par séquence

### 2.4 Phase 3 : Chiffrement XOR

```cpp
void encryptXOR(std::vector<BYTE>& data, const uint32_t key[4])
```

**Clé de chiffrement par défaut :**
```
key[0] = 0x12345678
key[1] = 0x9ABCDEF0
key[2] = 0xFEDCBA98
key[3] = 0x87654321
```

**Algorithme :**
1. La clé 128 bits (4 × 32 bits) est convertie en 16 bytes
2. Chaque byte des données est XOR avec un byte de la clé
3. La clé est répétée cycliquement sur toute la longueur

**Formule :**
```
data[i] = data[i] XOR key[i % 16]
```

**Exemple :**
```
Données : [0x42, 0x43, 0x44, ...]
Clé :     [0x78, 0x56, 0x34, ...]
Résultat: [0x3A, 0x15, 0x70, ...]
```

### 2.5 Phase 4 : Structure de la section packed

```cpp
#pragma pack(push, 1)
struct PackedSection {
    DWORD magic;             // 0x4B435041 ("PACK")
    DWORD unpacked_size;     // Taille originale
    DWORD packed_size;       // Taille compressée
    DWORD key[4];            // Clé XOR (128 bits)
    // Données chiffrées suivent immédiatement
};
```

**Layout en mémoire :**
```
[Header: 24 bytes]
  ├─ Magic: 4 bytes (0x4B435041)
  ├─ Unpacked size: 4 bytes
  ├─ Packed size: 4 bytes
  └─ XOR key: 16 bytes

[Payload chiffré: packed_size bytes]
```

### 2.6 Phase 5 : Génération du stub

```cpp
void generateStubSource(const char* outputPath)
```

**Le stub généré contient :**

1. **Lecture de lui-même :**
    - Obtient son propre chemin via `GetModuleFileName()`
    - Mappe le fichier en mémoire (memory-mapped file)

2. **Localisation de la section .packed :**
    - Parse les headers PE (DOS + NT)
    - Recherche la section nommée ".packed"

3. **Validation :**
    - Vérifie le magic number (0x4B435041)
    - Valide les tailles

4. **Déchiffrement :**
   ```c
   void decryptXOR(unsigned char* data, DWORD size, DWORD* key)
   ```
    - Applique XOR avec la clé stockée dans le header

5. **Décompression RLE :**
   ```c
   DWORD decompressRLE(unsigned char* input, DWORD inputSize, 
                       unsigned char* output, DWORD outputSize)
   ```
    - Décode le format RLE : `[0xFF][count][byte]` → répétitions
    - Restaure les données originales

6. **Extraction temporaire :**
    - Crée un fichier dans `%TEMP%` avec extension .exe
    - Écrit l'exécutable décompressé

7. **Exécution :**
    - Lance le programme via `CreateProcess()`
    - Transmet tous les arguments de ligne de commande
    - Attend la fin du processus (`WaitForSingleObject`)
    - Supprime le fichier temporaire
    - Retourne le code de sortie du programme

### 2.7 Phase 6 : Compilation du stub

```cpp
bool compileStub(const std::string& stubExePath)
```

**Processus :**
1. Génère le code source C dans `%TEMP%\stub_source.c`
2. Compile avec GCC :
   ```bash
   gcc -O2 -s -o unpacker_stub.exe stub_source.c
   ```
3. Options de compilation :
    - `-O2` : Optimisation niveau 2
    - `-s` : Strip symbols (réduit la taille)
4. Vérifie que le stub a été créé correctement
5. Gère les erreurs de compilation

### 2.8 Phase 7 : Injection des données

```cpp
bool injectPackedData(const std::string& stubExePath,
                      const std::vector<BYTE>& packedData,
                      const std::string& outputPath)
```

**Opérations :**

1. **Lecture du stub compilé** en mémoire

2. **Modification des headers PE :**
    - Calcule l'adresse de la nouvelle section
    - Aligne sur `SectionAlignment` et `FileAlignment`

3. **Création de la section .packed :**
   ```cpp
   IMAGE_SECTION_HEADER newSection;
   memcpy(newSection.Name, ".packed", 7);
   newSection.VirtualAddress = aligned_address;
   newSection.SizeOfRawData = aligned_size;
   newSection.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
   ```

4. **Mise à jour des headers :**
    - Incrémente `NumberOfSections`
    - Augmente `SizeOfImage`

5. **Construction du fichier final :**
    - Headers PE modifiés
    - Sections originales du stub
    - Nouvelle section .packed avec les données
    - Padding d'alignement

6. **Écriture du fichier de sortie**

---

## 3. Structure du fichier packé

### 3.1 Layout global

```
┌─────────────────────────────────┐
│   Headers PE (DOS + NT)         │
├─────────────────────────────────┤
│   Section .text (code stub)     │
├─────────────────────────────────┤
│   Section .data (data stub)     │
├─────────────────────────────────┤
│   Section .rdata (imports)      │
├─────────────────────────────────┤
│   Section .packed               │
│   ┌─────────────────────────┐   │
│   │  PackedSection header   │   │
│   │  (24 bytes)             │   │
│   ├─────────────────────────┤   │
│   │  Payload chiffré/compressé │
│   │  (packed_size bytes)    │   │
│   └─────────────────────────┘   │
└─────────────────────────────────┘
```

### 3.2 Alignement PE

**Section Alignment :** 4096 bytes (0x1000)
- Alignement des sections en mémoire virtuelle

**File Alignment :** 512 bytes (0x200)
- Alignement des sections sur disque

---

## 4. Sécurité et limitations

### 4.1 Forces

✅ **Compression efficace** pour les données répétitives
✅ **Chiffrement simple** empêche l'analyse statique basique
✅ **Préservation des arguments** en ligne de commande
✅ **Nettoyage automatique** des fichiers temporaires

### 4.2 Limitations

⚠️ **Détection antivirus possible** (extraction via fichier temporaire)
⚠️ **Chiffrement XOR faible** (clé statique, facilement cassable)
⚠️ **Compression RLE limitée** (inefficace sur données aléatoires)
⚠️ **Fichier temporaire visible** pendant l'exécution
⚠️ **Pas d'obfuscation du code** du programme original

### 4.3 Améliorations possibles

💡 Clé XOR aléatoire par exécutable
💡 Compression LZMA ou Deflate
💡 Exécution directe en mémoire (process hollowing)
💡 Anti-debug / anti-VM
💡 Chiffrement AES

---

## 5. Débogage

### 5.1 Messages d'erreur du packer

| Message | Cause | Solution |
|---------|-------|----------|
| `Invalid PE file!` | Fichier d'entrée non-PE | Vérifier que c'est un .exe Windows |
| `Failed to compile stub` | GCC non trouvé | Installer MinGW et ajouter au PATH |
| `Could not create output file` | Permissions insuffisantes | Lancer en administrateur |

### 5.2 Messages d'erreur du stub

| Message | Cause | Solution |
|---------|-------|----------|
| `.packed section not found` | Section manquante | Recompiler avec le packer |
| `Invalid magic` | Données corrompues | Vérifier l'intégrité du fichier |
| `Decompression failed` | Erreur RLE | Données endommagées |
| `Cannot start process` | Fichier temp invalide | Vérifier les droits sur %TEMP% |

---

## 6. Références techniques

### 6.1 Format PE
- [Microsoft PE/COFF Specification](https://docs.microsoft.com/en-us/windows/win32/debug/pe-format)

### 6.2 Compression RLE
- Algorithme simple, adapté aux données répétitives
- Ratio typique : 30-70% selon le contenu

### 6.3 API Windows utilisées
- `CreateFileMapping()` / `MapViewOfFile()` : Memory-mapped files
- `CreateProcess()` : Création de processus
- `VirtualAlloc()` : Allocation mémoire
- `GetTempPath()` / `GetTempFileName()` : Fichiers temporaires

---

## Licence et avertissement

⚠️ **Usage éducatif uniquement**

Ce code est fourni à des fins d'apprentissage sur :
- La structure des fichiers PE
- Les techniques de compression/chiffrement
- La manipulation de binaires Windows

**Ne pas utiliser pour :**
- Contourner des antivirus
- Distribuer des malwares
- Violer des licences logicielles

L'utilisation malveillante est de votre responsabilité.