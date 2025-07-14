;;; Code very heavily based on:
;;; https://wiki.osdev.org/Setting_Up_Long_Mode
        BITS 32

%macro call_print_str 1
        push dword %1
        call term_println
        add esp, 4
%endmacro
%macro set_fg 1
        push byte %1
        call term_set_fg
        add esp, 1
%endmacro

        section .text
        global switch_to_long:function
switch_to_long:
        extern term_println
        extern term_clear_screen
        extern yline

        call detect_cpuid
        jnz .have_cpuid
        call err_color
        call_print_str STR_NO_CPUID
        ret

        .have_cpuid:
        call_print_str STR_CPUID

        call detect_extended_functions
        jnb .ext_available
        call err_color
        call_print_str STR_NO_EXT
        ret

        .ext_available:
        call_print_str STR_EXT_AVAILABLE

        call detect_long_mode
        jnz .long_mode_available
        call err_color
        call_print_str STR_NO_LONG_MODE
        ret

        .long_mode_available:
        call_print_str STR_LONG_MODE_AVAILABLE

        ;; if we made it here, long mode is available
        ;; we set up the paging structures in the C code
        extern page_level_4_tab
        ;; enable the PAE paging flag
        mov eax, cr4
        or eax, 1 << 5
        mov cr4, eax

        ;; have the CR3 register point to the PML4T
        mov eax, page_level_4_tab
        mov cr3, eax
        call_print_str STR_PAE_ENABLED
        ;; set the long mode bit
        mov ecx, 0xC0000080           ; Set the C-register to 0xC0000080, which is the EFER MSR.
        rdmsr                         ; Read from the model-specific register.
        or eax, 1 << 8                ; Set the LM-bit which is the 9th bit (bit 8).
        wrmsr                         ; Write to the model-specific register.
        ;; enable paging
        mov eax, cr0                  ; Set the A-register to control register 0.
        or eax, 1 << 31               ; Set the PG-bit, which is the 32nd bit (bit 31).
        mov cr0, eax                  ; Set control register 0 to the A-register.
        ;; at this point, we have put the CPU into long mode and
        ;; enabled long mode paging. However, our GDT is still set up
        ;; for protected mode. The CPU is therefore running in 32-bit
        ;; compatibility mode, rather than true long mode.
        call_print_str STR_ENTERED_COMPAT
        ;; To get into true long mode, we need to load a GDT with a
        ;; long mode code segment, then make a far jump to 64-bit code
        lgdt [GDTR]
        jmp 08:long_mode

detect_cpuid:
        ;; Copy FLAGS in to EAX via stack
        pushfd
        pop eax

        ;; Copy to ECX as well for comparing later on
        mov ecx, eax

        ;; Flip the ID bit
        xor eax, 1 << 21

        ;; Copy EAX to FLAGS via the stack
        push eax
        popfd

        ;; Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
        pushfd
        pop eax

        ;; Restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit
        ;; back if it was ever flipped).
        push ecx
        popfd

        ;; Compare EAX and ECX. If they are equal then that means the bit wasn't
        ;; flipped, and CPUID isn't supported.
        xor eax, ecx
        ret

detect_extended_functions:
        mov eax, 0x80000000     ; Set the A-register to 0x80000000.
        cpuid                   ; CPU identification.
        cmp eax, 0x80000001     ; Compare the A-register with 0x80000001.
        ret

detect_long_mode:
        mov eax, 0x80000001     ; Set the A-register to 0x80000001.
        cpuid                   ; CPU identification.
        test edx, 1 << 29       ; Test if the LM-bit, which is bit 29, is set in the D-register.
        ret

err_color:
        extern term_set_fg
        set_fg byte 4
        ret

good_color:
        extern term_set_fg
        set_fg byte 10
        ret

        BITS 64
long_mode:
        ;; If the far jump succeeded without any issues, we're now
        ;; running in long mode!

        ;; First, reset all the segment registers, including the new
        ;; ones added in long mode.
        cli
        mov rax, 16
        mov ds, rax
        mov es, rax
        mov fs, rax
        mov gs, rax
        mov ss, rax

        ;; Place a red L in the top left as a test that we finally
        ;; made it to long mode. This will swiftly be overwritten by
        ;; the kernel, but it's nice to do it anyway :)
        mov [0xB8000], word 0x4F4C

        ;; Set up RAX and RBX to pass the multiboot magic number and
        ;; MIS to the kernel
        xor rax, rax
        mov eax, 0x2BADB002
        xor rbx, rbx
        extern mis
        mov ebx, dword [mis]
        ;; Retrieve the full 64-bit entrypoint pointer from the C code
        ;; and call it
        extern entry
        call [entry]

        ;; If we somehow end up back here, just hang the system
        .hang:
        cli
        hlt
        jmp .hang
        BITS 32

        section .rodata
strings:
        STR_NO_CPUID db "[-] CPUID is not available", 0
        STR_CPUID db "[+] CPUID is available", 0
        STR_EXT_AVAILABLE db "[+] Extended CPUID is available", 0
        STR_NO_EXT db "[-] Extended CPUID is not available", 0
        STR_LONG_MODE_AVAILABLE db "[+] Long mode is available", 0
        STR_NO_LONG_MODE db "[-] Long mode is not available", 0
        STR_PAE_ENABLED db "[+] PAE flag set", 0
        STR_ENTERED_COMPAT db "[+] Entered long mode, running in 32-bit compatibility mode", 0

GDTR:
        GDT_SIZE dw (GDT_END - GDT - 1)
        GDT_OFFSET dq GDT
GDT:
        NULL_ENTRY dq 0
        dw 0xFFFF
        dw 0
        db 0x0, 155, 0xAF, 0x0

        dw 0xFFFF
        dw 0
        db 0x0, 147, 0xCF, 0x0
GDT_END:
