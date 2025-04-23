/* Source: https://wiki.osdev.org/Bare_Bones#Verifying_Multiboot
   Accessed: 17 Apr 2025 */
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(kernel_main)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* The module loader is loaded at a 2M offset. However, x86_64
	generally requires 2MB aligment, so to make sure there is no
	overlap between the module loader and the kernel, we ensure
	that the kernel is loaded to an offset of 4M */
        . = 4M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
