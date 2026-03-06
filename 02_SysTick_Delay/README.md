# STM32F103C6 Bare Metal Project - SysTick Delay

Ein Bare-Metal Projekt fuer den STM32F103C6 mit einfacher SysTick-basierter Delay-Funktion in Millisekunden.
Das Projekt basiert auf `00_Blank_Project` und nutzt weiterhin den internen HSI-Takt mit 8 MHz.

## Projektstruktur

```
02_SysTick_Delay/
├── Makefile              # Build-System mit Debug/Release-Profilen
├── linker/
│   └── stm32f103c6t6.ld # Linker-Skript
├── src/
│   ├── main.c           # Hauptprogramm mit nicht-blockierender 500-ms-Logik
│   ├── gpio.c/h         # GPIO-Funktionen
│   ├── system.c         # System-Initialisierung
│   ├── systick.c/h      # Zentrale SysTick-Zeitbasis und Software-Timer-Helfer
│   ├── startup_stm32f103c6.s  # Startup-Code (ASM)
│   ├── stm32f10x.h      # Register-Definitionen
│   ├── stm32f103x6.h    # Device-spezifische Register
│   └── system_stm32f1xx.h     # System-Header
├── build/               # Build-Output (ELF, BIN, HEX, Disassembly)
├── tools/
│   ├── openocd.cfg      # OpenOCD Konfiguration
│   ├── gdbinit          # GDB Initialisierungsskript
│   └── opencd.cfg       # (Deprecated)
└── docs/                # Dokumentation

```

## Toolchain

- **Compiler**: arm-none-eabi-gcc
- **Debugger**: GDB (GNU Debugger)
- **Programmer**: OpenOCD
- **Target**: STM32F103C6T6 (Cortex-M3 @ HSI 8 MHz)

## SysTick Zeitbasis

Die Zeitlogik wird ueber ein zentrales Modul `systick.c/.h` realisiert:

- `systick_init_1ms()` startet den 1-ms-SysTick-Interrupt.
- `systick_now_ms()` liefert die laufende Systemzeit in Millisekunden.
- `systick_delay_ms(uint32_t delay_ms)` bietet bei Bedarf ein blockierendes Delay.
- `systick_period_elapsed(...)` prueft periodische Aufgaben nicht-blockierend.

Die Taktbasis bleibt `HSI = 8 MHz`, damit Debug- und Release-Builds das gleiche Zeitverhalten zeigen.

## Make-Befehle

### Build & Binary

| Befehl | Beschreibung |
|--------|-------------|
| `make all` | Kompiliert das komplette Projekt (Default) |
| `make clean` | Löscht alle Build-Artefakte |
| `make size` | Zeigt Größe und Memory-Layout des ELF-Files |

### Output-Formate

| Befehl | Beschreibung |
|--------|-------------|
| `make hex` | Generiert HEX-Datei für Flash-Programmierung |
| `make disasm` | Erzeugt Disassembly-Listing (für Analyse) |

### Programmierung & Debugging

| Befehl | Beschreibung |
|--------|-------------|
| `make flash` | Programmiert den Flash mit dem generierten Binary |
| `make erase` | Löscht den kompletten Flash |
| `make reset` | Setzt den Mikrocontroller zurück |
| `make debug` | Startet GDB-Session mit OpenOCD (Breakpoints möglich) |

### Utilities

| Befehl | Beschreibung |
|--------|-------------|
| `make check-openocd` | Prüft OpenOCD-Installation |
| `make check-gdb` | Prüft GDB-Installation |

## Build-Profile

Das Projekt unterstützt zwei Optimierungsprofile:

```bash
# Debug-Build (Default) - mit Full Debug-Info
make BUILD=debug

# Release-Build - optimierte Größe, minimale Debug-Info
make BUILD=release
```

## Schnellstart

1. **Projekt kompilieren**:
   ```bash
   make clean all
   ```

2. **Größe überprüfen**:
   ```bash
   make size
   ```

3. **Auf Flash programmieren** (benötigt OpenOCD + ST-Link):
   ```bash
   make flash
   ```

4. **Debuggen starten**:
   ```bash
   make debug
   # Im GDB-Prompt dann: continue
   ```

## Hardware-Anforderungen

- STM32F103C6T6 Entwicklungsboard
- ST-Link V2 oder ähnlicher Programmer/Debugger
- USB-Verbindung zum PC

## Wichtige Hinweise

- **Linker-Skript**: Das LD-Skript muss an die Flash/RAM-Größen angepasst sein:
  - Flash: 32 KB (0x08000000)
  - RAM: 10 KB (0x20000000)

- **Makefile**: Rezepte müssen mit **TABs** eingerückt sein, nicht mit Spaces!

- **GDB-Init**: Das `tools/gdbinit` Skript wird automatisch geladen für:
  - Automatisches OpenOCD-Target-Setup
  - Symbol-Laden
  - Breakpoint-Handling

## Fehlerbehebung

### OpenOCD verbindet nicht
```bash
# Manuell testen:
openocd -f tools/openocd.cfg
```

### GDB findet Symbols nicht
```bash
# Im GDB-Prompt:
(gdb) file build/firmware.elf
(gdb) symbol-file build/firmware.elf
```

### Flash-Programmierung schlägt fehl
- ST-Link korrekt angesteckt?
- OpenOCD läuft im Hintergrund? (wird von `make flash` gestartet)
- Firewall blockt OpenOCD?
