# STM32F103C6 Bare Metal Project - Tasterentprellung

Ein Bare-Metal Projekt für den STM32F103C6 mit Software-Entprellung (Debouncing) für mechanische Taster.
Das Projekt basiert auf `03_Taster` und demonstriert nicht-blockierende Entprellungslogik mit Zustandsmaschine.

## Projektstruktur

```
04_Entprellen/
├── Makefile              # Build-System mit Debug/Release-Profilen
├── linker/
│   └── stm32f103c6t6.ld # Linker-Skript
├── src/
│   ├── main.c           # Hauptprogramm mit Entprellungs-Demonstration
│   ├── gpio.c/h         # GPIO-Funktionen
│   ├── system.c         # System-Initialisierung
│   ├── systick.c/h      # Zentrale SysTick-Zeitbasis (1ms)
│   ├── debounce.c/h     # Entprellungs-Zustandsmaschine (30ms Filter)
│   ├── startup_stm32f103c6.s  # Startup-Code (ASM)
│   └── lokale Projektquellen
├── build/               # Build-Output (ELF, BIN, HEX, Disassembly)
├── tools/
│   ├── openocd.cfg      # OpenOCD Konfiguration
│   ├── gdbinit          # GDB Initialisierungsskript
│   └── opencd.cfg       # (Deprecated)
└── docs/                # Dokumentation (Prellen-Messungen)

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

## Hardware-Übersicht

| Pin | Funktion | Beschreibung |
|-----|----------|--------------|
| **PA1** | Taster-Eingang | Mechanischer Taster gegen GND, intern Pull-up |
| **PC13** | Haupt-LED | Active-low LED (Blue Pill) |
| **PA0** | Rohsignal-Ausgang | Spiegelt PA1 1:1 (für Messungen mit LA/Oszilloskop) |
| **PA2** | Entprelltes Signal | Gefiltertes Signal nach 30ms Entprellung |
| **PA3** | Prellen-Indikator | Blinkt bei jedem erkannten Prellen-Event |

## Entprellungslogik

### Problem: Mechanisches Prellen
- Mechanische Taster "prellen" beim Drücken/Loslassen (~20ms)
- Mehrere schnelle 0/1-Wechsel (elektrisches Rauschen)
- Software muss diese Prellen filtern

### Lösung: Software-Entprellung
- **Zustandsmaschine** mit 4 Zuständen: IDLE, DEBOUNCE_PRESS, PRESSED, DEBOUNCE_RELEASE
- **Entprellzeit**: 30ms (konfigurierbar in `debounce.h`)
- **Nicht-blockierend**: Nutzt SysTick-Zeitbasis (1ms)
- **Prellen-Zähler**: Zählt erkannte Prellen-Ereignisse für Statistik

### Vergleich Rohsignal vs. Entprelltes Signal
- **PA0**: Zeigt Rohsignal (direkt von PA1 gespiegelt)
- **PA2**: Zeigt entprelltes Signal (nach 30ms Filter)
- **Messung**: Auf Oszilloskop/LA beide Signale vergleichen

## Lernziele

1. **Mechanisches Prellen verstehen**: Warum Taster 5-50ms "prellen"
2. **Software-Entprellung implementieren**: Nicht-blockierende Zustandsmaschine
3. **SysTick nutzen**: Präzise Zeitmessung für Entprellungs-Timeout
4. **Vergleich Rohsignal vs. gefiltert**: PA0 vs. PA2 auf Oszilloskop
5. **Prellen-Statistik**: Zählen und visualisieren von Prellen-Ereignissen

## Vendor-Basis

Ab diesem Projekt werden die Vendor-Dateien gemeinsam genutzt:

- `../vendor/STM32F1036t6/CMSIS/Core/Include`
- `../vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include`

Die Projekte `00_Blank_Project`, `01_Clock_Test` und `02_SysTick_Delay` bleiben bewusst standalone.

## SysTick Zeitbasis

Die Zeitbasis wird über `systick.c/.h` bereitgestellt:

- `systick_init_1ms()` startet den 1-ms-SysTick-Interrupt.
- `systick_now_ms()` liefert die laufende Systemzeit in Millisekunden.
- `systick_delay_ms()` bietet bei Bedarf blockierendes Delay.
- `systick_period_elapsed()` prüft periodische Aufgaben nicht-blockierend.

## Entprellungs-API (`debounce.h`)

```c
/* Initialisierung */
void debounce_init(void);

/* Regelmäßig aufrufen (z.B. alle 1ms) */
void debounce_update(uint8_t raw_state);

/* Entprellten Zustand abfragen */
bool debounce_get_state(void);

/* Prellen-Zähler abfragen */
uint16_t debounce_get_bounce_count(void);

/* Zustandsmaschine abfragen (Debugging) */
ButtonState debounce_get_machine_state(void);
```

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
| `make doctor` | Prüft Build-/Debug-Umgebung (Compiler, GDB, OpenOCD, Vendor-Header) |
| `make check-openocd` | Prüft OpenOCD-Installation |
| `make check-gdb` | Prüft GDB-Installation |

## Setup-Hinweise

- Linux-Setup: `../SETUP_LINUX.md`
- Windows-Setup via WSL2: `../SETUP_WINDOWS_WSL.md`

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

## Messungen durchführen

### 1. Prellen messen (PA0)
- Logic Analyzer / Oszilloskop an **PA0** anschließen
- Taster drücken und Prellen-Dauer messen
- Typisch: 5-50ms (projektgemessen: ~20ms)

### 2. Entprellung verifizieren (PA2)
- Zweiter Kanal an **PA2** anschließen
- Vergleich: PA0 (roh) vs. PA2 (gefiltert)
- Entprelltes Signal sollte 30ms nach letztem Prellen-Event stabil sein

### 3. Prellen-Indikator beobachten (PA3)
- LED oder dritter Kanal an **PA3**
- Blinkt bei jedem erkannten Prellen-Event (100ms)
- Zählt Prellen-Ereignisse visuell

## Hardware-Anforderungen

- STM32F103C6T6 Entwicklungsboard (Blue Pill)
- ST-Link V2 oder ähnlicher Programmer/Debugger
- USB-Verbindung zum PC
- **Optional**: Logic Analyzer / Oszilloskop für Messungen

## Experimente

### 1. Entprellzeit anpassen
```c
// In debounce.h
#define DEBOUNCE_TIME_MS 10U  // Test mit 10ms
#define DEBOUNCE_TIME_MS 50U  // Test mit 50ms
```

### 2. Prellen-Zähler auslesen
- Im Debugger `debounce_get_bounce_count()` überwachen
- Bei jedem Tastendruck sollte Zähler um mehrere Einheiten steigen

### 3. Zustandsmaschine beobachten
- Debugger: `debounce_get_machine_state()` überwachen
- Zustände: 0=IDLE, 1=DEBOUNCE_PRESS, 2=PRESSED, 3=DEBOUNCE_RELEASE

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

### PA2/PA3 zeigen kein Signal
- Sind PA2 und PA3 auf Blue Pill verfügbar? (prüfe PCB-Layout)
- Alternative Pins in `main.c` anpassen

## Nächstes Projekt: 05_Interrupts

Nach diesem Projekt empfiehlt sich:
- **05_Interrupts**: EXTI-Interrupts für Taster (energieeffizient)
- Kombination: Entprellungslogik aus diesem Projekt + Interrupt-Architektur