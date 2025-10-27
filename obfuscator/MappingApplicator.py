#!/usr/bin/env python3
"""
Applicateur de Mapping - Applique un mapping existant à de nouveaux fichiers
Usage: python apply_mapping.py <mapping_file> <file1> <file2> ... <fileN>
"""

import re
import sys
from pathlib import Path


class MappingApplicator:
    def __init__(self):
        self.mappings = {}
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

    def load_mapping(self, mapping_file):
        """Charge un mapping depuis un fichier"""
        print(f"📖 Chargement du mapping: {mapping_file}")
        try:
            with open(mapping_file, 'r', encoding='utf-8') as f:
                for line in f:
                    line = line.strip()
                    # Ignorer les commentaires et lignes vides
                    if not line or line.startswith('#'):
                        continue

                    # Parser la ligne: "original -> obfuscated"
                    if ' -> ' in line:
                        parts = line.split(' -> ')
                        if len(parts) == 2:
                            original = parts[0].strip()
                            obfuscated = parts[1].strip()
                            self.mappings[original] = obfuscated

            print(f"✓ {len(self.mappings)} mappings chargés\n")
            return True
        except Exception as e:
            print(f"❌ Erreur lors du chargement du mapping: {e}")
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

    def apply_mapping_to_code(self, code):
        """Applique le mapping au code"""
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

        # Trier par longueur décroissante pour éviter les remplacements partiels
        sorted_identifiers = sorted(self.mappings.keys(), key=len, reverse=True)

        result = code_to_obfuscate
        replacements_made = 0

        for original in sorted_identifiers:
            obfuscated = self.mappings[original]
            pattern = r'\b' + re.escape(original) + r'\b'
            new_result = re.sub(pattern, obfuscated, result)
            if new_result != result:
                replacements_made += 1
            result = new_result

        result_lines = result.split('\n')
        for i, original_line in protected_lines:
            result_lines[i] = original_line

        result = '\n'.join(result_lines)
        result = self.restore_strings_and_chars(result, strings)

        return result, replacements_made

    def apply_to_files(self, input_files):
        """Applique le mapping à plusieurs fichiers"""
        print(" Application du mapping aux fichiers...")
        output_files = []

        for input_file in input_files:
            try:
                # Lire le fichier
                with open(input_file, 'r', encoding='utf-8') as f:
                    code = f.read()

                # Appliquer le mapping
                obfuscated_code, replacements = self.apply_mapping_to_code(code)

                # Générer le nom de fichier de sortie
                path = Path(input_file)
                output_file = path.stem + ".mapped" + path.suffix

                # Écrire le fichier obfusqué
                with open(output_file, 'w', encoding='utf-8') as f:
                    f.write(obfuscated_code)

                print(f"   {input_file} -> {output_file} ({replacements} remplacements)")
                output_files.append(output_file)

            except Exception as e:
                print(f"   Erreur avec {input_file}: {e}")

        return output_files

    def show_mapping_stats(self):
        """Affiche des statistiques sur le mapping"""
        if not self.mappings:
            return

        print("\n Statistiques du mapping:")
        print(f"  - Nombre total de mappings: {len(self.mappings)}")

        # Quelques exemples
        examples = list(self.mappings.items())[:5]
        if examples:
            print("   Exemples de mappings:")
            for orig, obf in examples:
                print(f"    - {orig} -> {obf}")


def main():
    if len(sys.argv) < 3:
        print("Usage: python apply_mapping.py <mapping_file> <file1> [file2] ... [fileN]")
        print("\nApplique un mapping d'obfuscation existant à de nouveaux fichiers")
        print("\nExemple:")
        print("  python apply_mapping.py global_mapping.txt newfile.cpp newfile.h")
        sys.exit(1)

    mapping_file = sys.argv[1]
    input_files = sys.argv[2:]

    print("=" * 60)
    print("  Applicateur de Mapping C++")
    print("=" * 60)
    print()

    applicator = MappingApplicator()

    # Charger le mapping
    if not applicator.load_mapping(mapping_file):
        sys.exit(1)

    applicator.show_mapping_stats()
    print()

    # Appliquer aux fichiers
    output_files = applicator.apply_to_files(input_files)

    if output_files:
        print(f"\n {len(output_files)} fichiers traités avec succès")
        print("\n" + "=" * 60)
        print("  Terminé!")
        print("=" * 60)
    else:
        print("\n Aucun fichier n'a pu être traité")
        sys.exit(1)


if __name__ == "__main__":
    main()