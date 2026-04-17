#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int seeds[] = {0xDEADBEEF, 0xCAFEBABE, 0xBAADF00D, 0xFEEDFACE};

static unsigned char rotate_left(unsigned char value, int shift) {
    shift %= 8;
    return (value << shift) | (value >> (8 - shift));
}

static unsigned char rotate_right(unsigned char value, int shift) {
    shift %= 8;
    return (value >> shift) | (value << (8 - shift));
}

static unsigned char unrot47(unsigned char c) {
    if (c >= 33 && c <= 126) {
        return ((c - 33 + 94 - 47) % 94) + 33;
    }
    return c;
}

void reverse_hex_pairs(char *hex, int hex_len) {
    for (int i = 0; i < hex_len / 2; i += 2) {
        char tmp0 = hex[i];
        char tmp1 = hex[i + 1];
        hex[i]               = hex[hex_len - i - 2];
        hex[i + 1]           = hex[hex_len - i - 1];
        hex[hex_len - i - 2] = tmp0;
        hex[hex_len - i - 1] = tmp1;
    }
}

int hex_to_bytes(const char *hex, unsigned char *out, int *out_len) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    *out_len = len / 2;
    for (int i = 0; i < *out_len; i++) {
        if (sscanf(hex + i * 2, "%2hhx", &out[i]) != 1) return 0;
    }
    return 1;
}

void try_decrypt(unsigned char *ciphertext, int len, unsigned int seed) {
    unsigned char buf[256];
    memcpy(buf, ciphertext, len);

    for (int i = 0; i < len; i++) {
        buf[i] = rotate_left(buf[i], (i + 3) % 8);
    }

    for (int i = 0; i < len; i++) {
        buf[i] = unrot47(buf[i]);
    }

    for (int i = 0; i < len; i++) {
        buf[i] = rotate_right(buf[i], i % 8);
    }

    srand(seed);
    for (int i = 0; i < len; i++) {
        buf[i] ^= rand() % 256;
    }

    buf[len] = '\0';

    if (strncmp((char *)buf, "Cybersphere{", 12) == 0 && buf[len - 1] == '}') {
        printf("Seed 0x%08X -> %s\n", seed, buf);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hex_ciphertext>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char hex[512];
    strncpy(hex, argv[1], sizeof(hex) - 1);
    int hex_len = strlen(hex);

    reverse_hex_pairs(hex, hex_len);

    unsigned char ciphertext[256];
    int ct_len = 0;

    if (!hex_to_bytes(hex, ciphertext, &ct_len)) {
        fprintf(stderr, "Invalid hex input\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 4; i++) {
        try_decrypt(ciphertext, ct_len, seeds[i]);
    }

    return EXIT_SUCCESS;
}