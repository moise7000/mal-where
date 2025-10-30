#!/usr/bin/env python3
"""
Générateur de variantes de fichiers ASM avec offuscation
Crée plusieurs fichiers assembleur similaires à partir d'un fichier source
avec offuscation des noms de fonctions, labels et boucles
"""

import argparse
import re
import random
import os
import string
from pathlib import Path


class ASMVariantGenerator:
    def __init__(self, input_file):
        self.input_file = input_file
        with open(input_file, 'r', encoding='utf-8') as f:
            self.content = f.read()
        self.lines = self.content.split('\n')
        self.label_mapping = {}

    def _generate_obfuscated_name(self, prefix='L'):
        """Génère un nom offusqué pour un label/fonction"""
        chars = string.ascii_uppercase + string.digits + '_'
        length = random.randint(8, 12)
        return prefix + ''.join(random.choice(chars) for _ in range(length))

    def _extract_labels(self):
        """Extrait tous les labels et noms de fonctions du code"""
        labels = set()
        # Pattern pour les labels (nom suivi de ':')
        label_pattern = r'^([a-zA-Z_][a-zA-Z0-9_]*):.*$'
        # Pattern pour les déclarations de fonction
        func_patterns = [
            r'^\s*([a-zA-Z_][a-zA-Z0-9_]*)\s+proc\b',  # MASM/TASM
            r'^\s*\.globl\s+([a-zA-Z_][a-zA-Z0-9_]*)',  # GAS
            r'^\s*global\s+([a-zA-Z_][a-zA-Z0-9_]*)',  # NASM
            r'^\s*extern\s+([a-zA-Z_][a-zA-Z0-9_]*)',  # NASM extern
        ]

        for line in self.lines:
            # Cherche les labels
            match = re.match(label_pattern, line.strip())
            if match:
                label = match.group(1)
                # Ignore les directives et mots-clés communs
                if label.lower() not in ['section', 'segment', 'data', 'text', 'bss']:
                    labels.add(label)

            # Cherche les déclarations de fonction
            for pattern in func_patterns:
                match = re.search(pattern, line, re.IGNORECASE)
                if match:
                    labels.add(match.group(1))

        return labels

    def _create_label_mapping(self, variant_num):
        """Crée une table de correspondance pour l'offuscation des labels"""
        labels = self._extract_labels()
        self.label_mapping = {}

        for label in labels:
            # Ignore certains labels spéciaux
            if label.lower() in ['main', 'start', '_start']:
                prefix = 'ENTRY_'
            elif 'loop' in label.lower():
                prefix = 'LOOP_'
            elif 'func' in label.lower() or 'fn' in label.lower():
                prefix = 'FN_'
            else:
                prefix = 'LAB_'

            self.label_mapping[label] = self._generate_obfuscated_name(prefix)

    def _obfuscate_labels(self, line):
        """Remplace les labels par leurs versions offusquées"""
        modified_line = line

        # Remplace dans les directives global/extern
        for original, obfuscated in self.label_mapping.items():
            # Directive global/extern
            global_pattern = r'(global|extern|\.globl)\s+' + re.escape(original) + r'\b'
            modified_line = re.sub(global_pattern, r'\1 ' + obfuscated, modified_line, flags=re.IGNORECASE)

            # Label avec ':'
            pattern = r'\b' + re.escape(original) + r'(?=:)'
            modified_line = re.sub(pattern, obfuscated, modified_line)

            # Références aux labels (après jmp, call, etc.)
            jump_pattern = r'(jmp|je|jne|jz|jnz|jg|jge|jl|jle|ja|jae|jb|jbe|call)\s+' + re.escape(original) + r'\b'
            modified_line = re.sub(jump_pattern, r'\1 ' + obfuscated, modified_line, flags=re.IGNORECASE)

        return modified_line

    def _remove_comments(self, line):
        """Supprime les commentaires d'une ligne"""
        comment_pos = line.find(';')
        if comment_pos != -1:
            line = line[:comment_pos].rstrip()
        return line

    def generate_variant(self, variant_num):
        """Génère une variante du fichier ASM avec offuscation"""
        # Crée une nouvelle table d'offuscation pour cette variante
        self._create_label_mapping(variant_num)

        new_lines = []

        for line in self.lines:
            # Supprime les commentaires
            line = self._remove_comments(line)

            # Ignore les lignes vides après suppression des commentaires
            if not line.strip():
                continue

            modified_line = line

            # Offuscation des labels
            modified_line = self._obfuscate_labels(modified_line)

            # Variante 1: Modifier les registres (avec probabilité)
            if random.random() < 0.3:
                modified_line = self._swap_registers(modified_line)

            # Variante 2: Modifier les valeurs immédiates
            if random.random() < 0.2:
                modified_line = self._modify_immediates(modified_line)

            new_lines.append(modified_line)

        return '\n'.join(new_lines)

    def _swap_registers(self, line):
        """Échange certains registres par des équivalents"""
        reg_swaps = {
            r'\beax\b': ['ebx', 'ecx', 'edx'],
            r'\bebx\b': ['eax', 'ecx', 'edx'],
            r'\becx\b': ['eax', 'ebx', 'edx'],
            r'\bedx\b': ['eax', 'ebx', 'ecx'],
            r'\brax\b': ['rbx', 'rcx', 'rdx'],
            r'\brbx\b': ['rax', 'rcx', 'rdx'],
            r'\brcx\b': ['rax', 'rbx', 'rdx'],
            r'\brdx\b': ['rax', 'rbx', 'rcx'],
        }

        for pattern, alternatives in reg_swaps.items():
            if re.search(pattern, line, re.IGNORECASE):
                if random.random() < 0.5:
                    replacement = random.choice(alternatives)
                    line = re.sub(pattern, replacement, line, count=1, flags=re.IGNORECASE)
                break

        return line

    def _modify_immediates(self, line):
        """Modifie légèrement les valeurs immédiates"""
        hex_pattern = r'\b0x[0-9a-fA-F]+\b'
        dec_pattern = r'\b\d+\b'

        def modify_hex(match):
            value = int(match.group(), 16)
            delta = random.randint(-10, 10)
            new_value = max(0, value + delta)
            return f"0x{new_value:x}"

        def modify_dec(match):
            value = int(match.group())
            if value > 1000:
                return match.group()
            delta = random.randint(-5, 5)
            return str(max(0, value + delta))

        if random.random() < 0.5:
            line = re.sub(hex_pattern, modify_hex, line, count=1)
        else:
            line = re.sub(dec_pattern, modify_dec, line, count=1)

        return line

    def _generate_random_name(self, length=8):
        """Génère une chaîne aléatoire pour le nom de fichier"""
        chars = string.ascii_lowercase + string.digits
        return ''.join(random.choice(chars) for _ in range(length))

    def generate_variants(self, num_variants, output_dir=None, name_length=8, save_mapping=False):
        """Génère plusieurs variantes et les sauvegarde"""
        if output_dir is None:
            output_dir = Path(self.input_file).parent
        else:
            output_dir = Path(output_dir)
            output_dir.mkdir(exist_ok=True)

        ext = Path(self.input_file).suffix
        generated_files = []
        used_names = set()

        for i in range(1, num_variants + 1):
            variant_content = self.generate_variant(i)

            # Génère un nom unique
            while True:
                random_name = self._generate_random_name(name_length)
                if random_name not in used_names:
                    used_names.add(random_name)
                    break

            output_file = output_dir / f"{random_name}{ext}"

            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(variant_content)

            # Sauvegarde optionnelle de la table de correspondance
            if save_mapping:
                mapping_file = output_dir / f"{random_name}_mapping.txt"
                with open(mapping_file, 'w', encoding='utf-8') as f:
                    f.write(f"Mapping pour {random_name}{ext}:\n")
                    f.write("=" * 50 + "\n")
                    for original, obfuscated in sorted(self.label_mapping.items()):
                        f.write(f"{original:30} -> {obfuscated}\n")

            generated_files.append(output_file)
            print(f"✓ Généré: {output_file}")

        return generated_files


