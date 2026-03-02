/* 
 * Linker Script für STM32F103C6T6A
 * 32KB Flash (0x08000000 - 0x08007FFF)
 * 10KB RAM   (0x20000000 - 0x200027FF)
 */

MEMORY {
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 10K
}

ENTRY(Reset_Handler)

/* Stack-Größe */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS {
    /* Interrupt Vector Table am Anfang von Flash */
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))  /* Nicht wegoptimieren! */
        . = ALIGN(4);
    } >FLASH

    /* Programmcode und Konstanten */
    .text : {
        . = ALIGN(4);
        *(.text)           /* Code */
        *(.text*)          /* Mehr Code */
        *(.rodata)         /* Konstanten */
        *(.rodata*)        /* Mehr Konstanten */
        . = ALIGN(4);
    } >FLASH

    /* Initialisierte globale/static Variablen 
     * Wird von Startup aus Flash in RAM kopiert */
    _sidata = LOADADDR(.data);  /* Startadresse der Initialwerte in Flash */

    .data : {
        . = ALIGN(4);
        _sdata = .;        /* Startadresse im RAM */
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;        /* Endadresse im RAM */
    } >RAM AT>FLASH

    /* Nicht-initialisierte globale/static Variablen (auf 0 gesetzt) */
    .bss : {
        . = ALIGN(4);
        _sbss = .;         /* Startadresse im RAM */
        *(.bss)
        *(.bss*)
        *(COMMON)          /* Uninitialisierte globale Variablen */
        . = ALIGN(4);
        _ebss = .;         /* Endadresse im RAM */
    } >RAM

    /* Stack-Bereich (optional, für Debugging) */
    .stack : {
        . = ALIGN(8);
        . = . + 1024;      /* 1KB Stack */
        . = ALIGN(8);
    } >RAM

    /* Debug-Informationen entfernen (reduziert Größe) */
    /DISCARD/ : {
        *(.note*)
        *(.comment)
        *(.ARM.attributes)
    }
}
