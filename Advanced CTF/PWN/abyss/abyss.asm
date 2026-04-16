BITS 64

section .text
global _start
global write_trace
global vuln

_start:
    mov     eax, 37
    mov     edi, 60
    syscall

    mov     eax, 157
    mov     edi, 38
    mov     esi, 1
    xor     edx, edx
    xor     r10d, r10d
    xor     r8d, r8d
    syscall

    mov     eax, 157
    mov     edi, 22
    mov     esi, 2
    lea     rdx, [rel bpf_prog]
    syscall

    call    write_trace

vuln:
    xor     eax, eax
    xor     edi, edi
    mov     rsi, rsp
    mov     edx, 0x800
    syscall
    ret

write_trace:
    mov     eax, 1
    mov     edi, 1
    lea     rsi, [rel trace_msg]
    mov     edx, 15
    syscall
    ret

section .rodata

trace_msg:  db  "entering abyss", 10

align 8
bpf_prog:
    dw      22
    times 6 db 0
    dq      bpf_filter

bpf_filter:
    dw 0x0020
    db 0, 0
    dd 4
    dw 0x0015
    db 1, 0
    dd 0xC000003E
    dw 0x0006
    db 0, 0
    dd 0x80000000
    dw 0x0020
    db 0, 0
    dd 0
    dw 0x0015
    db 0, 3
    dd 0
    dw 0x0020
    db 0, 0
    dd 0x10
    dw 0x0015
    db 14, 0
    dd 0
    dw 0x0006
    db 0, 0
    dd 0x80000000
    dw 0x0015
    db 0, 3
    dd 1
    dw 0x0020
    db 0, 0
    dd 0x10
    dw 0x0015
    db 10, 0
    dd 1
    dw 0x0006
    db 0, 0
    dd 0x80000000
    dw 0x0015
    db 8, 0
    dd 3
    dw 0x0015
    db 7, 0
    dd 15
    dw 0x0015
    db 6, 0
    dd 231
    dw 0x0015
    db 0, 1
    dd 59
    dw 0x0006
    db 0, 0
    dd 0x80000000
    dw 0x0015
    db 0, 1
    dd 322
    dw 0x0006
    db 0, 0
    dd 0x80000000
    dw 0x0015
    db 1, 0          ; vuln
    dd 257
    dw 0x0006
    db 0, 0
    dd 0x80000000
    dw 0x0006
    db 0, 0
    dd 0x7FFF0000

section .bss
storage:    resb 0x2000
