# 🛡️ Anti-Debug Namespace

Utilitaires de détection de débogage et d'analyse pour **Windows 7+** (compatible MSVC 2010+ / MinGW, C++03).

## Description

Namespace `anti_debug` contenant des fonctions pour détecter :
- **Debuggers** (OllyDbg, x64dbg, WinDbg, IDA Pro)
- **Instrumentation** (Frida, hooks, DLL injection)
- **Modifications** du binaire (checksums)
- **Timing attacks** (détection de stepping)

## Fonctions disponibles

### Détection de debugger

```cpp
bool is_debugger_present();
```
Détection multi-méthodes :
- `IsDebuggerPresent()` API
- `CheckRemoteDebuggerPresent()` API  
- PEB (Process Environment Block) `BeingDebugged` flag

**Détecte** : OllyDbg, x64dbg, WinDbg, Visual Studio debugger

```cpp
bool is_debugger_present_ptrace();
```
Windows : Vérifie le flag `NtGlobalFlag` dans le PEB.  
Détecte les heap flags de debug (FLG_HEAP_ENABLE_TAIL_CHECK, etc.)

###  Détection de timing

Détecte si le programme est exécuté en mode pas-à-pas (stepping) via des anomalies de timing.

**Méthodes utilisées** :
- `RDTSC` (Read Time-Stamp Counter) - instruction CPU
- `GetTickCount()` - timer Windows

###  Processus parent

```cpp
bool parent_process_suspicious();
```
Inspecte le processus parent via `CreateToolhelp32Snapshot()`.  
**Détecte** :
- ollydbg.exe
- x64dbg.exe / x32dbg.exe
- windbg.exe
- ida.exe / ida64.exe
- ImmunityDebugger.exe
- devenv.exe (Visual Studio)

### Détection d'instrumentation

```cpp
bool env_suspicious();
```
Vérifie les variables d'environnement suspectes :
- `_NT_SYMBOL_PATH`
- `_NT_ALT_SYMBOL_PATH`
- `COR_ENABLE_PROFILING`

```cpp
bool loaded_objects_suspicious();
```
Scanne les modules chargés via `CreateToolhelp32Snapshot()`.  
**Détecte** :
- frida DLLs
- x64dbg/OllyDbg plugins
- MinHook (hooking library)
- Detours (hooking library)

```cpp
bool has_preload();
```
Simplifié sur Windows (toujours false actuellement).

### Vérification d'intégrité

```cpp
unsigned long checksum_self(unsigned long* out_size);
```
Calcule l'Adler-32 du fichier .exe via `GetModuleFileName()`.  
Détecte si le binaire a été modifié sur disque.

```cpp
unsigned long text_segments_checksum(unsigned long* out_size);
```
Calcule l'Adler-32 des sections exécutables (`.text`, `.code`) en mémoire.  
**Détecte** :
- Breakpoints logiciels (0xCC)
- Patches en mémoire
- Code injection

## Compilation

### Visual Studio (MSVC)

```cmd
cl /EHsc /c anti_debug.cpp
```

### MinGW (GCC sur Windows)

```cmd
g++ -c anti_debug.cpp -std=c++03 -mwindows
```

### Projet complet

```cmd
REM Avec Visual Studio
cl /EHsc test_anti_debug.cpp anti_debug.cpp /link psapi.lib

REM Avec MinGW
g++ -o test_anti_debug.exe test_anti_debug.cpp anti_debug.cpp -lpsapi -std=c++03
```

**Note** : Lier avec `psapi.lib` (PSAPI = Process Status API)

## Exemple d'utilisation

