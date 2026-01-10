# Résumé Fonctionnel : Projet Malware - Groupe 3

**Auteurs :** Ewan Decima, Garance Frolla, Ely Marthouret

Ce projet est un exécutable hybride capable d'agir comme un logiciel légitime (*goodware*) ou malveillant (*malware*) selon l'entrée utilisateur, protégé par un packer personnalisé.

---

### 1. Mécanisme de Routage (Point d'entrée)
Le programme analyse la longueur de la chaîne de caractères en entrée :
* **Mode "Goodware"** : Activé si la chaîne est courte (< 8 caractères).
* **Mode "Malware"** : Activé si la chaîne est longue (≥ 8 caractères).

### 2. Mode "Goodware" (Fonction Custom)
Ce mode simule un validateur de mot de passe complexe pour masquer son comportement réel :

* **Anti-Analyse** : Détection de débogueur qui déclenche de fausses récursions pour leurrer l'analyste.
* **Fonction de Hachage Personnalisée (`custom_hash`)** : Utilise un algorithme dérivé de DJB2 avec des rotations de bits spécifiques (gauche pour les index pairs, droite pour les impairs) et un masquage final (XOR `0xDEADBEEF`) pour produire un hash unique sur 32 bits.
* **Validation Impossible (Leurre)** :
    * Le programme compare le hash calculé (32 bits) avec une constante secrète hardcodée (64 caractères, type SHA-256).
    * **Intérêt** : Cette comparaison est **mathématiquement impossible** en raison de la différence de longueur. Le code de "Victoire" (déchiffrement du flag) est donc inatteignable.
    * **Résultat** : Le programme emprunte toujours la branche "échec" de la validation, qui consiste simplement à afficher la chaîne d'entrée (comportement normal d'un "echo"). Cela rend le code d'apparence suspecte mais fonctionnellement inoffensif.

### 3. Mode "Malware" (Fonction Devil)
Séquence destructrice en trois étapes :
1.  **Rickroll** : Animation ASCII dans le terminal.
2.  **Création de Zipbomb** : Génération d'un fichier compressé contenant **1 To** de données (10 niveaux de récursion).
3.  **Détonation** : Décompression récursive saturant disque et mémoire jusqu'au crash.

### 4. Le Packer (Protection et Déploiement)
Enveloppe l'exécutable pour le dissimuler :
* **Chiffrement** : Payload compressé (RLE) et chiffré (XOR 128-bit).
* **Process Hollowing** : Injection du code malveillant dans un processus suspendu légitime.
* **Camouflage** : Conservation des métadonnées originales pour paraître sain.

### 5. Techniques de Leurre
* **Social Engineering** : Inclusion de liens vers un faux GitHub et de faux symboles de debug (.pdp) pour égarer les analystes.

---
*Ce projet est destiné uniquement à l'apprentissage de la sécurité informatique.*