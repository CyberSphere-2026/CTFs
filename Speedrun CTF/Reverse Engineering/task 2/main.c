#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define FLAG_LEN 36
#define NUM_CHECKS 22

static const uint8_t TARGET[NUM_CHECKS] = {
    0x69, 0x6E, 0x4B, 0x28, 0x75, 0xCB,
    0xC8, 0x04, 0x93, 0x5C, 0x6D, 0x88,
    0x12, 0x58, 0x13, 0x0C, 0x5B, 0x7F,
    0x1C, 0xA5, 0x47, 0x2F
};

static int verify(const uint8_t *f) {
    if (strncmp((const char *)f, "Cybersphere{", 12) != 0) return 0;
    int32_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
    int32_t x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21;

    x5  = ((int32_t)f[21] - (int32_t)f[4]) ^ 0x30;
    x18 = (int32_t)f[23] ^ 0x42;
    x3  = (int32_t)f[3]  ^ 0x46;
    x21 = (int32_t)f[29] - 32;
    x10 = (int32_t)f[17] - (int32_t)f[16] + 82;
    x7  = (int32_t)f[6]  ^ (int32_t)f[7];
    x0  = (int32_t)f[0]  - 50  + (int32_t)f[1];
    x16 = 6 * ((int32_t)f[18] ^ (int32_t)f[19]) + 54;
    x2  = 4  * (int32_t)f[2];
    x19 = (int32_t)f[25] - (int32_t)f[26] / 2 - 55;
    x11 = (int32_t)f[11] - 38;
    x6  = (int32_t)f[6]  * (int32_t)f[5] + 99;
    x4  = 36 - ((int32_t)f[3] - (int32_t)f[4]);
    x20 = 4  * (int32_t)f[27] - ((int32_t)f[28] + 128);
    x9  = ((int32_t)f[9]  & 0x37) - 3;
    x1  = (int32_t)f[1]  - 100 + (int32_t)f[2];
    x15 = (int32_t)f[26] + 5;
    x8  = ((int32_t)f[7] + 45) ^ (int32_t)f[8];
    x13 = (int32_t)f[13] - (int32_t)f[14] - 1;
    x12 = 4  * (((int32_t)f[12] ^ (int32_t)f[6]) + 4);
    x14 = (int32_t)f[22];
    x17 = (int32_t)f[21] + 49 + ((int32_t)f[20] ^ 0x73);

    uint8_t s[NUM_CHECKS];

    s[16] = (uint8_t)(((x18 & 0x36) + 53)        & 0xFF);
    s[9]  = (uint8_t)(((x10 ^ 3) - x8 + 54)      & 0xFF);
    s[0]  = (uint8_t)(((x0  ^ 2) - 31)            & 0xFF);
    s[12] = (uint8_t)(((x13 + 18) ^ 0x1A)         & 0xFF);
    s[3]  = (uint8_t)((x2   ^ 0xA0)               & 0xFF);
    s[17] = (uint8_t)((x20  ^ 0xFD)               & 0xFF);
    s[7]  = (uint8_t)((x7   - 20)                 & 0xFF);
    s[4]  = (uint8_t)(((x3  ^ 0x4D) + 7)          & 0xFF);
    s[18] = (uint8_t)(((x20 >> x21) ^ 0x1C)       & 0xFF);
    s[2]  = (uint8_t)(((4   * x1) ^ 0x97)         & 0xFF);
    s[11] = (uint8_t)((x11  + 51)                 & 0xFF);
    s[1]  = (uint8_t)((((x1 % 2) ^ x0) - 29)      & 0xFF);
    s[15] = (uint8_t)((x17  ^ 0xE5)               & 0xFF);
    s[10] = (uint8_t)((x9   ^ 0x42)               & 0xFF);
    s[14] = (uint8_t)((x15  - 37)                 & 0xFF);
    s[8]  = (uint8_t)((x8   ^ 0x63)               & 0xFF);
    s[6]  = (uint8_t)((x6   + 21)                 & 0xFF);
    s[13] = (uint8_t)((x14  - 7)                  & 0xFF);
    s[5]  = (uint8_t)((4    * x5 - 1)             & 0xFF);
    s[19] = (uint8_t)((x4   + 116)                & 0xFF);
    s[20] = (uint8_t)((x12  ^ 0xB3)               & 0xFF);
    s[21] = (uint8_t)((x19  ^ 0x34)               & 0xFF);

    uint8_t diff = 0;
    for (int i = 0; i < NUM_CHECKS; i++)
        diff |= (s[i] ^ TARGET[i]);

    if (diff != 0) return 0;

    if (((f[17] * 3)   & 0xFF) != 153) return 0;
    if (((f[5]  + 105) & 0xFF) != 220) return 0;
    if (((f[9]  - 93)  & 0xFF) != 21)  return 0;
    if (((f[11] * 42)  & 0xFF) != 46)  return 0;
    if (((f[23] - 84)  & 0xFF) != 24)  return 0;
    if (((f[26] + 13)  & 0xFF) != 64)  return 0;
    if (((f[29] - 29)  & 0xFF) != 22)  return 0;

    if (((f[18] * 97)  & 0xFF) != 255) return 0;
    if ( (f[19] ^ 0x3A)        != 0x43) return 0;

    if (((f[24] + 77)  & 0xFF) != 126) return 0;
    if (((f[34] - 111) & 0xFF) != 178) return 0;
    if (((f[15] * 81)  & 0xFF) != 48)  return 0;
    if (((f[33] * 6)   & 0xFF) != 198) return 0;
    if (((f[30] + 64)  & 0xFF) != 176) return 0;
    if (((f[35] - 64)  & 0xFF) != 61)  return 0;
    if ( (f[31] ^ 0x1B)        != 0x62) return 0;
    if (((f[10] + 54)  & 0xFF) != 155) return 0;
    if (((f[32] - 118) & 0xFF) != 171) return 0;

    if (((f[13] * 3)   & 0xFF) != 29)  return 0;
    if (((f[14] + 22)  & 0xFF) != 126) return 0;
    if (((f[27] * 3)   & 0xFF) != 29)  return 0;
    if (((f[28] + 4)   & 0xFF) != 126) return 0;
    if (((f[16] * 3)   & 0xFF) != 80)  return 0;

    return 1;
}

int main(void) {
    char buf[256];
    printf("Enter the flag: ");

    if (!fgets(buf, sizeof(buf), stdin)) {
        printf("Error reading input!\n");
        return 1;
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';

    if (len != FLAG_LEN) {
        printf("Incorrect flag length!\n");
        return 1;
    }

    if (verify((const uint8_t *)buf)) {
        printf("Correct flag!\n");
    } else {
        printf("Wrong flag!\n");
        return 1;
    }

    return 0;
}