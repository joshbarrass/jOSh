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

%macro handler 2 ; arg 1: interrupt number. arg 2: interrupt pushes err code?
global handle_int%1
handle_int%1:
        push qword %1           ; push the interrupt number to the stack
        %if %2<>0
        jmp have_errcode
        %else
        jmp have_no_errcode
        %endif
%endmacro

have_no_errcode:
        ;; we need to push an error code ourselves to normalise the
        ;; stack frame
        push qword 0
have_errcode:
        ;; here we have an error code, so we can just push the
        ;; registers and call the general handler
        pusha_64
        ;; sysv abi says rdi is the first arg
        ;; pass the current stack pointer as the first arg, so we can
        ;; read it with the InterruptStackFrame struct.
        mov rdi, rsp
        extern general_int_handler
        call general_int_handler
        popa_64
        ;; remove the error code and the interrupt number
        ;; error code is a qword in x86_64
        ;; https://wiki.osdev.org/Interrupt_Service_Routines#x86-64
        ;; "If the interrupt is an exception, the CPU will push an
        ;; error code onto the stack, padded with bytes to form a
        ;; quadword."
        add rsp, 16
        iretq

%macro build_errcode_handlers 1-*
        %rep %0
        handler %1, 1
        %rotate 1
        %endrep
%endmacro

%macro build_noerrcode_handlers 1-*
        %rep %0
        handler %1, 0
        %rotate 1
        %endrep
%endmacro

build_errcode_handlers 8, 10, 11, 12, 13, 14, 17, 21
build_noerrcode_handlers 0, 1, 2, 3, 4, 5, 6, 7, 9, 15, 16, 18, 19, 20

;;; handlers for interrupts 22-31 are reserved and should always
;;; panic. We don't know what they will be and realistically don't
;;; need to care, because we will always handle them by crashing the
;;; system.
build_noerrcode_handlers 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
