import hashlib
import base64

username = "U3BlZWQ="

username = base64.b64decode(username).decode()

md5_hash = hashlib.md5(username.encode()).hexdigest()

xored = "".join(chr(ord(c) ^ 0x42) for c in md5_hash)

key = hashlib.sha256(xored.encode()).hexdigest()

print(f"Username: {username}")
print(f"Key: {key}")
print(f"Flag: Cybersphere{{{username}_{key}}}")