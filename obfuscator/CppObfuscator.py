#!/usr/bin/env python3
"""
Obfuscateur C++ - Renomme les variables et fonctions
Usage: python obfuscator.py input.cpp output.cpp
"""

import re
import sys
import random
import string
from pathlib import Path


class CppObfuscator:
    def __init__(self):
        self.mappings = {}
        self.counter = 0
        # Mots-clés C++ à ne pas obfusquer
        self.keywords = {
            'alignas', 'alignof', 'and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor',
            'bool', 'break', 'case', 'catch', 'char', 'char8_t', 'char16_t', 'char32_t',
            'class', 'compl', 'concept', 'const', 'consteval', 'constexpr', 'constinit',
            'const_cast', 'continue', 'co_await', 'co_return', 'co_yield', 'decltype',
            'default', 'delete', 'do', 'double', 'dynamic_cast', 'else', 'enum',
            'explicit', 'export', 'extern', 'false', 'float', 'for', 'friend', 'goto',
            'if', 'inline', 'int', 'long', 'mutable', 'namespace', 'new', 'noexcept',
            'not', 'not_eq', 'nullptr', 'operator', 'or', 'or_eq', 'private', 'protected',
            'public', 'register', 'reinterpret_cast', 'requires', 'return', 'short',
            'signed', 'sizeof', 'static', 'static_assert', 'static_cast', 'struct',
            'switch', 'template', 'this', 'thread_local', 'throw', 'true', 'try',
            'typedef', 'typeid', 'typename', 'union', 'unsigned', 'using', 'virtual',
            'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq',
            # Types standards
            'std', 'string', 'vector', 'map', 'set', 'list', 'queue', 'stack',
            'pair', 'size_t', 'uint8_t', 'int8_t', 'uint16_t', 'int16_t',
            'uint32_t', 'int32_t', 'uint64_t', 'int64_t',
            # Fonctions/méthodes standard à préserver
            'main', 'printf', 'scanf', 'cout', 'cin', 'endl',
            'size', 'length', 'push_back', 'pop_back', 'begin', 'end',
            'iterator', 'empty', 'clear', 'reserve', 'resize',
            'insert', 'erase', 'find', 'at', 'front', 'back',
            'make_pair', 'make_unique', 'make_shared',
            # Exceptions
            'runtime_error', 'logic_error', 'invalid_argument', 'out_of_range',
            'what', 'exception',
            # Autres
            'Type', 'PLAIN', 'BASE64', 'BASE32'
        }

    def generate_name(self):
        """Génère un nom obfusqué"""
        name = f"_{self._random_string(8)}"
        self.counter += 1
        return name

    def _random_string(self, length):
        """Génère une chaîne aléatoire"""
        chars = string.ascii_letters + string.digits
        return ''.join(random.choice(chars) for _ in range(length))

    def get_obfuscated_name(self, original_name):
        """Retourne le nom obfusqué (ou le crée s'il n'existe pas)"""
        if original_name in self.keywords:
            return original_name
        if original_name not in self.mappings:
            self.mappings[original_name] = self.generate_name()
        return self.mappings[original_name]

    def extract_identifiers(self, code):
        """Extrait tous les identifiants du code"""
        identifiers = set()

        # Pattern pour tout identifiant (lettres, chiffres, underscore)
        # qui commence par une lettre ou underscore
        all_identifiers = re.findall(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', code)

        for identifier in all_identifiers:
            # Ignorer les mots-clés
            if identifier not in self.keywords:
                identifiers.add(identifier)

        return identifiers

    def should_preserve(self, identifier):
        """Détermine si un identifiant doit être préservé"""
        # Préserver les mots-clés
        if identifier in self.keywords:
            return True

        # Préserver les identifiants std::
        if identifier.startswith('std::'):
            return True

        # Préserver les macros (tout en majuscules avec underscores)
        if identifier.isupper() and len(identifier) > 1:
            return True

        # Préserver les identifiants qui ressemblent à des types de base
        if identifier in ['size_t', 'int8_t', 'int16_t', 'int32_t', 'int64_t',
                          'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t']:
            return True

        return False

    def remove_comments(self, code):
        """Supprime tous les commentaires C++ (// et /* */)"""
        # D'abord protéger les chaînes pour ne pas supprimer les // ou /* */ à l'intérieur
        strings = []
        counter = 0

        def replace_string(match):
            nonlocal counter
            strings.append((counter, match.group(0)))
            placeholder = f"___TEMP_STRING_{counter}___"
            counter += 1
            return placeholder

        # Protéger les chaînes
        result = re.sub(r'"(?:[^"\\]|\\.)*"', replace_string, code)

        # Supprimer les commentaires de ligne //
        result = re.sub(r'//.*?$', '', result, flags=re.MULTILINE)

        # Supprimer les commentaires de bloc /* */
        result = re.sub(r'/\*.*?\*/', '', result, flags=re.DOTALL)

        # Restaurer les chaînes
        for i, original in strings:
            result = result.replace(f"___TEMP_STRING_{i}___", original)

        return result

    def protect_strings_and_chars(self, code):
        """Remplace temporairement les chaînes et caractères par des placeholders"""
        strings = []
        counter = 0

        # Protéger les chaînes entre guillemets doubles
        def replace_string(match):
            nonlocal counter
            strings.append((counter, match.group(0)))
            placeholder = f"___STRING_{counter}___"
            counter += 1
            return placeholder

        # Protéger les caractères entre guillemets simples
        def replace_char(match):
            nonlocal counter
            strings.append((counter, match.group(0)))
            placeholder = f"___CHAR_{counter}___"
            counter += 1
            return placeholder

        # Remplacer les chaînes "..." et les caractères '.'
        result = re.sub(r'"(?:[^"\\]|\\.)*"', replace_string, code)
        result = re.sub(r"'(?:[^'\\]|\\.)'", replace_char, result)

        return result, strings

    def restore_strings_and_chars(self, code, strings):
        """Restaure les chaînes et caractères originaux"""
        result = code
        for counter, original in strings:
            result = result.replace(f"___STRING_{counter}___", original)
            result = result.replace(f"___CHAR_{counter}___", original)
        return result

    def obfuscate(self, code):
        """Obfusque le code C++"""
        # Supprimer tous les commentaires
        code = self.remove_comments(code)

        # Protéger les chaînes et caractères littéraux
        code_protected, strings = self.protect_strings_and_chars(code)

        # Séparer les lignes #include et autres directives du préprocesseur
        lines = code_protected.split('\n')
        protected_lines = []
        code_lines = []

        for i, line in enumerate(lines):
            stripped = line.strip()
            if stripped.startswith('#'):
                protected_lines.append((i, line))
                code_lines.append(f"___PROTECTED_LINE_{i}___")
            else:
                code_lines.append(line)

        # Reconstituer le code sans les lignes protégées
        code_to_obfuscate = '\n'.join(code_lines)

        # Extraire tous les identifiants
        identifiers = self.extract_identifiers(code_to_obfuscate)

        # Créer les mappings uniquement pour les identifiants non protégés
        for identifier in identifiers:
            if not self.should_preserve(identifier):
                self.get_obfuscated_name(identifier)

        # Remplacer les identifiants par ordre de longueur décroissante
        # (pour éviter les remplacements partiels)
        sorted_identifiers = sorted(self.mappings.keys(), key=len, reverse=True)

        result = code_to_obfuscate
        for original in sorted_identifiers:
            obfuscated = self.mappings[original]
            # Utiliser des limites de mots pour éviter les remplacements partiels
            pattern = r'\b' + re.escape(original) + r'\b'
            result = re.sub(pattern, obfuscated, result)

        # Restaurer les lignes protégées
        result_lines = result.split('\n')
        for i, original_line in protected_lines:
            result_lines[i] = original_line

        result = '\n'.join(result_lines)

        # Restaurer les chaînes et caractères
        result = self.restore_strings_and_chars(result, strings)

        return result

    def save_mapping(self, filename):
        """Sauvegarde le mapping dans un fichier"""
        with open(filename, 'w', encoding='utf-8') as f:
            f.write("# Mapping d'obfuscation\n")
            f.write("# Original -> Obfusqué\n\n")
            for original, obfuscated in sorted(self.mappings.items()):
                f.write(f"{original} -> {obfuscated}\n")


def main():
    if len(sys.argv) < 2:
        print("Usage: python obfuscator.py <input_file> [output_file]")
        print("Si output_file n'est pas spécifié, utilise input_file.obfuscated.cpp")
        sys.exit(1)

    input_file = sys.argv[1]

    # Déterminer le fichier de sortie
    if len(sys.argv) >= 3:
        output_file = sys.argv[2]
    else:
        path = Path(input_file)
        output_file = path.stem + ".obfuscated" + path.suffix

    mapping_file = Path(output_file).stem + "_mapping.txt"

    # Lire le fichier d'entrée
    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            code = f.read()
    except Exception as e:
        print(f"Erreur lors de la lecture de {input_file}: {e}")
        sys.exit(1)

    # Obfusquer
    obfuscator = CppObfuscator()
    obfuscated_code = obfuscator.obfuscate(code)

    # Écrire le fichier de sortie
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(obfuscated_code)
        print(f" Code obfusqué écrit dans: {output_file}")
    except Exception as e:
        print(f"Erreur lors de l'écriture de {output_file}: {e}")
        sys.exit(1)

    # Sauvegarder le mapping
    try:
        obfuscator.save_mapping(mapping_file)
        print(f" Mapping sauvegardé dans: {mapping_file}")
    except Exception as e:
        print(f"Erreur lors de l'écriture du mapping: {e}")

    print(f"\n{len(obfuscator.mappings)} identifiants ont été obfusqués")


if __name__ == "__main__":
    main()