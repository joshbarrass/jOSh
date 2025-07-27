/* Source: https://wiki.osdev.org/Bare_Bones#Verifying_Multiboot
   Accessed: 17 Apr 2025 */
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_entry)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
        . = 0xfffffe8000200000;
        __kernel_start = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
                *(.eh_frame)
                *(.rodata*)
                /* Pad out the section to the next page boundary */
                FILL(0x00);
                . = ALIGN(4K) - 1;
                BYTE(0x00);
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
                /* Pad out the section to the next page boundary */
                FILL(0x00);
                . = ALIGN(4K) - 1;
                BYTE(0x00);
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
                /* Create stack in .bss directly.
                   This allows us to access the stack directly from
                   ASM, which we have to use to avoid symbol
                   relocation errors.*/
                . = ALIGN(16);
                stack = .;
                . += 16K;
                stack_top = .;

		*(COMMON)
		*(.bss)
                FILL(0x00);
                . = ALIGN(4K);
	}

        __kernel_end = .;

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
