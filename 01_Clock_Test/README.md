# STM32F103C6 Bare Metal Blank Project

Ein minimal konfiguriertes Bare-Metal Projekt für den STM32F103C6 Mikrocontroller. Dieses Projekt dient als Grundlage für STM32-Entwicklung ohne externe HAL-Bibliotheken.

## Projektstruktur

```
00_Blank_Project/
├── Makefile              # Build-System mit Debug/Release-Profilen
├── linker/
│   └── stm32f103c6t6.ld # Linker-Skript
├── src/
│   ├── main.c           # Hauptprogramm
│   ├── gpio.c/h         # GPIO-Funktionen
│   ├── system.c         # System-Initialisierung
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
- **Target**: STM32F103C6T6 (Cortex-M3 @ 72 MHz)

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
