# string_generator.py

import os
import random

class StringGenerator:
    @staticmethod
    def generate_octet_strings(x: int):
        """Renvoie une liste de x chaînes hexadécimales de 8 octets"""
        return [os.urandom(8).hex() for _ in range(x)]

    @staticmethod
    def generate_french_like_strings(x: int):
        lettres = "abcdefghijklmnopqrstuvwxyzàâçéèêëîïôùûüÿ"
        symbols = "aeiouyàâéèêëîïôùûüÿ-_@#&('§!)^$*¨`£ù%+= "
        consonnes = "".join([c for c in lettres if c not in symbols])

        def mot_francais_like():
            mot = ""
            for i in range(8):
                mot += random.choice(consonnes) if i % 2 == 0 else random.choice(symbols)
            return mot

        return [mot_francais_like() for _ in range(x)]

    @staticmethod
    def create_file(func, x, out_file):
        lines = func(x)
        with open(out_file, "a", encoding="utf-8") as f:
            for line in lines:
                f.write(line + "\n")
        print(f"{x} chaînes ajoutées dans '{out_file}'.")
