section .data
numbers_size:
    dd 8
numbers:
    dd 10, 34, 55, 106, 44, 0, 45, 400

section .text
global _start

_start:
    mov ecx, numbers         ; pointer to the array
    mov ebx, [numbers_size]  ; iteration count (and later divisor)
    xor eax, eax             ; eax will hold the running sum

sum_loop:
    add eax, [ecx]           ; sum += numbers[current]
    add ecx, 4               ; advance to next 4-byte element
    dec ebx                  ; one fewer element left
    jnz sum_loop             ; loop until we've processed numbers_size elements

    ; now eax = sum of all elements
    mov ebx, [numbers_size]  ; divisor = length of the array
    xor edx, edx             ; clear high 32 bits for 32-bit divide
    div ebx                  ; eax = sum / numbers_size, edx = remainder

    ; return integer average in ebx as the exit status
    mov ebx, eax             ; average (assumed < 256)
    mov eax, 1               ; sys_exit
    int 80h