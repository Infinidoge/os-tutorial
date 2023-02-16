; Defined in isr.c
[extern isr_handler]
[extern irq_handler]

; Common ISR code
isr_common_stub:
    ; 1. Save CPU state
    pusha ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    mov ax, ds ; Lower 16-bits of eax = ds.
    push eax ; save the data segment descriptor
    mov ax, 0x10  ; kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 2. Call C handler
    call isr_handler

    ; 3. Restore state
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8 ; Cleans up the pushed error code and pushed ISR number
    sti
    iret ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

; Common IRQ code. Identical to ISR code except for the 'call'
; and the 'pop ebx'
irq_common_stub:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq_handler ; Different than the ISR code
    pop ebx  ; Different than the ISR code
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    sti
    iret

; We don't get information about which interrupt was called
; when the handler is run, so we will need to have a different handler
; for every interrupt.
; Furthermore, some interrupts push an error code onto the stack but others
; don't, so we will push a dummy error code for those which don't, so that
; we have a consistent stack for all of them.


; Macro setup from https://github.com/royreznik/rezOS-32bit/blob/master/cpu/interrupt.asm
%macro ISR_NOERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        cli                     ; Disable interrupts
        push byte 0             ; Push dummy error code, as the fist 16 interrupts doesn't have one
        push byte %1            ; Push the interrupt number
        jmp isr_common_stub     ; Call common handler
%endmacro


%macro ISR_ERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        cli
        push byte %1
        jmp isr_common_stub
%endmacro


%macro IRQ 2
    [GLOBAL irq%1]
    irq%1:
        cli
        push byte %1
        push byte %2
        jmp irq_common_stub
%endmacro

ISR_NOERRCODE 0; 0: Divide By Zero Exception
ISR_NOERRCODE 1; 1: Debug Exception
ISR_NOERRCODE 2; 2: Non Maskable Interrupt Exception
ISR_NOERRCODE 3; 3: Int 3 Exception
ISR_NOERRCODE 4; 4: INTO Exception
ISR_NOERRCODE 5; 5: Out of Bounds Exception
ISR_NOERRCODE 6; 6: Invalid Opcode Exception
ISR_NOERRCODE 7; 7: Coprocessor Not Available Exception
ISR_ERRCODE 8; 8: Double Fault Exception (With Error Code!)
ISR_NOERRCODE 9; 9: Coprocessor Segment Overrun Exception
ISR_ERRCODE 10; 10: Bad TSS Exception (With Error Code!)
ISR_ERRCODE 11; 11: Segment Not Present Exception (With Error Code!)
ISR_ERRCODE 12; 12: Stack Fault Exception (With Error Code!)
ISR_ERRCODE 13; 13: General Protection Fault Exception (With Error Code!)
ISR_ERRCODE 14; 14: Page Fault Exception (With Error Code!)
ISR_NOERRCODE 15; 15: Reserved Exception
ISR_NOERRCODE 16; 16: Floating Point Exception
ISR_NOERRCODE 17; 17: Alignment Check Exception
ISR_NOERRCODE 18; 18: Machine Check Exception
ISR_NOERRCODE 19; 19: Reserved
ISR_NOERRCODE 20; 20: Reserved
ISR_NOERRCODE 21; 21: Reserved
ISR_NOERRCODE 22; 22: Reserved
ISR_NOERRCODE 23; 23: Reserved
ISR_NOERRCODE 24; 24: Reserved
ISR_NOERRCODE 25; 25: Reserved
ISR_NOERRCODE 26; 26: Reserved
ISR_NOERRCODE 27; 27: Reserved
ISR_NOERRCODE 28; 28: Reserved
ISR_NOERRCODE 29; 29: Reserved
ISR_NOERRCODE 30; 30: Reserved
ISR_NOERRCODE 31; 31: Reserved

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
