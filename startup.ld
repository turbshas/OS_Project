OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    SRAM0 (rwx)         : ORIGIN = 0x20000000, LENGTH = 128K
#ifdef __STM32F4xx__
    CCMRAM0 (rwx)       : ORIGIN = 0x10000000, LENGTH = 64K
#endif
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 1M
}

SECTIONS
{
    . = 0x0; /* Start at 0x00000000 */

    .text :
    {
        KEEP(*(ISR_VECTORS))  /* vector table */
        *(.start)
        *(.text)         /* program code */
        *(.text*)
        *(.eh_frame)
        *(.init)
        *(.fini)
    } >FLASH

    .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >FLASH
    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    /* Read-only data can stay in the flash (no need to copy to RAM) */
    .rodata :
    {
        *(.rodata)
        *(.rodata*)
    } >FLASH
    _RODATA_SIZE = SIZEOF(.rodata);

    /* Location of .data section in binary file */
    _DATA_ROM_START = .;

#ifdef __STM32F4xx__
    /* This section is probably wrong, but I'm not using it right now anyway */
    /* Location of .ccmram section in binary file */
    _CCM_ROM_START = ADDR(_DATA_ROM_START) + SIZEOF(.data);

    . = 0x10000000;
    .ccmram : AT(_CCM_ROM_START)
    {
        _CCM_RAM_START = .;
        *(.ccmram)
        *(.ccmram*)
        _CCM_RAM_END = .;
    } >CCMRAM0
    _CCMRAM_SIZE = SIZEOF(.ccmram);
#endif

    . = 0x20000000;

    /* Section for data in RAM (file scope variables)
     * Loaded into the flash, then copied from the flash to RAM during boot
     */
    .data :
    {
        _DATA_RAM_START = .;
        *(.data)
        _DATA_RAM_END  = .;
    } >SRAM0 AT> FLASH
    _DATA_SIZE = SIZEOF(.data);

    /* Zero-initialized data
     * Loaded into the flash, then initialized in RAM during boot
     */
    .bss :
    {
        _BSS_START = .;
        __bss_start__ = .;
        *(.bss)
        __bss_end__ = .;
        _BSS_END = .;
    } >SRAM0 AT> FLASH
    _BSS_SIZE = SIZEOF(.bss);

    _ALLOCABLE_MEM = .;

    /* Regular RAM section */
    .heap :
    {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } >SRAM0 AT> FLASH
    _HEAP_SIZE = SIZEOF(.heap);

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}

