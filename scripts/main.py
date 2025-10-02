# main.py

from string_generator import StringGenerator
from else_if_generator import ElseIfGenerator

# --- Génération des fichiers de chaînes ---
#StringGenerator.create_file(StringGenerator.generate_octet_strings, 3000, "strings/octet.txt")
#StringGenerator.create_file(StringGenerator.generate_french_like_strings, 10, "strings/french.txt")

# --- Génération du code C++ ---
#generator = ElseIfGenerator(
 #   cpp_file="echo.cpp",
 #   strings_file="strings/octet.txt",
 #   output_file="custom_echo_generated.cpp"
#)
#generator.load_strings()
#generator.generate_cpp()


from cipher_bytecode import PasswordCrypto

bytecode = ['0x48', '0x65', '0x6C', '0x6C', '0x6F']  # "Hello"
password = "@HoVeXgtEzMFXB8lG6@qnCb4Gn#H?R#GWxa3V"

crypto = PasswordCrypto(password, algorithm='AES')

# Chiffrement
encrypted_hex = crypto.encrypt(bytecode)
print("Encrypted:", encrypted_hex)

# Déchiffrement
decrypted = crypto.decrypt(encrypted_hex)
print("Decrypted:", decrypted)  # b'Hello'