```cpp
#include "anti_debug.h"
#include <iostream>

int main() {
    // Méthode 1: Debugger présent ?
    if (anti_debug::is_debugger_present()) {
        std::cout << "Debugger detecte!" << std::endl;
        return 1;
    }
    
    // Méthode 2: PEB NtGlobalFlag
    if (anti_debug::is_debugger_present_ptrace()) {
        std::cout << "Heap flags de debug detectes!" << std::endl;
        return 1;
    }
    
    // Méthode 3: Processus parent suspect
    if (anti_debug::parent_process_suspicious()) {
        std::cout << "Lance depuis un debugger!" << std::endl;
        return 1;
    }
    
    // Méthode 4: Modules suspects chargés
    if (anti_debug::loaded_objects_suspicious()) {
        std::cout << "DLLs suspectes detectees!" << std::endl;
        return 1;
    }
    
    // Méthode 5: Checksum d'intégrité
    unsigned long size = 0;
    unsigned long chk = anti_debug::checksum_self(&size);
    std::cout << "Checksum: 0x" << std::hex << chk 
              << " (taille: " << std::dec << size << " bytes)" << std::endl;
    
    // Checksum des sections .text
    unsigned long textChk = anti_debug::text_segments_checksum(&size);
    std::cout << "Text checksum: 0x" << std::hex << textChk 
              << " (" << std::dec << size << " bytes)" << std::endl;
    
    // Programme normal...
    std::cout << "Programme securise en cours d'execution" << std::endl;
    
    return 0;
}
```

## Techniques Windows

### IsDebuggerPresent()
API Windows standard. Vérifie le flag `BeingDebugged` dans le PEB.

### CheckRemoteDebuggerPresent()
Vérifie si un debugger est attaché au processus (même à distance).

### PEB (Process Environment Block)
Structure kernel accessible via :
- `fs:[0x30]` (x86)
- `gs:[0x60]` (x64)

**Champs vérifiés** :
- `BeingDebugged` (offset 0x02)
- `NtGlobalFlag` (offset 0x68 x86 / 0xBC x64)

### RDTSC (Read Time-Stamp Counter)
Instruction CPU qui lit le nombre de cycles depuis le boot.  
**Détection** : Si trop de cycles entre deux mesures → stepping.

### CreateToolhelp32Snapshot()
API pour énumérer :
- Processus (`TH32CS_SNAPPROCESS`)
- Modules/DLLs (`TH32CS_SNAPMODULE`)

### PE Headers
Parcours des sections du Portable Executable :
- `IMAGE_DOS_HEADER`
- `IMAGE_NT_HEADERS`
- `IMAGE_SECTION_HEADER`

Vérification des sections avec flag `IMAGE_SCN_MEM_EXECUTE`.

## Contournements possibles

 **Ces techniques peuvent être contournées** :

| Technique | Contournement |
|-----------|---------------|
| `IsDebuggerPresent()` | Hook/patch de l'API |
| PEB `BeingDebugged` | Patch du flag en mémoire |
| `NtGlobalFlag` | Restaurer les heap flags normaux |
| RDTSC | Virtualisation du timestamp |
| Parent process | Lancer via un processus légitime |
| Checksums | Restaurer le code original avant vérification |

**Défense en profondeur** : Utiliser PLUSIEURS techniques simultanément.

## Debuggers Windows courants

| Debugger | Détecté par |
|----------|-------------|
| **OllyDbg** | `is_debugger_present()`, `parent_process_suspicious()` |
| **x64dbg/x32dbg** | `is_debugger_present()`, `parent_process_suspicious()`, `loaded_objects_suspicious()` |
| **WinDbg** | `is_debugger_present()`, `parent_process_suspicious()` |
| **IDA Pro** | `parent_process_suspicious()` |
| **Visual Studio** | `is_debugger_present()`, `parent_process_suspicious()` |
| **Frida** | `loaded_objects_suspicious()` (frida-agent.dll) |

## Compatibilité

- ✅ Windows 7 (testé)
- ✅ Windows 8/8.1
- ✅ Windows 10/11
- ✅ MSVC 2010+ (Visual Studio)
- ✅ MinGW (GCC sur Windows)
- ✅ x86 (32-bit)
- ✅ x64 (64-bit)

## Fichiers

```
obfuscation_methods/anti_debug/
├── anti_debug.h      # Déclarations du namespace
├── anti_debug.cpp    # Implémentations (Windows)
└── README.md         # Ce fichier
```

## Références
---

**Projet** : mal-where (malware course)  
**Branch** : 39-create-an-antidebug-namespace  
**Platform** : Windows 7+  
**Compatible** : MSVC 2010+ / MinGW (C++03)
