from z3 import *

FLAG_LEN  = 36
NUM_CHECK = 22

TARGET = [
    0x69, 0x6E, 0x4B, 0x28, 0x75, 0xCB,
    0xC8, 0x04, 0x93, 0x5C, 0x6D, 0x88,
    0x12, 0x58, 0x13, 0x0C, 0x5B, 0x7F,
    0x1C, 0xA5, 0x47, 0x2F
]

f = [BitVec(f'f{i}', 32) for i in range(FLAG_LEN)]
s = Solver()

banned = list('^,"?[]*()|;<>\\`&./')
for i in range(FLAG_LEN):
    s.add(f[i] >= 33, f[i] <= 125)
    for b in banned:
        s.add(f[i] != ord(b))

s.add(((f[5]  + 105) & 0xFF) == 220)
s.add(((f[9]  - 93)  & 0xFF) == 21)
s.add(((f[11] * 42)  & 0xFF) == 46)
s.add(((f[23] - 84)  & 0xFF) == 24)
s.add(((f[26] + 13)  & 0xFF) == 64)
s.add(((f[29] - 29)  & 0xFF) == 22)
s.add(((f[18] * 97)  & 0xFF) == 255)
s.add( (f[19] ^ 0x3A)        == 0x43)
s.add(((f[24] + 77)  & 0xFF) == 126)
s.add(((f[34] - 111) & 0xFF) == 178)
s.add(((f[15] * 81)  & 0xFF) == 48)
s.add(((f[33] * 6)   & 0xFF) == 198)
s.add(((f[30] + 64)  & 0xFF) == 176)
s.add(((f[35] - 64)  & 0xFF) == 61)
s.add( (f[31] ^ 0x1B)        == 0x62)
s.add(((f[10] + 54)  & 0xFF) == 155)
s.add(((f[32] - 118) & 0xFF) == 171)

s.add(((f[13] * 3)   & 0xFF) == 29)
s.add(((f[14] + 22)  & 0xFF) == 126)
s.add(((f[27] * 3)   & 0xFF) == 29)
s.add(((f[28] + 4)   & 0xFF) == 126)
s.add(((f[16] * 3)   & 0xFF) == 80)
s.add(((f[17] * 3)   & 0xFF) == 153)

x5  = (f[21] - f[4])  ^ 0x30
x18 = f[23] ^ 0x42
x3  = f[3]  ^ 0x46
x21 = f[29] - 32
x10 = f[17] - f[16] + 82
x7  = f[6]  ^ f[7]
x0  = f[0]  - 50 + f[1]
x2  = 4 * f[2]
x19 = f[25] - f[26] / 2 - 55
x11 = f[11] - 38
x6  = f[6]  * f[5] + 99
x4  = 36 - (f[3] - f[4])
x20 = 4 * f[27] - (f[28] + 128)
x9  = (f[9] & 0x37) - 3
x1  = f[1]  - 100 + f[2]
x15 = f[26] + 5
x8  = (f[7] + 45) ^ f[8]
x13 = f[13] - f[14] - 1
x12 = 4 * ((f[12] ^ f[6]) + 4)
x14 = f[22]
x17 = f[21] + 49 + (f[20] ^ 0x73)

sv = [None] * NUM_CHECK
sv[16] = ((x18 & 0x36) + 53)      & 0xFF
sv[9]  = ((x10 ^ 3) - x8 + 54)    & 0xFF
sv[0]  = ((x0  ^ 2) - 31)         & 0xFF
sv[12] = ((x13 + 18) ^ 0x1A)      & 0xFF
sv[3]  = (x2   ^ 0xA0)            & 0xFF
sv[17] = (x20  ^ 0xFD)            & 0xFF
sv[7]  = (x7   - 20)              & 0xFF
sv[4]  = ((x3  ^ 0x4D) + 7)       & 0xFF
sv[18] = ((x20 >> x21) ^ 0x1C)    & 0xFF
sv[2]  = ((4   * x1) ^ 0x97)      & 0xFF
sv[11] = (x11  + 51)              & 0xFF
sv[1]  = (((x1 % 2) ^ x0) - 29)   & 0xFF
sv[15] = (x17  ^ 0xE5)            & 0xFF
sv[10] = (x9   ^ 0x42)            & 0xFF
sv[14] = (x15  - 37)              & 0xFF
sv[8]  = (x8   ^ 0x63)            & 0xFF
sv[6]  = (x6   + 21)              & 0xFF
sv[13] = (x14  - 7)               & 0xFF
sv[5]  = (4    * x5 - 1)          & 0xFF
sv[19] = (x4   + 116)             & 0xFF
sv[20] = (x12  ^ 0xB3)            & 0xFF
sv[21] = (x19  ^ 0x34)            & 0xFF

for i in range(NUM_CHECK):
    s.add(sv[i] == TARGET[i])

result = s.check()
if result == sat:
    model = s.model()
    flag = ''.join(chr(int(str(model[f[i]]))) for i in range(FLAG_LEN))
    print(f"Flag: {flag}")
else:
    print("unsat")