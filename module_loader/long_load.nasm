        BITS 32

%macro call_print_str 3
        push dword %3
        push dword %2
        push dword %1
        call print_string
        add esp, 4*3
%endmacro

        section .text
        global switch_to_long:function
switch_to_long:
        ;; Code very heavily based on:
        ;; https://wiki.osdev.org/Setting_Up_Long_Mode
        extern print_string
        extern clear_screen
        extern yline

        call detect_cpuid
        jnz .have_cpuid
        call err_color
        call_print_str STR_NO_CPUID, 0, [yline]
        ret

        .have_cpuid:
        call_print_str STR_CPUID, 0, [yline]
        inc dword [yline]

        call detect_extended_functions
        jnb .ext_available
        call err_color
        call_print_str STR_NO_EXT, 0, [yline]
        ret

        .ext_available:
        call_print_str STR_EXT_AVAILABLE, 0, [yline]
        inc dword [yline]

        call detect_long_mode
        jnz .long_mode_available
        call err_color
        call_print_str STR_NO_LONG_MODE, 0, [yline]
        ret

        .long_mode_available:
        call_print_str STR_LONG_MODE_AVAILABLE, 0, [yline]
        inc dword [yline]

        ret

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
        extern terminal_color
        mov [terminal_color], byte 4
        ret

good_color:
        extern terminal_color
        mov [terminal_color], byte 10
        ret

        section .rodata
        STR_NO_CPUID db "[-] CPUID is not available", 0
        STR_CPUID db "[+] CPUID is available", 0
        STR_EXT_AVAILABLE db "[+] Extended CPUID is available", 0
        STR_NO_EXT db "[-] Extended CPUID is not available", 0
        STR_LONG_MODE_AVAILABLE db "[+] Long mode is available", 0
        STR_NO_LONG_MODE db "[-] Long mode is not available", 0
