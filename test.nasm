global _start     
section .bss     
section .data     
section .text     
     
_start:     
     xor     ebp, ebp
     call    main
     mov     rdi, rax
     mov     rax, 60
     syscall
     
main:     
     push    rbp
     mov     rbp, rsp
     mov     BYTE [rbp-1], 1
     mov     DWORD [rbp-8], 3
     cmp     BYTE [rbp-1], 0
     je      .L3
     mov     DWORD [rbp-8], 10
     jmp     .L2
.L3:     
     cmp     BYTE [rbp-1], 0
     je      .L4
     mov     DWORD [rbp-8], 20
     jmp     .L2
.L4:     
     cmp     BYTE [rbp-1], 0
     je      .L5
     mov     DWORD [rbp-8], 40
     jmp     .L2
.L5:     
     mov     DWORD [rbp-8], 30
.L2:     
     mov     QWORD rax, [rbp-8]
     pop     rbp
     ret
     pop     rbp
     ret
