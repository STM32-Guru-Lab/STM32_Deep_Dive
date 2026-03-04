@ ===========================================================================
@ Startup Code für STM32F103C6T6A (Cortex-M3)
@ ARM Assembler Implementierung
@ ===========================================================================

.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

@ ===========================================================================
@ Interrupt Vector Table
@ ===========================================================================
.section .isr_vector, "a", %progbits
.align 2
.global _isr_vectors

_isr_vectors:
    .word   _estack        @ Stack Pointer Initialwert
    .word   Reset_Handler  @ Reset Handler
    .word   NMI_Handler    @ NMI Handler
    .word   HardFault_Handler  @ Hard Fault Handler
    .word   MemManage_Handler  @ MPU Fault Handler
    .word   BusFault_Handler   @ Bus Fault Handler
    .word   UsageFault_Handler @ Usage Fault Handler
    .word   0               @ Reserved
    .word   0               @ Reserved
    .word   0               @ Reserved
    .word   0               @ Reserved
    .word   SVC_Handler     @ SVCall Handler
    .word   DebugMon_Handler @ Debug Monitor Handler
    .word   0               @ Reserved
    .word   PendSV_Handler  @ PendSV Handler
    .word   SysTick_Handler @ SysTick Handler

    @ Externe Interrupts
    .rept 68
    .word   Default_Handler
    .endr

@ ===========================================================================
@ Reset Handler - Der Einstiegspunkt nach Reset
@ ===========================================================================
.section .text.Reset_Handler, "ax", %progbits
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    @ 1. Stack Pointer setzen
    ldr   r0, =_estack
    mov   sp, r0

    @ 2. .data Sektion von Flash nach RAM kopieren
    @    (_sidata = Quelle in Flash, _sdata = Ziel in RAM, _edata = Ende)
    ldr   r0, =_sidata
    ldr   r1, =_sdata
    ldr   r2, =_edata
    b     .data_copy_check

.data_copy_loop:
    ldr   r3, [r0], #4      @ 4 Bytes aus Flash lesen
    str   r3, [r1], #4      @ In RAM schreiben
.data_copy_check:
    cmp   r1, r2            @ Ende erreicht?
    blo   .data_copy_loop   @ Nein, weiter kopieren

    @ 3. .bss Sektion auf 0 setzen
    ldr   r0, =_sbss
    ldr   r1, =_ebss
    mov   r2, #0
    b     .bss_zero_check

.bss_zero_loop:
    str   r2, [r0], #4      @ 0 in RAM schreiben
.bss_zero_check:
    cmp   r0, r1            @ Ende erreicht?
    blo   .bss_zero_loop    @ Nein, weiter nullen

    @ 4. System initialisieren (Clock, Flash, etc.)
    bl    SystemInit

    @ 5. main() aufrufen
    bl    main

    @ 6. Falls main() zurückkehrt
.Infinite_Loop:
    b     .Infinite_Loop

.size Reset_Handler, .-Reset_Handler

@ ===========================================================================
@ Default Handler für nicht implementierte Interrupts
@ ===========================================================================
.section .text.Default_Handler, "ax", %progbits
.weak Default_Handler
.type Default_Handler, %function

Default_Handler:
    b     Default_Handler   @ In Endlosschleife, falls unerwarteter Interrupt

.size Default_Handler, .-Default_Handler

@ ===========================================================================
@ Weak Aliases für alle Interrupt-Handler
@ Handler zeigen initial auf Default_Handler
@ Handler koennen in C ueberschrieben werden (z.B. SysTick_Handler)
@ ===========================================================================
.weak   NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak   HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak   MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler

.weak   BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler

.weak   UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler

.weak   SVC_Handler
.thumb_set SVC_Handler, Default_Handler

.weak   DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler

.weak   PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler

.weak   SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler

@ ===========================================================================
@ Ende des Startup-Codes
@ ===========================================================================
