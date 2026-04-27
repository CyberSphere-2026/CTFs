from Crypto.Cipher import AES
import os
key=os.urandom(16)
nonce=os.urandom(8)
ciphertext=b'????????????????????????'
decoy = b'[admin] login from 192.168.1.1 at 14:32:07 UTC!!' 
print(len(ciphertext)==len(decoy))
cipher1 = AES.new(key, AES.MODE_CTR, nonce=nonce)
c1 = cipher1.encrypt(ciphertext)
cipher2 = AES.new(key, AES.MODE_CTR, nonce=nonce)
c2 = cipher2.encrypt(decoy)
with open("flag.txt","w") as f:
    f.write(f"flag={c1.hex()}\ndecoy={c2.hex()}")