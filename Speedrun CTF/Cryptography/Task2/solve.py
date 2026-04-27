def xor(a: bytes, b: bytes) -> bytes:
    return bytes(x ^ y for x, y in zip(a, b))

def period(s: bytes) -> int:
    for p in range(1, len(s) + 1):
        if all(s[i] == s[i % p] for i in range(len(s))):
            return p
    return len(s)


PNG_SIG = b"\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"

with open("out.png", "rb") as f:
    enc = f.read()


stream = xor(enc[:8], PNG_SIG)


key_len = period(stream)
key = stream[:key_len]


dec = bytes(c ^ key[i % len(key)] for i, c in enumerate(enc))

with open("recovered.png", "wb") as f:
    f.write(dec)

print(f"Detected Key: {key.decode()}")