def main():
    parser = argparse.ArgumentParser(
        description="Génère des variantes d'un fichier assembleur (ASM) avec offuscation"
    )
    parser.add_argument(
        'input_file',
        help="Fichier ASM source"
    )
    parser.add_argument(
        '-n', '--num-variants',
        type=int,
        default=5,
        help="Nombre de variantes à générer (défaut: 5)"
    )
    parser.add_argument(
        '-o', '--output-dir',
        help="Répertoire de sortie (défaut: même que le fichier source)"
    )
    parser.add_argument(
        '-l', '--name-length',
        type=int,
        default=8,
        help="Longueur du nom aléatoire (défaut: 8)"
    )
    parser.add_argument(
        '-m', '--save-mapping',
        action='store_true',
        help="Sauvegarde la table de correspondance des labels offusqués"
    )

    args = parser.parse_args()

    # Vérifie que le fichier existe
    if not os.path.exists(args.input_file):
        print(f"❌ Erreur: Le fichier '{args.input_file}' n'existe pas")
        return 1

    print(f"📄 Fichier source: {args.input_file}")
    print(f"🔢 Nombre de variantes: {args.num_variants}")
    print(f"🔒 Offuscation des labels: activée")
    print(f"🗑️  Suppression des commentaires: activée\n")

    generator = ASMVariantGenerator(args.input_file)
    files = generator.generate_variants(
        args.num_variants,
        args.output_dir,
        args.name_length,
        args.save_mapping
    )

    print(f"\n✅ {len(files)} variantes générées avec succès!")
    if args.save_mapping:
        print(f"📋 Tables de correspondance sauvegardées")

    return 0


if __name__ == "__main__":
    exit(main())