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

    @ Externe Interrupts (STM32F103x6, IRQn 0..42)
    .word   WWDG_IRQHandler              @ 0  WWDG
    .word   PVD_IRQHandler               @ 1  PVD
    .word   TAMPER_IRQHandler            @ 2  TAMPER
    .word   RTC_IRQHandler               @ 3  RTC
    .word   FLASH_IRQHandler             @ 4  FLASH
    .word   RCC_IRQHandler               @ 5  RCC
    .word   EXTI0_IRQHandler             @ 6  EXTI0
    .word   EXTI1_IRQHandler             @ 7  EXTI1
    .word   EXTI2_IRQHandler             @ 8  EXTI2
    .word   EXTI3_IRQHandler             @ 9  EXTI3
    .word   EXTI4_IRQHandler             @ 10 EXTI4
    .word   DMA1_Channel1_IRQHandler     @ 11 DMA1_Channel1
    .word   DMA1_Channel2_IRQHandler     @ 12 DMA1_Channel2
    .word   DMA1_Channel3_IRQHandler     @ 13 DMA1_Channel3
    .word   DMA1_Channel4_IRQHandler     @ 14 DMA1_Channel4
    .word   DMA1_Channel5_IRQHandler     @ 15 DMA1_Channel5
    .word   DMA1_Channel6_IRQHandler     @ 16 DMA1_Channel6
    .word   DMA1_Channel7_IRQHandler     @ 17 DMA1_Channel7
    .word   ADC1_2_IRQHandler            @ 18 ADC1_2
    .word   USB_HP_CAN1_TX_IRQHandler    @ 19 USB_HP_CAN1_TX
    .word   USB_LP_CAN1_RX0_IRQHandler   @ 20 USB_LP_CAN1_RX0
    .word   CAN1_RX1_IRQHandler          @ 21 CAN1_RX1
    .word   CAN1_SCE_IRQHandler          @ 22 CAN1_SCE
    .word   EXTI9_5_IRQHandler           @ 23 EXTI9_5
    .word   TIM1_BRK_IRQHandler          @ 24 TIM1_BRK
    .word   TIM1_UP_IRQHandler           @ 25 TIM1_UP
    .word   TIM1_TRG_COM_IRQHandler      @ 26 TIM1_TRG_COM
    .word   TIM1_CC_IRQHandler           @ 27 TIM1_CC
    .word   TIM2_IRQHandler              @ 28 TIM2
    .word   TIM3_IRQHandler              @ 29 TIM3
    .word   0                            @ 30 Reserved
    .word   I2C1_EV_IRQHandler           @ 31 I2C1_EV
    .word   I2C1_ER_IRQHandler           @ 32 I2C1_ER
    .word   0                            @ 33 Reserved
    .word   0                            @ 34 Reserved
    .word   SPI1_IRQHandler              @ 35 SPI1
    .word   0                            @ 36 Reserved
    .word   USART1_IRQHandler            @ 37 USART1
    .word   USART2_IRQHandler            @ 38 USART2
    .word   0                            @ 39 Reserved
    .word   EXTI15_10_IRQHandler         @ 40 EXTI15_10
    .word   RTCAlarm_IRQHandler          @ 41 RTC_Alarm
    .word   USBWakeUp_IRQHandler         @ 42 USBWakeUp

@ ===========================================================================
@ Reset Handler - Der Einstiegspunkt nach Reset
@ ===========================================================================
.section .text.Reset_Handler, "ax", %progbits
.global Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    @ 1. .data Sektion von Flash nach RAM kopieren
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

    @ 2. .bss Sektion auf 0 setzen
    ldr   r0, =_sbss
    ldr   r1, =_ebss
    mov   r2, #0
    b     .bss_zero_check

.bss_zero_loop:
    str   r2, [r0], #4      @ 0 in RAM schreiben
.bss_zero_check:
    cmp   r0, r1            @ Ende erreicht?
    blo   .bss_zero_loop    @ Nein, weiter nullen

    @ 3. System initialisieren
    bl    SystemInit

    @ 4. main() aufrufen
    bl    main

    @ 5. Falls main() zurueckkehrt
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

.weak   WWDG_IRQHandler
.thumb_set WWDG_IRQHandler, Default_Handler

.weak   PVD_IRQHandler
.thumb_set PVD_IRQHandler, Default_Handler

