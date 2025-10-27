#!/usr/bin/env python3
"""
Obfuscateur C++ Multi-Fichiers - Obfusque plusieurs fichiers en préservant les liens
Usage: python multi_obfuscator.py file1.cpp file2.h file3.cpp ...
       python multi_obfuscator.py *.cpp *.h
"""

import re
import sys
import random
import string
from pathlib import Path


class CppMultiObfuscator:
    def __init__(self):
        self.mappings = {}
        self.counter = 0
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
            'std', 'string', 'vector', 'map', 'set', 'list', 'queue', 'stack',
            'pair', 'size_t', 'uint8_t', 'int8_t', 'uint16_t', 'int16_t',
            'uint32_t', 'int32_t', 'uint64_t', 'int64_t',
            'main', 'printf', 'scanf', 'cout', 'cin', 'endl', 'cerr',
            'size', 'length', 'push_back', 'pop_back', 'begin', 'end',
            'iterator', 'empty', 'clear', 'reserve', 'resize',
            'insert', 'erase', 'find', 'at', 'front', 'back',
            'make_pair', 'make_unique', 'make_shared',
            'runtime_error', 'logic_error', 'invalid_argument', 'out_of_range',
            'what', 'exception', 'nullptr_t'
        }

    def generate_name(self):
        """Génère un nom obfusqué unique"""
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
        all_identifiers = re.findall(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', code)

        for identifier in all_identifiers:
            if identifier not in self.keywords:
                identifiers.add(identifier)

        return identifiers

    def should_preserve(self, identifier):
        """Détermine si un identifiant doit être préservé"""
        if identifier in self.keywords:
            return True
        if identifier.startswith('std::'):
            return True
        if identifier.isupper() and len(identifier) > 1:
            return True
        if identifier in ['size_t', 'int8_t', 'int16_t', 'int32_t', 'int64_t',
                          'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t']:
            return True
        return False

    def remove_comments(self, code):
        """Supprime tous les commentaires C++"""
        strings = []
        counter = 0

        def replace_string(match):
            nonlocal counter
            strings.append((counter, match.group(0)))
            placeholder = f"___TEMP_STRING_{counter}___"
            counter += 1
            return placeholder

        result = re.sub(r'"(?:[^"\\]|\\.)*"', replace_string, code)
        result = re.sub(r'//.*?$', '', result, flags=re.MULTILINE)
        result = re.sub(r'/\*.*?\*/', '', result, flags=re.DOTALL)

        for i, original in strings:
            result = result.replace(f"___TEMP_STRING_{i}___", original)

        return result

    def protect_strings_and_chars(self, code):
        """Remplace temporairement les chaînes et caractères par des placeholders"""
        strings = []
        counter = 0

        def replace_string(match):
            nonlocal counter
            strings.append((counter, match.group(0)))
            placeholder = f"___STRING_{counter}___"
            counter += 1
            return placeholder

        def replace_char(match):
            nonlocal counter
            strings.append((counter, match.group(0)))
            placeholder = f"___CHAR_{counter}___"
            counter += 1
            return placeholder

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

    def collect_all_identifiers(self, files_content):
        """Collecte tous les identifiants de tous les fichiers"""
        print(" Analyse de tous les fichiers...")
        all_identifiers = set()

        for filename, code in files_content.items():
            code_no_comments = self.remove_comments(code)
            code_protected, _ = self.protect_strings_and_chars(code_no_comments)
            identifiers = self.extract_identifiers(code_protected)
            all_identifiers.update(identifiers)
            print(f"  - {filename}: {len(identifiers)} identifiants trouvés")

        # Créer les mappings pour tous les identifiants non protégés
        for identifier in all_identifiers:
            if not self.should_preserve(identifier):
                self.get_obfuscated_name(identifier)

        print(f" Total: {len(self.mappings)} identifiants à obfusquer\n")

    def obfuscate_single_file(self, code):
        """Obfusque un seul fichier en utilisant les mappings existants"""
        code = self.remove_comments(code)
        code_protected, strings = self.protect_strings_and_chars(code)

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

        code_to_obfuscate = '\n'.join(code_lines)
        sorted_identifiers = sorted(self.mappings.keys(), key=len, reverse=True)

        result = code_to_obfuscate
        for original in sorted_identifiers:
            obfuscated = self.mappings[original]
            pattern = r'\b' + re.escape(original) + r'\b'
            result = re.sub(pattern, obfuscated, result)

        result_lines = result.split('\n')
        for i, original_line in protected_lines:
            result_lines[i] = original_line

        result = '\n'.join(result_lines)
        result = self.restore_strings_and_chars(result, strings)

        return result

    def obfuscate_files(self, input_files):
        """Obfusque plusieurs fichiers en préservant les liens"""
        # Lire tous les fichiers
        files_content = {}
        print(" Lecture des fichiers...")
        for input_file in input_files:
            try:
                with open(input_file, 'r', encoding='utf-8') as f:
                    files_content[input_file] = f.read()
                print(f" {input_file}")
            except Exception as e:
                print(f" Erreur lecture {input_file}: {e}")
                continue

        if not files_content:
            print("Aucun fichier n'a pu être lu")
            return False

        # Collecter tous les identifiants de tous les fichiers
        self.collect_all_identifiers(files_content)

        # Obfusquer chaque fichier
        print("Obfuscation des fichiers...")
        output_files = []
        for input_file, code in files_content.items():
            path = Path(input_file)
            output_file = path.stem + ".obfuscated" + path.suffix

            obfuscated_code = self.obfuscate_single_file(code)

            try:
                with open(output_file, 'w', encoding='utf-8') as f:
                    f.write(obfuscated_code)
                print(f"  ✓ {output_file}")
                output_files.append(output_file)
            except Exception as e:
                print(f"  ✗ Erreur écriture {output_file}: {e}")

        # Sauvegarder le mapping global
        mapping_file = "global_mapping.txt"
        self.save_mapping(mapping_file)
        print(f"\n Mapping global sauvegardé: {mapping_file}")
        print(f" {len(output_files)} fichiers obfusqués avec succès")
        print(f" {len(self.mappings)} identifiants obfusqués au total")

        return True

    def save_mapping(self, filename):
        """Sauvegarde le mapping dans un fichier"""
        with open(filename, 'w', encoding='utf-8') as f:
            f.write("# Mapping d'obfuscation global\n")
            f.write("# Original -> Obfusqué\n\n")
            for original, obfuscated in sorted(self.mappings.items()):
                f.write(f"{original} -> {obfuscated}\n")


def main():
    if len(sys.argv) < 2:
        print("Usage: python multi_obfuscator.py <file1> <file2> ... <fileN>")
        print("       python multi_obfuscator.py *.cpp *.h")
        print("\nObfusque plusieurs fichiers C++ en préservant les liens entre eux")
        sys.exit(1)

    input_files = sys.argv[1:]

    print("=" * 60)
    print("  Obfuscateur C++ Multi-Fichiers")
    print("=" * 60)
    print()

    obfuscator = CppMultiObfuscator()
    success = obfuscator.obfuscate_files(input_files)

    if not success:
        sys.exit(1)

    print("\n" + "=" * 60)
    print("  Obfuscation terminée avec succès!")
    print("=" * 60)


if __name__ == "__main__":
    main()