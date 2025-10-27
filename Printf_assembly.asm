; Fichier: Printf.assembly.asm

section .text
    global asm_print_string


asm_print_string:
    push    rbp             
    mov     rbp, rsp        
    push    rdi             
    mov     rsi, qword [rbp - 8] 

    xor     rdx, rdx        

.loop_count_length:
    cmp     byte [rsi + rdx], 0 
    je      .end_count_length   
    inc     rdx             
    jmp     .loop_count_length  

.end_count_length:
    
    mov     rax, 1         
    mov     rdi, 1         
    mov     rsi, qword [rbp - 8] 
    syscall                

    pop     rdi             
    pop     rbp             
    ret                     