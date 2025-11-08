# Hash Function - SHA-256 Implementation

## Objectif

Implémenter une fonction de hachage SHA-256 qui permet à un attaquant de reverser un hash pour retrouver la clé originale.

**Scénario** :
- L'attaquant obtient un digest `X`
- L'attaquant connaît la fonction de hachage `H` utilisée
- L'attaquant cherche la clé `K` telle que `X = H(K)`
- Avec la connaissance de `H`, l'attaquant peut utiliser **hashcat** pour retrouver `K`

## Fichiers

### `hash_function.cpp`
Implémentation complète de SHA-256 selon FIPS 180-4 :
- Structure `Digest` pour stocker les résultats
- Fonction `sha256()` : implémentation complète de l'algorithme
- Fonction `hash_function()` : interface pour l'attaquant
- Fonction `verify_hash()` : vérification d'un hash

### `test_hash_function.cpp`
Suite de tests complète :
- **Partie 1** : Génération de digests pour différentes clés
- **Partie 2** : Vérification de hash
- **Partie 3** : Simulation d'attaque par dictionnaire
- **Partie 4** : Instructions pour utiliser hashcat

## Compilation

```bash
g++ -Wall -Wextra -std=c++03 -o test_hash_function test_hash_function.cpp
```

Compatible avec GCC 4.4.7 (C++03).

## Exécution

```bash
./test_hash_function
```

## Utilisation avec Hashcat

### 1. Créer un fichier de hashes

```bash
echo '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8' > hashes.txt
```

### 2. Attaque par dictionnaire

```bash
hashcat -m 1400 -a 0 hashes.txt wordlist.txt
```

**Options** :
- `-m 1400` : Mode SHA-256
- `-a 0` : Attaque par dictionnaire
- `wordlist.txt` : Fichier contenant des mots de passe potentiels

### 3. Attaque par force brute

```bash
# 8 caractères alphanumériques
hashcat -m 1400 -a 3 hashes.txt ?a?a?a?a?a?a?a?a

# 6 chiffres
hashcat -m 1400 -a 3 hashes.txt ?d?d?d?d?d?d

# 4 lettres minuscules
hashcat -m 1400 -a 3 hashes.txt ?l?l?l?l
```

**Masques** :
- `?l` : lettre minuscule (a-z)
- `?u` : lettre majuscule (A-Z)
- `?d` : chiffre (0-9)
- `?s` : caractère spécial
- `?a` : tous les caractères

### 4. Vérifier les résultats

```bash
hashcat -m 1400 --show hashes.txt
```

## Exemples de Hashes

| Clé              | SHA-256 Hash                                                     |
|------------------|------------------------------------------------------------------|
| `password`       | `5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8` |
| `123456`         | `8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92` |
| `admin`          | `8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918` |
| (chaîne vide)    | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |

## Fonctionnement de l'attaque

1. **L'attaquant récupère le digest** `X` (par exemple lors d'une fuite de données)
2. **L'attaquant identifie l'algorithme** `H` (ici SHA-256)
3. **L'attaquant utilise hashcat** avec des dictionnaires ou force brute
4. **Hashcat teste des millions de combinaisons** jusqu'à trouver `K` tel que `H(K) = X`
5. **L'attaquant récupère la clé originale** `K`

## Sécurité

⚠️ **Cette implémentation est à des fins éducatives uniquement**

Dans un contexte réel :
- Utilisez des **fonctions de dérivation de clés** (PBKDF2, bcrypt, Argon2)
- Ajoutez un **salt** unique pour chaque mot de passe
- Utilisez plusieurs **itérations** pour ralentir les attaques
- Ne stockez **jamais** les mots de passe en clair

## Références

- [FIPS 180-4 : SHA-256 Specification](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
- [Hashcat Documentation](https://hashcat.net/wiki/)
- [OWASP Password Storage](https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html)
