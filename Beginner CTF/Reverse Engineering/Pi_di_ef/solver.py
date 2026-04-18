#!/usr/bin/env python3

def rc4_decrypt(key_string, ciphertext_array):
    S = list(range(256))
    j = 0
    for i in range(256):
        j = (j + S[i] + ord(key_string[i % len(key_string)])) % 256
        S[i], S[j] = S[j], S[i]
    
    i = j = 0
    plaintext = []
    for byte in ciphertext_array:
        i = (i + 1) % 256
        j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        
        decrypted_char = byte ^ S[(S[i] + S[j]) % 256]
        plaintext.append(chr(decrypted_char))
        
    return "".join(plaintext)

def main():

    
    extracted_key = "h0t3l_v1b3S"
    
    extracted_target = [71, 176, 103, 115, 203, 106, 209, 233, 232, 205, 154, 246, 154, 64, 187, 234, 170, 76, 10, 23, 228, 181, 122, 171, 221, 157, 144, 227, 106, 141, 56, 221, 139, 92, 245, 104, 172, 33, 36, 133, 165, 144, 208, 65, 128, 48, 58, 128, 154, 5, 199, 212, 127, 72, 192, 41, 68]

    flag = rc4_decrypt(extracted_key, extracted_target)

    print(f"Flag : {flag}")

if __name__ == "__main__":
    main()