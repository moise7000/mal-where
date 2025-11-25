import pefile

pe = pefile.PE("main.exe")

for section in pe.sections:
    name = section.Name.decode().rstrip('\x00')
    raw_offset = section.PointerToRawData
    raw_size   = section.SizeOfRawData

    print(f"{name}: offset={hex(raw_offset)} size={hex(raw_size)}")

    with open(name + ".bin", "wb") as f:
        f.write(section.get_data())
