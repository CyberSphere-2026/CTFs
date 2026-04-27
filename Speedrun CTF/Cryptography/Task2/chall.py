k = b"???????????"

with open("flag.png", "rb") as f:
    d = f.read()

x = bytes(b ^ k[i % len(k)] for i, b in enumerate(d))

with open("out.png", "wb") as f:
    f.write(x)