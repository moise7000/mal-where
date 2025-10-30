#!/usr/bin/env python3
"""
Offuscateur de code C++ avec plusieurs méthodes d'offuscation
Supporte la sauvegarde et le chargement de mappings pour offusquer plusieurs fichiers de manière cohérente
"""

import re
import random
import string
import argparse
import json
from pathlib import Path


class CppObfuscator:
    def __init__(self, mapping_file=None):
        self.var_mapping = {}
        self.func_mapping = {}
        self.string_mapping = {}
        self.mapping_file = mapping_file
        self.preserved_keywords = {
            'int', 'float', 'double', 'char', 'void', 'bool', 'long', 'short',
            'unsigned', 'signed', 'const', 'static', 'return', 'if', 'else',
            'for', 'while', 'do', 'switch', 'case', 'break', 'continue',
            'class', 'struct', 'public', 'private', 'protected', 'namespace',
            'using', 'include', 'define', 'ifdef', 'endif', 'ifndef',
            'true', 'false', 'nullptr', 'new', 'delete', 'this', 'virtual',
            'override', 'template', 'typename', 'auto', 'std', 'cout', 'cin',
            'endl', 'string', 'vector', 'map', 'set', 'main', 'size', 'length',
            'push_back', 'pop_back', 'clear', 'empty', 'begin', 'end'
        }

        # Charge le mapping existant si fourni
        if mapping_file and Path(mapping_file).exists():
            self.load_mapping(mapping_file)

    def generate_random_name(self, prefix='', length=8):
        """Génère un nom aléatoire pour les variables/fonctions"""
        chars = string.ascii_letters + string.digits
        name = prefix + ''.join(random.choices(chars, k=length))
        # S'assure que le nom commence par une lettre
        if name[0].isdigit():
            name = random.choice(string.ascii_letters) + name[1:]
        return name

    def save_mapping(self, filepath):
        """Sauvegarde le mapping dans un fichier JSON"""
        mapping_data = {
            'variables': self.var_mapping,
            'functions': self.func_mapping,
            'strings': self.string_mapping
        }

        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(mapping_data, f, indent=2, ensure_ascii=False)

        print(f"[+] Mapping sauvegardé dans: {filepath}")
        print(f"    - {len(self.var_mapping)} variables")
        print(f"    - {len(self.func_mapping)} fonctions")
        print(f"    - {len(self.string_mapping)} chaînes")

    def load_mapping(self, filepath):
        """Charge un mapping depuis un fichier JSON"""
        with open(filepath, 'r', encoding='utf-8') as f:
            mapping_data = json.load(f)

        self.var_mapping = mapping_data.get('variables', {})
        self.func_mapping = mapping_data.get('functions', {})
        self.string_mapping = mapping_data.get('strings', {})

        print(f"[+] Mapping chargé depuis: {filepath}")
        print(f"    - {len(self.var_mapping)} variables")
        print(f"    - {len(self.func_mapping)} fonctions")
        print(f"    - {len(self.string_mapping)} chaînes")

    def rename_variables(self, code):
        """Renomme les variables avec des noms aléatoires"""
        # Pattern pour trouver les déclarations de variables
        var_pattern = r'\b(int|float|double|char|bool|long|short|auto|string|unsigned|signed)\s+([a-zA-Z_][a-zA-Z0-9_]*)\b'

        def replace_var(match):
            var_type = match.group(1)
            var_name = match.group(2)

            if var_name not in self.preserved_keywords and var_name not in self.var_mapping:
                # Génère un nouveau nom unique
                new_name = self.generate_random_name('v_')
                while new_name in self.var_mapping.values():
                    new_name = self.generate_random_name('v_')
                self.var_mapping[var_name] = new_name

            if var_name in self.var_mapping:
                return f"{var_type} {self.var_mapping[var_name]}"
            return match.group(0)

        code = re.sub(var_pattern, replace_var, code)

        # Remplace toutes les occurrences des variables
        for old_name, new_name in self.var_mapping.items():
            code = re.sub(r'\b' + old_name + r'\b', new_name, code)

        return code

    def rename_functions(self, code):
        """Renomme les fonctions (sauf main)"""
        # Pattern pour les déclarations de fonctions
        func_pattern = r'\b(int|float|double|char|bool|void|auto|long|short|unsigned|signed)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\('

        def replace_func(match):
            return_type = match.group(1)
            func_name = match.group(2)

            if func_name not in self.preserved_keywords and func_name not in self.func_mapping:
                # Génère un nouveau nom unique
                new_name = self.generate_random_name('f_')
                while new_name in self.func_mapping.values():
                    new_name = self.generate_random_name('f_')
                self.func_mapping[func_name] = new_name

            if func_name in self.func_mapping:
                return f"{return_type} {self.func_mapping[func_name]}("
            return match.group(0)

        code = re.sub(func_pattern, replace_func, code)

        # Remplace les appels de fonctions
        for old_name, new_name in self.func_mapping.items():
            code = re.sub(r'\b' + old_name + r'\b', new_name, code)

        return code

    def add_junk_code(self, code):
        """Ajoute du code inutile mais valide"""
        junk_snippets = [
            "int __junk_{} = 0;",
            "float __dummy_{} = 1.0f;",
            "bool __unused_{} = false;",
            "char __tmp_{}[10];",
        ]

        lines = code.split('\n')
        result = []

        for line in lines:
            result.append(line)
            # Ajoute du junk code aléatoirement (10% de chance)
            if random.random() < 0.1 and line.strip() and not line.strip().startswith('//'):
                junk = random.choice(junk_snippets).format(random.randint(1000, 9999))
                indent = len(line) - len(line.lstrip())
                result.append(' ' * indent + junk)

        return '\n'.join(result)

    def insert_fake_conditions(self, code):
        """Insère des conditions toujours vraies/fausses"""
        fake_conditions = [
            "if(1) { ",
            "if(true) { ",
            "while(0) { break; } ",
        ]

        lines = code.split('\n')
        result = []

        for line in lines:
            # Enveloppe certaines lignes dans des conditions factices
            if random.random() < 0.05 and '{' in line and 'if' not in line and 'for' not in line:
                indent = len(line) - len(line.lstrip())
                fake = random.choice(fake_conditions)
                result.append(' ' * indent + fake)
                result.append(line)
                result.append(' ' * indent + '}')
            else:
                result.append(line)

        return '\n'.join(result)

    def add_comments_obfuscation(self, code):
        """Remplace les commentaires par du texte aléatoire"""
        # Supprime les commentaires existants
        code = re.sub(r'//.*$', '', code, flags=re.MULTILINE)
        code = re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)

        # Ajoute des faux commentaires
        fake_comments = [
            "// System initialization",
            "// Memory allocation",
            "// Data processing",
            "// Cache optimization",
            "// Buffer management",
        ]

        lines = code.split('\n')
        result = []

        for line in lines:
            if random.random() < 0.08 and line.strip():
                indent = len(line) - len(line.lstrip())
                result.append(' ' * indent + random.choice(fake_comments))
            result.append(line)

        return '\n'.join(result)

    def obfuscate_strings(self, code):
        """Obfusque les chaînes de caractères"""

        def hex_encode_string(match):
            s = match.group(1)
            if len(s) < 3:  # Garde les petites chaînes intactes
                return match.group(0)

            # Utilise le mapping existant ou crée un nouveau
            if s not in self.string_mapping:
                hex_values = ','.join([f"0x{ord(c):02x}" for c in s])
                self.string_mapping[s] = f'(char[]){{{hex_values},0}}'

            return self.string_mapping[s]

        # Obfusque les chaînes entre guillemets
        code = re.sub(r'"([^"]{3,})"', hex_encode_string, code)

        return code

    def add_arithmetic_obfuscation(self, code):
        """Obfusque les constantes numériques simples"""

        def obfuscate_number(match):
            num = int(match.group(0))
            if abs(num) > 1000:  # Ne touche pas aux grands nombres
                return match.group(0)

            # Remplace par une expression équivalente
            rand = random.randint(1, 100)
            operations = [
                f"({num + rand} - {rand})",
                f"({num * 2} / 2)",
                f"({num - rand} + {rand})",
            ]
            return random.choice(operations)

        # Obfusque les nombres simples (pas dans les #define, includes, etc.)
        lines = code.split('\n')
        result = []

        for line in lines:
            if not line.strip().startswith('#') and not 'include' in line:
                line = re.sub(r'\b(\d+)\b', obfuscate_number, line)
            result.append(line)

        return '\n'.join(result)

    def obfuscate(self, code, methods=None):
        """Applique toutes les méthodes d'offuscation"""
        if methods is None:
            methods = ['variables', 'functions', 'junk', 'conditions', 'comments', 'strings', 'arithmetic']

        print(f"[+] Application des méthodes: {', '.join(methods)}")

        if 'variables' in methods:
            print("  → Renommage des variables...")
            code = self.rename_variables(code)

        if 'functions' in methods:
            print("  → Renommage des fonctions...")
            code = self.rename_functions(code)

        if 'junk' in methods:
            print("  → Ajout de code inutile...")
            code = self.add_junk_code(code)

        if 'conditions' in methods:
            print("  → Insertion de fausses conditions...")
            code = self.insert_fake_conditions(code)

        if 'comments' in methods:
            print("  → Obfuscation des commentaires...")
            code = self.add_comments_obfuscation(code)

        if 'strings' in methods:
            print("  → Obfuscation des chaînes...")
            code = self.obfuscate_strings(code)

        if 'arithmetic' in methods:
            print("  → Obfuscation arithmétique...")
            code = self.add_arithmetic_obfuscation(code)

        return code


