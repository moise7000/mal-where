# else_if_generator.py

class ElseIfGenerator:
    def __init__(self, cpp_file: str, strings_file: str, output_file: str, placeholder: str = 'else if (str == "cb446ca2e30ab76e")'):
        self.cpp_file = cpp_file
        self.strings_file = strings_file
        self.output_file = output_file
        self.placeholder = placeholder
        self.strings = []

    # --- Lecture des fichiers ---
    def load_strings(self):
        with open(self.strings_file, 'r', encoding='utf-8') as f:
            self.strings = [line.strip() for line in f if line.strip()]
        print(f"{len(self.strings)} chaînes chargées depuis '{self.strings_file}'.")

    def load_cpp_lines(self):
        with open(self.cpp_file, 'r', encoding='utf-8') as f:
            return f.readlines()

    # --- Génération des blocs else-if ---
    def generate_else_if_block(self):
        lines = [f'    // Generated else if branches\n']
        for s in self.strings:
            lines.append(f'    else if (str == "{s}") {{\n')
            lines.append(f'        std::cout << str << std::endl;\n')
            lines.append(f'    }}\n')
        return lines

    # --- Génération du nouveau code C++ ---
    def generate_cpp(self):
        cpp_lines = self.load_cpp_lines()
        new_cpp_lines = []
        skip_block = False

        for line in cpp_lines:
            if self.placeholder in line:
                skip_block = True
                new_cpp_lines.extend(self.generate_else_if_block())
                continue
            if skip_block:
                if '}' in line:
                    skip_block = False
                continue
            new_cpp_lines.append(line)

        with open(self.output_file, 'w', encoding='utf-8') as f:
            f.writelines(new_cpp_lines)

        print(f"Fichier C++ généré avec {len(self.strings)} nouvelles branches else-if : {self.output_file}")
