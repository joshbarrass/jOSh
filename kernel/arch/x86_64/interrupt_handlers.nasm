        BITS 64
%macro pusha_64 0
        push rax
        push rbx
        push rcx
        push rdx
        push rsi
        push rdi
        push rsp
        push rbp
        push r8
        push r9
        push r10
        push r11
        push r12
        push r13
        push r14
        push r15
%endmacro
%macro popa_64 0
        pop r15
        pop r14
        pop r13
        pop r12
        pop r11
        pop r10
        pop r9
        pop r8
        pop rbp
        pop rsp
        pop rdi
        pop rsi
        pop rdx
        pop rcx
        pop rbx
        pop rax
%endmacro

global df_handler
df_handler:
        pusha_64
        extern do_df
        call do_df
        popa_64
        ;; remove error code error code is a qword in x86_64
        ;; https://wiki.osdev.org/Interrupt_Service_Routines#x86-64
        ;; "If the interrupt is an exception, the CPU will push an
        ;; error code onto the stack, padded with bytes to form a
        ;; quadword. "
        add rsp, 8
        iretq
