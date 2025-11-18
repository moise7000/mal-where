# 😈 Devil Code - Zip Bombs & Malicious Echo

Code de démonstration pour cours de malwares (3A).

## Description

- **Zip bombs** : Compression imbriquée (50 MB → 1 GB compressés en ~200-280 bytes)
- **Malicious echo** : Appelle `devil()` si input < 8 caractères
- **Devil code** : Rickroll + création zipbomb + détonation (DANGEREUX)

## Compilation

```bash
# Programme de test (création de bombs + tests + devil code)
g++ -o test_devil test_devil.cpp DevilCode.cpp Rickroll.cpp zipbomb.cpp ../packer/Compressor.cpp -lz -std=c++03
```

## Utilisation

```bash
./test_devil
```

Le programme exécute 3 tests :

1. **Création de zip bombs** (safe) - Crée 4 fichiers :
   - `test_zipbomb_50mb_5levels.bin` (~220 bytes)
   - `test_zipbomb_100mb_6levels.bin` (~230 bytes)
   - `test_zipbomb_500mb_7levels.bin` (~250 bytes)
   - `test_zipbomb_1gb_8levels.bin` (~280 bytes)

2. **Malicious echo** (safe) - Teste l'echo conditionnel

3. **Devil code** (DANGEREUX) - Demande confirmation avant de :
   - Lancer le rickroll
   - Créer une zip bomb (1 GB)
   - **DÉTONER la zip bomb**

⚠️ **VM avec snapshot obligatoire pour le test 3 !**

## Technique

**Compression imbriquée** : Compression de données déjà compressées

1. 1 GB de zéros → ~1 MB (zlib niveau 9)
2. 1 MB → ~1 KB (re-compression)
3. 1 KB → ~100 bytes (re-re-compression)
4. ... (5-8 niveaux selon le fichier)

Ratio final : **~3-5 millions : 1**

## Fichiers

```
devil/
├── DevilCode.cpp          # Fonction devil (rickroll + zipbomb + détonation)
├── DevilCode.h            # Header fonction devil
├── zipbomb.cpp            # Création et détonation zip bombs
├── zipbomb.h              # Headers zip bombs
├── test_devil.cpp         # Programme principal (tests + validation)
├── MaliciousEcho.cpp      # Echo malveillant
├── MaliciousEcho.h
├── Rickroll.cpp           # Rickroll
├── Rickroll.h
└── README.md              # Ce fichier
```

### DevilCode.cpp / .h

Fonction `devil()` qui exécute séquentiellement :
1. Rickroll (100 frames)
2. Création d'une zip bomb (1 GB, 8 niveaux)
3. **Détonation** de la zip bomb (DANGEREUX)

### zipbomb.cpp / .h

- `create_zipbomb()` : Crée des zip bombs avec compression imbriquée
- `detonate_zipbomb()` : Décompresse récursivement (DANGEREUX)

### test_devil.cpp

Programme principal avec 3 tests et validation avant le devil code.

### MaliciousEcho.cpp / .h

Template de comportement conditionnel :
- Si input < 8 caractères → appelle `devil()`
- Sinon → appelle la fonction d'écho normale

### Rickroll.cpp / .h

Animation ASCII rickroll (149 frames, désactivée pour les tests).

## Sécurité

⚠️ **NE JAMAIS EXÉCUTER EN PRODUCTION**

- Utiliser uniquement dans VM avec snapshot
- Peut remplir complètement la RAM et swap
- Peut causer un crash système

## Références

- [Zip bomb Wikipedia](https://en.wikipedia.org/wiki/Zip_bomb)
- [42.zip](https://www.unforgettable.dk/) : 42 KB → 4.5 PB

---

**Projet** : mal-where (malware course)  
**Branche** : devil-code  
**Compatible** : GCC 4.4.7 (C++03)
