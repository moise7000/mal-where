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



.