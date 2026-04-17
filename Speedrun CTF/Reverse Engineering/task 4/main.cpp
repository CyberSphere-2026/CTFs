#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <openssl/evp.h>
using namespace std;

void xor_encrypt(string &data, int key) {
    for (size_t i = 0; i < data.size(); i++) {
        data[i] ^= key;
    }
}

string base64_encode(string input) {
    string output;
    string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) output.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (output.size() % 4) output.push_back('=');
    return output;
}

string hash_digest(const string &input, const char *algo) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_Digest((unsigned char*)input.c_str(), input.size(), hash, &len, EVP_get_digestbyname(algo), NULL);

    string output = "";
    for (unsigned int i = 0; i < len; i++) {
        char buf[3];
        sprintf(buf, "%02x", hash[i]);
        output += buf;
    }
    return output;
}

bool check_key(string username, string key) {
    string md5_hash = hash_digest(username, "MD5");
    xor_encrypt(md5_hash, 0x42);
    string final_hash = hash_digest(md5_hash, "SHA256");
    return final_hash == key;
}

int main() {
    string username;
    cout << "So, who in the blue hell are you?" << endl;
    cin >> username;
    cout << "So, " << username << " What do you wanna talk about?" << endl;
    string key;
    cin >> key;
    if (base64_encode(username) == "U3BlZWQ=" && check_key(username, key)) {
        cout << "You are him!" << endl;
        cout << "Cybersphere{" << username << "_" << key << "}" << endl;
    } else {
        cout << "Bad luck I guess!" << endl;
        exit(0);
    }
    return 0;
}