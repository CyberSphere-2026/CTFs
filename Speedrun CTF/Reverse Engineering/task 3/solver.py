import hashlib, base64

def rc4(key, data):
    S = list(range(256)); j = 0
    for i in range(256):
        j = (j + S[i] + key[i % len(key)]) % 256; S[i], S[j] = S[j], S[i]
    i = j = 0; out = bytearray()
    for b in data:
        i = (i + 1) % 256; j = (j + S[i]) % 256; S[i], S[j] = S[j], S[i]
        out.append(b ^ S[(S[i] + S[j]) % 256])
    return bytes(out)

key = hashlib.sha256(b"7anach").digest()
raw = base64.b64decode("p77/jK4EXzWUyuxdwiRY1BHAYsLK+4kx/bO3U6zPKbLjLvQL4P8Rs4q4SsHQxGN/eDYydgFdPCBkkmqoMcyxAqvdIWnuudCIa1fcyLvoEjzJPg==")
print(rc4(key[:16], bytes([b ^ key[i % 32] for i, b in enumerate(raw)])).decode())