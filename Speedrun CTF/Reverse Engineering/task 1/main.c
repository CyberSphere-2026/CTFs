#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static unsigned int seeds[] = {0xDEADBEEF, 0xCAFEBABE, 0xBAADF00D, 0xFEEDFACE};

static unsigned char rotate_left(unsigned char value, int shift) {
    shift %= 8;
    return (value << shift) | (value >> (8 - shift));
}

static unsigned char rotate_right(unsigned char value, int shift) {
    shift %= 8;
    return (value >> shift) | (value << (8 - shift));
}

void rot47(char *input, int len) {
    for (int i = 0; i < len; i++) {
        if (input[i] >= 33 && input[i] <= 126) {
            input[i] = ((input[i] - 33 + 47) % 94) + 33;
        }
    }
}

void encrypt(char *input) {
    int len_input = (int)strlen(input);
    int k = rand() % 4;
    srand(seeds[k]);

    for (int i = 0; i < len_input; i++) {
        input[i] ^= rand() % 256;
    }

    for (int i = 0; i < len_input; i++) {
        input[i] = (char)rotate_left((unsigned char)input[i], i % 8);
    }

    rot47(input, len_input);

    for (int i = 0; i < len_input; i++) {
        input[i] = (char)rotate_right((unsigned char)input[i], (i + 3) % 8);
    }

    char hex_output[512] = {0};
    for (int i = 0; i < len_input; i++) {
        sprintf(hex_output + i * 2, "%02x", (unsigned char)input[i]);
    }

    int hex_len = len_input * 2;
    for (int i = 0; i < hex_len / 2; i += 2) {
        char tmp0 = hex_output[i];
        char tmp1 = hex_output[i + 1];
        hex_output[i]               = hex_output[hex_len - i - 2];
        hex_output[i + 1]           = hex_output[hex_len - i - 1];
        hex_output[hex_len - i - 2] = tmp0;
        hex_output[hex_len - i - 1] = tmp1;
    }

    strncpy(input, hex_output, hex_len + 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));
    printf("You've got: %s\n", argv[1]);
    encrypt(argv[1]);
    printf("You get: %s\n", argv[1]);

    return EXIT_SUCCESS;
}