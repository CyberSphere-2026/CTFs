from Crypto.Cipher import AES
flag="87f1c6012e41a50c96a26b90fc692a57a0f1f5ea44cd9f99bb4882c27816abaaac174faac12d00dd572e2e5a16752e90"
decoy_enc="9fe9c009355c88449fbf6982f679135cffe98aa94ecbddc4f42fdf873f4be794aa520bed9e414b88126b5164274f7ccc"
decoy="[admin] login from 192.168.1.1 at 14:32:07 UTC!!"
flag_bytes=bytes.fromhex(flag)
decoy_bytes=bytes.fromhex(decoy_enc)
decoy_plain=decoy.encode()
for i in range(min(len(flag_bytes), len(decoy_bytes), len(decoy_plain))):
    print(chr((flag_bytes[i] ^ decoy_bytes[i]) ^ decoy_plain[i]), end="")