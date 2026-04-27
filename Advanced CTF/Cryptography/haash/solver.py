
from pwn import *

HOST = "18.199.81.207"
PORT = 7000


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

# ── Reverse one CRC step ──────────────────────────────────────────────────────
def reverse_step(new_state: int, byte_val: int) -> int:
    for lo in range(256):
        old = (((new_state ^ _TABLE[lo]) << 8) | (lo ^ byte_val)) & 0xFFFFFFFF
        if (old >> 8) ^ _TABLE[(old ^ byte_val) & 0xFF] == new_state:
            return old
    return None

# ── CRC Forgery (Meet in the Middle) ─────────────────────────────────────────
def forge(prefix: bytes, target: int) -> bytes:
    
   
    state = 0xFFFFFFFF
    for b in prefix:
        state = (state >> 8) ^ _TABLE[(state ^ b) & 0xFF]

    desired = target ^ 0xFFFFFFFF

    
    fwd = {}
    for b1 in range(256):
        s1 = (state >> 8) ^ _TABLE[(state ^ b1) & 0xFF]
        for b2 in range(256):
            s2 = (s1 >> 8) ^ _TABLE[(s1 ^ b2) & 0xFF]
            fwd[s2] = (b1, b2)

    
    for b4 in range(256):
        p4 = reverse_step(desired, b4)
        if p4 is None:
            continue
        for b3 in range(256):
            p3 = reverse_step(p4, b3)
            if p3 is None:
                continue
            if p3 in fwd:
                b1, b2 = fwd[p3]
                result = prefix + bytes([b1, b2, b3, b4])
                if crc32c(result) == target:
                    return result
    return None
 
def exploit():
    io = remote(HOST, PORT)

    banner = io.recvuntil(b"Enter hex string A : ").decode()
    print(banner)

   
    target = None
    for line in banner.splitlines():
        if "hashed to" in line:
            target = int(line.strip().split()[-1], 16)
            break

    if target is None:
        log.failure("Could not parse target hash")
        io.close()
        return

    log.info(f"Target : {target:#010x}")

    
    a = forge(b"AAAA", target)
    b = forge(b"BBBB", target)

    if not a or not b:
        log.failure("Forge failed")
        io.close()
        return

    log.success(f"A = {a.hex()}  CRC = {crc32c(a):#010x}")
    log.success(f"B = {b.hex()}  CRC = {crc32c(b):#010x}")

    # Send as hex strings
    io.sendline(a.hex().encode())
    io.recvuntil(b"Enter hex string B : ")
    io.sendline(b.hex().encode())

    response = io.recvall(timeout=5).decode()
    print(response)

    if "CTF{" in response:
        flag = response.split("FLAG: ")[1].split("\n")[0].strip()
        log.success(f"FLAG: {flag}")
    else:
        log.failure("No flag received")

    io.close()


if __name__ == "__main__":
    exploit()