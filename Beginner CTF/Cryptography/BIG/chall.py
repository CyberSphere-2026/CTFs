from Crypto.Util.number import *
def gen():
    p=getPrime(1024)
    q=p+2
    while(not isPrime(q)):
        q+=2
    return p, q
p,q=gen()
m=bytes_to_long(b'??????????????????????????')
n=p*q
e=65537
phi=(p-1)*(q-1)
c=pow(m,e,n)
with open("flag.txt","w") as f:
    f.write(f"n={n}\n")
    f.write(f"e={e} \n")
    f.write(f"c={c}\n")
