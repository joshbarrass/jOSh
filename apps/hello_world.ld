ENTRY(main)

SECTIONS
{
        . = 0xfffffe8000200000; /* PML4T[509] + 1GB */

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
		*(COMMON)
		*(.bss)
                FILL(0x00);
                . = ALIGN(4K);
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
