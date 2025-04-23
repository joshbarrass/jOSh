        BITS 32
        MULTIBOOT_FLAG_4K_ALIGN equ (1 << 0)
        MULTIBOOT_FLAGS equ MULTIBOOT_FLAG_4K_ALIGN
        MULTIBOOT_MAGIC equ 0x1BADB002
        MULTIBOOT_LOADER_MAGIC equ 0x2BADB002
        MULTIBOOT_CHECKSUM equ -(MULTIBOOT_FLAGS + MULTIBOOT_MAGIC)

        section .multiboot
        align 4
        dd MULTIBOOT_MAGIC
        dd MULTIBOOT_FLAGS
        dd MULTIBOOT_CHECKSUM

;;; Multiboot does not provide a stack for us, so we have to create
;;; our own. We put it in the .bss section because this is left
;;; uninitialised in the ELF file, storing only the size of the
;;; section and not its contents. This means we can avoid putting
;;; 16KiB of empty space in the executable.
;;; The stack on x86 must be 16-byte aligned according to the System V
;;; ABI standard and de-facto extensions. Compilers will assume the
;;; stack is properly aligned, and failure to align the stack will
;;; result in undefined behavior.

        section .bss
        align 16
stack_bottom:
        resb 16384
stack_top:

        section .text
        global _start:function
_start:
        ;; Multiboot will:
        ;;  - put us into protected mode
        ;;  - keep interrupts disabled
        ;;  - set up one code and one data segment spanning all memory
        ;;    (no paging yet)

        ;; set up stack
        mov esp, stack_top

        ;; check that we were loaded by a multiboot loader
        cmp eax, MULTIBOOT_LOADER_MAGIC
        jne .hang

        ;; store the address of the Multiboot Information Struct so
        ;; that the C code can correctly access it
        extern mis
        mov [mis], ebx

        ;; jump to the C loader
        extern module_loader_main
        call module_loader_main

        ;; if we end up back here, we just need to hang the system
        .hang:
        cli
        hlt
        jmp .hang
