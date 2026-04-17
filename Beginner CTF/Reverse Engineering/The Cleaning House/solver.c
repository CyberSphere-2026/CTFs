#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



static const char KEY[] = "CINEAMA";
#define KEYLEN 7

static void un_xor_key(uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++)
        buf[i] ^= (uint8_t)KEY[i % KEYLEN];
}

static void reverse_buf(uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len / 2; i++) {
        uint8_t tmp      = buf[i];
        buf[i]           = buf[len - 1 - i];
        buf[len - 1 - i] = tmp;
    }
}

static void un_xor_rand(uint8_t *buf, size_t len, uint16_t seed) {
    srand((unsigned int)seed);
    for (size_t i = 0; i < len; i++)
        buf[i] ^= (uint8_t)(rand() & 0xFF);
}


static uint8_t un_odd_even(uint8_t b) {
    uint8_t x_odd  = (uint8_t)((b - 0x13) & 0xFF);
    if (x_odd & 1) {
        if (((uint8_t)((x_odd + 0x13) & 0xFF)) == b)
            return x_odd;
    }
    uint8_t x_even = (uint8_t)((b + 0x47) & 0xFF);
    if (!(x_even & 1)) {
        if (((uint8_t)((x_even - 0x47) & 0xFF)) == b)
            return x_even;
    }
    return x_odd;
}

static uint8_t rotr8_3(uint8_t b) {
    return (uint8_t)((b >> 3) | (b << 5));
}

static uint8_t nibble_swap(uint8_t b) {
    return (uint8_t)(((b & 0x0F) << 4) | ((b & 0xF0) >> 4));
}

static int hex_decode(const char *hex, uint8_t *out, size_t *out_len) {
    size_t hlen = strlen(hex);
    if (hlen % 2 != 0) return -1;
    *out_len = hlen / 2;
    for (size_t i = 0; i < *out_len; i++) {
        unsigned int byte;
        if (sscanf(hex + i * 2, "%2x", &byte) != 1) return -1;
        out[i] = (uint8_t)byte;
    }
    return 0;
}

static void decrypt_with_seed(const uint8_t *cipher, size_t len,
                               uint16_t seed, uint8_t *out) {
    uint8_t *buf = (uint8_t *)malloc(len);
    memcpy(buf, cipher, len);

    un_xor_key(buf, len);

    reverse_buf(buf, len);

    un_xor_rand(buf, len, seed);

    for (size_t i = 0; i < len; i++)
        buf[i] = un_odd_even(buf[i]);

    for (size_t i = 0; i < len; i++)
        buf[i] = rotr8_3(buf[i]);

    for (size_t i = 0; i < len; i++)
        buf[i] = nibble_swap(buf[i]);

    memcpy(out, buf, len);
    out[len] = '\0';
    free(buf);
}

static const char FLAG_PREFIX[] = "Cybersphere{";
#define PREFIX_LEN 12

static int is_valid_flag(const uint8_t *buf, size_t len) {
    if (len < PREFIX_LEN + 1) return 0;
    if (memcmp(buf, FLAG_PREFIX, PREFIX_LEN) != 0) return 0;
    if (buf[len - 1] != '}') return 0;
    for (size_t i = 0; i < len; i++)
        if (buf[i] < 0x20 || buf[i] > 0x7E) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <encrypted_hex_string>\n", argv[0]);
        return 1;
    }

    const char *hex = argv[1];
    size_t cipher_len = 0;
    uint8_t cipher[512] = {0};

    if (hex_decode(hex, cipher, &cipher_len) != 0) {
        fprintf(stderr, "Invalid\n");
        return 1;
    }

    uint8_t result[512] = {0};
    int found = 0;
    for (uint32_t seed = 0; seed <= 0xFFFF; seed++) {
        decrypt_with_seed(cipher, cipher_len, (uint16_t)seed, result);

        if (is_valid_flag(result, cipher_len)) {
            printf("SEED FOUND : 0x%04X (%u)\n", seed, seed);
            printf("FLAG       : %s\n", (char *)result);
            found = 1;
        }
    }
    return found ? 0 : 1;
}