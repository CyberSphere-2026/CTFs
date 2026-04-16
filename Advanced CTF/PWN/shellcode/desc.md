- difficulty : hard 
- description : seccomp that only allows exit . the flag is read in memory , you can execute your shellcode , but you have no clue about where the read flag is and you cant just print it 
- solver : solve.py
- solver description : 
    - look for instruction that try can look into memory without crashing if memory is invalid , 
    look example , if rbx is valid memory , reading from it the second time will take less time and cpu cycles > so based on the cpu timestamp we can get from rdtsc instruction , we can see how long it took > we can brute force all memory (this will take too long on docker so i just gave 3 bytes as leak to make it easier on server )
    ```assembly
        vmaskmovps ymm0, ymm0, ymmword ptr[rbx]
        mfence
        rdtsc

        vmaskmovps ymm0, ymm0, ymmword ptr[rbx]
        mfence
        rdtsc
    ```
    - leak the flag char by char using an oracle (infinite loop to create sleep duration)