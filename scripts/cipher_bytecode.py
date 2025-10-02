# crypto_utils.py

import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.primitives import hashes

class PasswordCrypto:
    def __init__(self, password: str, algorithm: str = 'AES', key_len: int = 32):
        self.password = password
        self.algorithm = algorithm.upper()
        self.key_len = key_len
        self.backend = default_backend()

    def derive_key(self, salt: bytes) -> bytes:
        """Derive a fixed-length key from the password and salt using PBKDF2."""
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=self.key_len,
            salt=salt,
            iterations=100_000,
            backend=self.backend
        )
        return kdf.derive(self.password.encode())

    def encrypt(self, data) -> list[str]:
        """Encrypt data (bytes or list of '0xNN') and return list of '0xNN'."""
        # Convert input to bytes
        if isinstance(data, str):
            with open(data, 'rb') as f:
                plaintext = f.read()
        else:
            plaintext = bytes(int(b, 16) if isinstance(b, str) else b for b in data)

        salt = os.urandom(16)
        key = self.derive_key(salt)

        if self.algorithm == 'AES':
            iv = os.urandom(12)
            cipher = Cipher(algorithms.AES(key), modes.GCM(iv), backend=self.backend)
            encryptor = cipher.encryptor()
            ciphertext = encryptor.update(plaintext) + encryptor.finalize()
            # Structure: salt(16) + iv(12) + tag(16) + ciphertext
            encrypted_bytes = salt + iv + encryptor.tag + ciphertext
        elif self.algorithm == 'CHACHA20':
            nonce = os.urandom(16)
            cipher = Cipher(algorithms.ChaCha20(key, nonce), mode=None, backend=self.backend)
            encryptor = cipher.encryptor()
            ciphertext = encryptor.update(plaintext)
            # Structure: salt(16) + nonce(16) + ciphertext
            encrypted_bytes = salt + nonce + ciphertext
        else:
            raise ValueError("Algorithm must be 'AES' or 'ChaCha20'")

        return [f"0x{b:02X}" for b in encrypted_bytes]

    def decrypt(self, encrypted_hex: list[str]) -> bytes:
        """Decrypt a list of '0xNN' and return the plaintext bytes."""
        encrypted_bytes = bytes(int(b, 16) for b in encrypted_hex)
        salt = encrypted_bytes[:16]
        key = self.derive_key(salt)

        if self.algorithm == 'AES':
            iv = encrypted_bytes[16:28]
            tag = encrypted_bytes[28:44]
            ciphertext = encrypted_bytes[44:]
            cipher = Cipher(algorithms.AES(key), modes.GCM(iv, tag), backend=self.backend)
            decryptor = cipher.decryptor()
            plaintext = decryptor.update(ciphertext) + decryptor.finalize()
        elif self.algorithm == 'CHACHA20':
            nonce = encrypted_bytes[16:32]
            ciphertext = encrypted_bytes[32:]
            cipher = Cipher(algorithms.ChaCha20(key, nonce), mode=None, backend=self.backend)
            decryptor = cipher.decryptor()
            plaintext = decryptor.update(ciphertext)
        else:
            raise ValueError("Algorithm must be 'AES' or 'ChaCha20'")

        return plaintext
