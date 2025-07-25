        BITS 64

        extern _entry_c
        extern stack_top

global _entry
_entry:
        ;; backup rax to rcx
        mov rcx, rax
        ;; set up the stack via rax
        mov rax, stack_top
        mov rsp, rax
        ;; forward EAX and EBX according to sysv convention
        xor rdi, rdi
        mov edi, ecx
        xor rsi, rsi
        mov esi, ebx
        jmp _entry_c