def main():
    parser = argparse.ArgumentParser(description='Offuscateur de code C++')
    parser.add_argument('input', nargs='+', help='Fichier(s) C++ source')
    parser.add_argument('-o', '--output-dir', help='Répertoire de sortie (défaut: même répertoire que l\'input)')
    parser.add_argument('-m', '--methods', nargs='+',
                        choices=['variables', 'functions', 'junk', 'conditions', 'comments', 'strings', 'arithmetic',
                                 'all'],
                        default=['all'],
                        help='Méthodes d\'offuscation à appliquer')
    parser.add_argument('--mapping', help='Fichier de mapping JSON (pour charger/sauvegarder)')
    parser.add_argument('--save-mapping', help='Sauvegarder le mapping dans ce fichier')
    parser.add_argument('--load-mapping', help='Charger le mapping depuis ce fichier')

    args = parser.parse_args()

    # Détermine le fichier de mapping à utiliser
    mapping_file = args.load_mapping or args.mapping

    # Crée l'offuscateur avec le mapping si fourni
    obfuscator = CppObfuscator(mapping_file)

    # Détermine les méthodes à appliquer
    methods = args.methods
    if 'all' in methods:
        methods = ['variables', 'functions', 'junk', 'conditions', 'comments', 'strings', 'arithmetic']

    # Détermine le répertoire de sortie
    output_dir = Path(args.output_dir) if args.output_dir else None
    if output_dir:
        output_dir.mkdir(parents=True, exist_ok=True)

    # Traite chaque fichier
    for input_file in args.input:
        input_path = Path(input_file)
        if not input_path.exists():
            print(f"[!] Erreur: Le fichier {input_file} n'existe pas")
            continue

        print(f"\n{'=' * 60}")
        print(f"[+] Offuscation de {input_file}...")
        print(f"{'=' * 60}")

        # Lit le fichier source
        with open(input_path, 'r', encoding='utf-8') as f:
            source_code = f.read()

        # Obfusque le code
        obfuscated_code = obfuscator.obfuscate(source_code, methods)

        # Détermine le nom du fichier de sortie
        if output_dir:
            output_path = output_dir / f"{input_path.stem}_obfuscated{input_path.suffix}"
        else:
            output_path = input_path.parent / f"{input_path.stem}_obfuscated{input_path.suffix}"

        # Écrit le résultat
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(obfuscated_code)

        print(f"[+] Code obfusqué enregistré dans: {output_path}")
        print(f"[+] Taille originale: {len(source_code)} octets")
        print(f"[+] Taille obfusquée: {len(obfuscated_code)} octets")

    # Sauvegarde le mapping si demandé
    save_mapping_file = args.save_mapping or args.mapping
    if save_mapping_file:
        print(f"\n{'=' * 60}")
        obfuscator.save_mapping(save_mapping_file)


if __name__ == "__main__":
    main()