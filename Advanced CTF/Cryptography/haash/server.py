import random

FLAG = "Cybersphere{????????????????????}"

WORDS = [
    "just random files u dont need to know them"    
]

# ── CRC32C ────────────────────────────────────────────────────────────────────
def _build_table():
    table = []
    for i in range(256):
        crc = i
        for _ in range(8):
            crc = (crc >> 1) ^ (0x82F63B78 if crc & 1 else 0)
        table.append(crc & 0xFFFFFFFF)
    return table

_TABLE = _build_table()

def crc32c(data: bytes) -> int:
    crc = 0xFFFFFFFF
    for b in data:
        crc = (crc >> 8) ^ _TABLE[(crc ^ b) & 0xFF]
    return (crc ^ 0xFFFFFFFF) & 0xFFFFFFFF


def main():
    word   = random.choice(WORDS)
    target = crc32c(word.encode())

    print("""
+===========================================+
|         Crack the files       |
+===========================================+


  Polynomial : 0x82F63B78  (Castagnoli)
  Hash size  : 32 bits

  Input format : hex-encoded bytes
""")
    print(f"  A Btrfs filename was hashed to : {target:#010x}")
    print()
    print("  Find two DIFFERENT hex strings A and B such that:")
    print()
    print(f"      CRC32C(bytes.fromhex(A)) == CRC32C(bytes.fromhex(B)) == {target:#010x}")
    print()
    print("-------------------------------------------")
    print()

    MAX_ATTEMPTS = 3
    for attempt in range(1, MAX_ATTEMPTS + 1):
        print(f"[Attempt {attempt}/{MAX_ATTEMPTS}]")

        a_hex = input("  Enter hex string A : ").strip()
        b_hex = input("  Enter hex string B : ").strip()

        try:
            a = bytes.fromhex(a_hex)
            b = bytes.fromhex(b_hex)
        except ValueError:
            print("[-] Invalid hex input. Try again.\n")
            continue

        if not a or not b:
            print("[-] Empty input. Try again.\n")
            continue

        if len(a) > 256 or len(b) > 256:
            print("[-] Max 256 bytes.\n")
            continue

        if a == b:
            print("[-] Inputs must be different!\n")
            continue

        ha = crc32c(a)
        hb = crc32c(b)

        print(f"\n  CRC32C({a_hex!r}) = {ha:#010x}")
        print(f"  CRC32C({b_hex!r}) = {hb:#010x}")
        print(f"  Target           = {target:#010x}\n")

        if ha == hb == target:
            print("+===========================================+")
            print("[+] COLLISION CONFIRMED!")
            print(f"    Both hash to {target:#010x}\n")
            print(f"    FLAG: {FLAG}")
            print("+===========================================+")
            return

        if ha == hb:
            print("[-] Good collision but wrong target. Keep going.\n")
        else:
            print("[-] Hashes differ and don't match target.\n")

    print("\n[-] No more attempts. Reconnect for a new target.")


if __name__ == "__main__":
    main()