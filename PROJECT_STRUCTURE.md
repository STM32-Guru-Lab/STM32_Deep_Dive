# Projektstruktur

## Standalone-Basis

Diese Projekte bleiben bewusst standalone (jeweils eigene Vendor-Dateien):

- `00_Blank_Project`
- `01_Clock_Test`
- `02_SysTick_Delay`

## Gemeinsame Vendor-Dateien ab Projekt 03

Ab `03_Taster` werden CMSIS-Dateien zentral genutzt:

- `vendor/STM32F1036t6/CMSIS/Core/Include`
- `vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include`

Vorteile:

- weniger Duplikate im Repository
- einheitlicher Header-Stand fuer Folgeprojekte
- einfachere Wartung bei Updates
