from Crypto.Util.number import *
p=getPrime(1024)
q=getPrime(1024)
n=p*q
e=3
m=bytes_to_long(b'???????????????????????????')
c=pow(m,e,n)

phi=(p-1)*(q-1)
d=inverse(e,phi)
with open("flag.txt","w") as f:
    f.write(f"c={c}\n")
    f.write(f"n={n}\n")
    f.write(f"e={e}\n")
