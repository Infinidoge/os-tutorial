[bits 32]
[extern _start] ; Define calling point. Must have same name as kernel.c '_start' function
call _start ; Calls the C function. The linker will know where it is placed in memory
jmp $
