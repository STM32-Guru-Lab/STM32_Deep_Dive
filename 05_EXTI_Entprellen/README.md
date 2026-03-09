# STM32F103C6 Bare Metal Project - EXTI-basierte Tasterentprellung

**Projekt 05: Erweiterung von Projekt 04 mit External Interrupts (EXTI)**

Ein Bare-Metal Projekt für den STM32F103C6 mit EXTI-basierter Software-Entprellung (Debouncing) für mechanische Taster.
Das Projekt demonstriert den Übergang von Polling (Projekt 04) zu Interrupt-gesteuerter Abfrage mit Energieeffizienz.

## Projektstruktur

```
05_EXTI_Entprellen/
├── Makefile              # Build-System mit Debug/Release-Profilen
├── linker/
│   └── stm32f103c6t6.ld # Linker-Skript
├── src/
│   ├── main.c           # Hauptprogramm mit EXTI- und WFI-Logik
│   ├── gpio.c/h         # GPIO-Funktionen
│   ├── system.c         # System-Initialisierung (mit AFIO Clock)
│   ├── systick.c/h      # Zentrale SysTick-Zeitbasis (1ms)
│   ├── debounce.c/h     # Entprellungs-Zustandsmaschine (30ms Filter)
│   ├── exti.c/h         # EXTI-Konfiguration für Taster PA1
│   ├── nvic.c/h         # NVIC-Konfiguration für Interrupt-Prioritäten
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

## Vergleich: Projekt 04 (Polling) vs. Projekt 05 (EXTI)

| Aspekt | Projekt 04 (Polling) | Projekt 05 (EXTI) |
|--------|---------------------|-------------------|
| **Taster-Abfrage** | Polling in Main-Loop | EXTI-Interrupt bei Flankenänderung |
| **CPU-Auslastung** | 100% (busy loop) | <1% (WFI zwischen Interrupts) |
| **Entprellung** | Software in Main-Loop | Teilweise in ISR, teilweise in Main-Loop |
| **Latenz** | Bis zu 1ms (Polling-Zyklus) | Sofort (Interrupt) |
| **Energieverbrauch** | Hoch | Sehr niedrig (CPU in Sleep-Mode) |
| **Code-Komplexität** | Einfach | Mittel (Interrupt-Handling, ISR-sichere Variablen) |
| **Reaktionszeit** | Abhängig von Polling-Frequenz | Sofort bei Flankenänderung |
| **Skalierbarkeit** | Schlecht (mehr Taster = höhere CPU-Last) | Gut (mehr Taster = mehr Interrupts) |

## Hardware-Übersicht

| Pin | Funktion | Beschreibung |
|-----|----------|--------------|
| **PA1** | Taster-Eingang | Mechanischer Taster gegen GND, intern Pull-up → **EXTI_Line1** |
| **PC13** | Haupt-LED | Active-low LED (Blue Pill) |
| **PA0** | Rohsignal-Ausgang | Spiegelt PA1 1:1 (für Messungen mit LA/Oszilloskop) |
| **PA2** | Entprelltes Signal | Gefiltertes Signal nach 30ms Entprellung |
| **PA3** | Prellen-Indikator | Blinkt bei jedem erkannten Prellen-Event |

## EXTI-Architektur

### 1. **EXTI-Konfiguration (PA1 → EXTI_Line1)**
```c
// In main.c
exti_init((uint32_t)GPIOA, 1, EXTI_TRIGGER_BOTH);
nvic_enable_irq(EXTI1_IRQn, NVIC_PRIORITY_MEDIUM);
exti_enable_interrupt();
nvic_enable_global_interrupts();
```

### 2. **Interrupt Service Routine (ISR)**
```c
void EXTI1_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR1) {
        last_exti_time = systick_now_ms();
        exti_triggered = 1;  // Flag für Hauptprogramm
        EXTI->PR = EXTI_PR_PR1;  // Flag löschen
    }
}
```

### 3. **WFI (Wait For Interrupt)**
```c
// In Hauptschleife
while(1) {
    if (exti_triggered) {
        // Verarbeitung...
    }
    __WFI();  // CPU schlafen legen bis nächster Interrupt
}
```

## Lernziele (Erweiterung zu Projekt 04)

### Neue Lernziele in Projekt 05:
1. **EXTI (External Interrupt) verstehen**: GPIO-Pins auf Interrupts mappen
2. **NVIC konfigurieren**: Interrupt-Prioritäten und -Aktivierung
3. **ISR-sichere Programmierung**: volatile Variablen für Interrupt-Kommunikation
4. **Energieeffizienz durch WFI**: CPU in Sleep-Mode zwischen Interrupts
5. **Vergleich Polling vs. Interrupt**: Praxisnaher Vergleich der Architekturen

### Fortgeführte Lernziele aus Projekt 04:
6. **Software-Entprellung**: Gleiche Zustandsmaschine, aber Interrupt-getriggert
7. **SysTick Zeitbasis**: Weiterhin 1ms Basis für Entprellungs-Timeout
8. **Debug-Pins**: PA0, PA2, PA3 für Messungen

## Entprellungslogik (wie Projekt 04)

### Gleiche Implementierung, andere Trigger:
- **Zustandsmaschine** mit 4 Zuständen
- **Entprellzeit**: 30ms (konfigurierbar in `debounce.h`)
- **Trigger**: EXTI-Interrupt statt Polling
- **Prellen-Zähler**: Weiterhin aktiv für Statistik

### Vergleich Rohsignal vs. Entprelltes Signal:
- **PA0**: Rohsignal (direkt von PA1 gespiegelt)
- **PA2**: Entprelltes Signal (nach 30ms Filter)
- **Messung**: Auf Oszilloskop/LA beide Signale vergleichen

## Vendor-Basis

Ab diesem Projekt werden die Vendor-Dateien gemeinsam genutzt:

- `../vendor/STM32F1036t6/CMSIS/Core/Include`
- `../vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include`

## Neue API-Komponenten

### EXTI-API (`exti.h`)
```c
bool exti_init(uint32_t port, uint16_t pin, ExtiTrigger trigger);
void exti_enable_interrupt(void);
void exti_disable_interrupt(void);
void exti_clear_pending(void);
bool exti_is_pending(void);
void EXTI1_IRQHandler(void);
```

### NVIC-API (`nvic.h`)
```c
void nvic_enable_irq(IrqNumber irq_num, NvicPriority priority);
void nvic_disable_irq(IrqNumber irq_num);
void nvic_clear_pending(IrqNumber irq_num);
void nvic_enable_global_interrupts(void);
void nvic_disable_global_interrupts(void);
```

## Make-Befehle (wie Projekt 04)

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

### 1. Interrupt-Latenz messen
- Logic Analyzer an **PA0** (Rohsignal) anschließen
- Taster drücken und Latenz bis ISR-Aufruf messen
- Typisch: < 1µs bei Cortex-M3

### 2. Energieverbrauch vergleichen
- Strommessung zwischen Projekt 04 und 05
- Projekt 05 sollte deutlich weniger Strom verbrauchen (WFI)

### 3. Prellen-Indikator beobachten (PA3)
- LED oder dritter Kanal an **PA3**
- Blinkt bei jedem erkannten Prellen-Event (100ms)
- Zählt Prellen-Ereignisse visuell

## Hardware-Anforderungen

- STM32F103C6T6 Entwicklungsboard (Blue Pill)
- ST-Link V2 oder ähnlicher Programmer/Debugger
- USB-Verbindung zum PC
- **Optional**: Logic Analyzer / Oszilloskop für Messungen
- **Optional**: Strommessgerät für Energieverbrauchs-Vergleich

## Experimente

### 1. EXTI-Trigger-Typ ändern
```c
// In main.c, exti_setup()
exti_init((uint32_t)GPIOA, 1, EXTI_TRIGGER_RISING);   // Nur steigende Flanke
exti_init((uint32_t)GPIOA, 1, EXTI_TRIGGER_FALLING);  // Nur fallende Flanke
```

### 2. WFI entfernen
```c
// In main.c, Hauptschleife
// __WFI(); // Kommentieren und Polling-Verhalten beobachten
```

### 3. Interrupt-Priorität ändern
```c
// In main.c, exti_setup()
nvic_enable_irq(EXTI1_IRQn, NVIC_PRIORITY_HIGHEST);   // Höchste Priorität
nvic_enable_irq(EXTI1_IRQn, NVIC_PRIORITY_LOWEST);    // Niedrigste Priorität
```

### 4. Entprellzeit anpassen
```c
// In debounce.h
#define DEBOUNCE_TIME_MS 10U  // Test mit 10ms
#define DEBOUNCE_TIME_MS 50U  // Test mit 50ms
```

## Wichtige Hinweise

### AFIO Clock aktivieren
- EXTI benötigt AFIO (Alternate Function I/O) Clock
- In `system.c` wurde `RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;` hinzugefügt

### ISR-sichere Variablen
- Kommunikation zwischen ISR und Hauptprogramm über `volatile` Variablen
- `exti_triggered` und `last_exti_time` sind `volatile`

### Interrupt-Flags löschen
- In ISR muss EXTI-Flag gelöscht werden: `EXTI->PR = EXTI_PR_PR1;`
- Sonst Endlos-Interrupts

### Startup-Code
- `startup_stm32f103c6.s` enthält Vector-Table mit Weak-Symbol für `EXTI1_IRQHandler`
- Unsere Implementierung in `main.c` überschreibt das Weak-Symbol

## Debugging-Tipps

### 1. Interrupts im Debugger
```bash
make debug
(gdb) break EXTI1_IRQHandler
(gdb) continue
```

### 2. EXTI-Register anzeigen
```bash
# Im GDB
(gdb) monitor mdw 0x40010400 10  # EXTI Basis-Address
```

### 3. NVIC-Register anzeigen
```bash
# Im GDB
(gdb) monitor mdw 0xE000E100 10  # NVIC ISER0
```

### 4. Prellen-Zähler überwachen
```bash
# Im GDB
(gdb) watch debounce_get_bounce_count()
```

## Fehlerbehebung

### Keine EXTI-Interrupts
1. **AFIO Clock aktiviert?** (siehe `system.c`)
2. **EXTI korrekt konfiguriert?** (PA1 → EXTI_Line1)
3. **NVIC Interrupt aktiviert?** (`nvic_enable_irq()`)
4. **Globale Interrupts aktiviert?** (`nvic_enable_global_interrupts()`)
5. **Interrupt-Flag gelöscht?** (in ISR: `EXTI->PR = EXTI_PR_PR1;`)

### CPU bleibt nicht in WFI
1. **Andere Interrupts aktiv?** (SysTick, etc.)
2. **Debugger hält CPU an?** (Breakpoints, Single-Step)
3. **EXTI-Flag nicht gelöscht?** → Endlos-Interrupts

### Größenvergleich zu Projekt 04
```bash
# Projekt 04 Größe
cd ../04_Entprellen && make size

