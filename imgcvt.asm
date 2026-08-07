; imgCvtGrayDoubleToInt(double* input, uint8_t* output, int height, int width)
; Windows x64 calling convention:
;   RCX = input   (double*)
;   RDX = output  (uint8_t*)
;   R8D = height  (int)
;   R9D = width   (int)

default rel

section .data
align 16
scale255:  dq 255.0        ; multiplier constant, used via movsd

section .text
global imgCvtGrayDoubleToInt

imgCvtGrayDoubleToInt:
    ; --- prologue ---
    push    rbx
    push    rsi
    push    rdi

    mov     rsi, rcx            ; rsi = input pointer
    mov     rdi, rdx            ; rdi = output pointer
    imul    r8d, r9d            ; r8d = height * width = total pixel count
    xor     rbx, rbx            ; rbx = loop index i = 0

    movsd   xmm2, [scale255]    ; xmm2 = 255.0 (kept constant for the loop)

.loop:
    cmp     rbx, r8
    jge     .done

    movsd   xmm0, [rsi + rbx*8]     ; xmm0 = input[i]  (double, 8 bytes/elem)
    mulsd   xmm0, xmm2              ; xmm0 = input[i] * 255.0
    cvtsd2si eax, xmm0              ; round-to-nearest per current rounding mode -> eax

    ; clamp to [0, 255] just in case of float edge cases
    cmp     eax, 0
    jge     .not_below
    xor     eax, eax
.not_below:
    cmp     eax, 255
    jle     .not_above
    mov     eax, 255
.not_above:

    mov     [rdi + rbx], al        ; output[i] = (uint8_t)eax

    inc     rbx
    jmp     .loop

.done:
    pop     rdi
    pop     rsi
    pop     rbx
    ret