.weak   TAMPER_IRQHandler
.thumb_set TAMPER_IRQHandler, Default_Handler

.weak   RTC_IRQHandler
.thumb_set RTC_IRQHandler, Default_Handler

.weak   FLASH_IRQHandler
.thumb_set FLASH_IRQHandler, Default_Handler

.weak   RCC_IRQHandler
.thumb_set RCC_IRQHandler, Default_Handler

.weak   EXTI0_IRQHandler
.thumb_set EXTI0_IRQHandler, Default_Handler

.weak   EXTI1_IRQHandler
.thumb_set EXTI1_IRQHandler, Default_Handler

.weak   EXTI2_IRQHandler
.thumb_set EXTI2_IRQHandler, Default_Handler

.weak   EXTI3_IRQHandler
.thumb_set EXTI3_IRQHandler, Default_Handler

.weak   EXTI4_IRQHandler
.thumb_set EXTI4_IRQHandler, Default_Handler

.weak   DMA1_Channel1_IRQHandler
.thumb_set DMA1_Channel1_IRQHandler, Default_Handler

.weak   DMA1_Channel2_IRQHandler
.thumb_set DMA1_Channel2_IRQHandler, Default_Handler

.weak   DMA1_Channel3_IRQHandler
.thumb_set DMA1_Channel3_IRQHandler, Default_Handler

.weak   DMA1_Channel4_IRQHandler
.thumb_set DMA1_Channel4_IRQHandler, Default_Handler

.weak   DMA1_Channel5_IRQHandler
.thumb_set DMA1_Channel5_IRQHandler, Default_Handler

.weak   DMA1_Channel6_IRQHandler
.thumb_set DMA1_Channel6_IRQHandler, Default_Handler

.weak   DMA1_Channel7_IRQHandler
.thumb_set DMA1_Channel7_IRQHandler, Default_Handler

.weak   ADC1_2_IRQHandler
.thumb_set ADC1_2_IRQHandler, Default_Handler

.weak   USB_HP_CAN1_TX_IRQHandler
.thumb_set USB_HP_CAN1_TX_IRQHandler, Default_Handler

.weak   USB_LP_CAN1_RX0_IRQHandler
.thumb_set USB_LP_CAN1_RX0_IRQHandler, Default_Handler

.weak   CAN1_RX1_IRQHandler
.thumb_set CAN1_RX1_IRQHandler, Default_Handler

.weak   CAN1_SCE_IRQHandler
.thumb_set CAN1_SCE_IRQHandler, Default_Handler

.weak   EXTI9_5_IRQHandler
.thumb_set EXTI9_5_IRQHandler, Default_Handler

.weak   TIM1_BRK_IRQHandler
.thumb_set TIM1_BRK_IRQHandler, Default_Handler

.weak   TIM1_UP_IRQHandler
.thumb_set TIM1_UP_IRQHandler, Default_Handler

.weak   TIM1_TRG_COM_IRQHandler
.thumb_set TIM1_TRG_COM_IRQHandler, Default_Handler

.weak   TIM1_CC_IRQHandler
.thumb_set TIM1_CC_IRQHandler, Default_Handler

.weak   TIM2_IRQHandler
.thumb_set TIM2_IRQHandler, Default_Handler

.weak   TIM3_IRQHandler
.thumb_set TIM3_IRQHandler, Default_Handler

.weak   I2C1_EV_IRQHandler
.thumb_set I2C1_EV_IRQHandler, Default_Handler

.weak   I2C1_ER_IRQHandler
.thumb_set I2C1_ER_IRQHandler, Default_Handler

.weak   SPI1_IRQHandler
.thumb_set SPI1_IRQHandler, Default_Handler

.weak   USART1_IRQHandler
.thumb_set USART1_IRQHandler, Default_Handler

.weak   USART2_IRQHandler
.thumb_set USART2_IRQHandler, Default_Handler

.weak   EXTI15_10_IRQHandler
.thumb_set EXTI15_10_IRQHandler, Default_Handler

.weak   RTCAlarm_IRQHandler
.thumb_set RTCAlarm_IRQHandler, Default_Handler

.weak   USBWakeUp_IRQHandler
.thumb_set USBWakeUp_IRQHandler, Default_Handler

@ ===========================================================================
@ Ende des Startup-Codes
@ ===========================================================================