# Projekt 05 Größe
cd ../05_EXTI_Entprellen && make size
```
- Projekt 05 sollte minimal größer sein (EXTI/NVIC Code)
- Typisch: +100-200 Bytes Flash

## Nächstes Projekt: 06_USART

Nach diesem Projekt empfiehlt sich:
- **06_USART**: Serielle Kommunikation über UART
- Kombination: EXTI für Taster + USART für Debug-Ausgabe
- Energieeffizientes System mit seriellem Monitoring

## Lizenz und Credits

- **Autor**: Pavel
- **Projekt**: STM32-Guru Deep Dive Serie
- **Basis**: Projekt 04_Entprellen
- **Lizenz**: Für Bildungszwecke, Anpassung erlaubt

## Changelog (vs. Projekt 04)

### Neue Dateien:
- `src/exti.c/h`: EXTI-Konfiguration und Interrupt-Handling
- `src/nvic.c/h`: NVIC-Funktionen für Interrupt-Prioritäten

### Geänderte Dateien:
- `src/main.c`: EXTI statt Polling, WFI für Energieeffizienz
- `src/system.c`: AFIO Clock aktiviert für EXTI
- `Makefile`: Neue Source-Dateien hinzugefügt
- `README.md`: Vollständig überarbeitet für Projekt 05

### Gleiche Dateien (unverändert):
- `src/debounce.c/h`: Gleiche Entprellungslogik
- `src/gpio.c/h`: Gleiche GPIO-Funktionen
- `src/systick.c/h`: Gleiche Zeitbasis
- `linker/stm32f103c6t6.ld`: Gleiches Linker-Skript
- `tools/`: Gleiche Debug-Tools