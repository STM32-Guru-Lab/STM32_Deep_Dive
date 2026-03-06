# STM32F103C6 Bare Metal Project - Taster

Ein Bare-Metal Projekt fuer den STM32F103C6 mit einfacher Tasterabfrage per Polling.
Das Projekt basiert auf `02_SysTick_Delay` und nutzt weiterhin den internen HSI-Takt mit 8 MHz.

## Projektstruktur

```
03_Taster/
├── Makefile              # Build-System mit Debug/Release-Profilen
├── linker/
│   └── stm32f103c6t6.ld # Linker-Skript
├── src/
│   ├── main.c           # Hauptprogramm mit einfacher Polling-Abfrage
│   ├── gpio.c/h         # GPIO-Funktionen
│   ├── system.c         # System-Initialisierung
│   ├── systick.c/h      # Zentrale SysTick-Zeitbasis und Software-Timer-Helfer
│   ├── startup_stm32f103c6.s  # Startup-Code (ASM)
│   └── lokale Projektquellen
├── build/               # Build-Output (ELF, BIN, HEX, Disassembly)
├── tools/
│   ├── openocd.cfg      # OpenOCD Konfiguration
│   ├── gdbinit          # GDB Initialisierungsskript
│   └── opencd.cfg       # (Deprecated)
└── docs/                # Dokumentation

Gemeinsame Vendor-Dateien:
vendor/
└── STM32F1036t6/
    └── CMSIS/
        ├── Core/Include/
        └── Device/ST/STM32F1xx/Include/

```

## Toolchain

- **Compiler**: arm-none-eabi-gcc
- **Debugger**: GDB (GNU Debugger)
- **Programmer**: OpenOCD
- **Target**: STM32F103C6T6 (Cortex-M3 @ HSI 8 MHz)

## Taster-Logik

Die Anwendung nutzt einen Taster an `PA1` (gegen GND):

- Der Eingang wird als `GPIO_CNF_INPUT_PULL` konfiguriert.
- Der interne Pull-up erzeugt den Ruhezustand `HIGH`.
- Ein gueltiger Tastendruck ist `LOW`.
- Die LED (`PC13`) und der Debug-Pin (`PA0`) folgen direkt dem Tasterzustand.
- Die Abfrage erfolgt in einer einfachen Polling-Schleife.

## Vendor-Basis

Ab diesem Projekt werden die Vendor-Dateien gemeinsam genutzt:

- `../vendor/STM32F1036t6/CMSIS/Core/Include`
- `../vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include`

Die Projekte `00_Blank_Project`, `01_Clock_Test` und `02_SysTick_Delay` bleiben bewusst standalone.

## SysTick Zeitbasis

Die Zeitbasis wird ueber `systick.c/.h` bereitgestellt:

- `systick_init_1ms()` startet den 1-ms-SysTick-Interrupt.
- `systick_now_ms()` liefert die laufende Systemzeit in Millisekunden.
- `systick_period_elapsed(...)` prueft periodische Aufgaben nicht-blockierend.

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
