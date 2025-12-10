section .data
prompt:
    db "Please enter your name: "
prompt_len  equ $ - prompt

greeting:
    db "Nice to meet you, "
greeting_len equ $ - greeting

section .bss
name_buf:
    resb 20                  ; buffer for up to 20 characters

section .text
global _start

_start:
    ; write the prompt: "Please enter your name: "
    mov eax, 4               ; sys_write
    mov ebx, 1               ; stdout
    mov ecx, prompt          ; address of prompt string
    mov edx, prompt_len      ; length of prompt
    int 80h

    ; read up to 20 bytes of user input into name_buf
    mov eax, 3               ; sys_read
    mov ebx, 0               ; stdin
    mov ecx, name_buf        ; buffer
    mov edx, 20              ; max bytes to read
    int 80h                  ; on return, eax = number of bytes read

    mov esi, eax             ; save number of bytes read in esi

    ; write the greeting: "Nice to meet you, "
    mov eax, 4               ; sys_write
    mov ebx, 1               ; stdout
    mov ecx, greeting        ; address of greeting string
    mov edx, greeting_len    ; length of greeting
    int 80h

    ; write back exactly the bytes the user entered
    mov eax, 4               ; sys_write
    mov ebx, 1               ; stdout
    mov ecx, name_buf        ; address of name buffer
    mov edx, esi             ; number of bytes read from stdin
    int 80h

    ; exit with status 0
    mov eax, 1               ; sys_exit
    mov ebx, 0               ; exit status
    int 80h