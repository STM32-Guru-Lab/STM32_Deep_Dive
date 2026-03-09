#!/bin/bash
# Einfaches Skript zum Anzeigen des GPIOA ODR Registers live
# Drücke den Button an PA1 und sieh wie sich die Bits ändern

set -e

echo "=== GPIOA ODR Register Live-Ansicht ==="
echo "Button an PA1 drücken und Bits beobachten"
echo ""

# Prüfe ob OpenOCD läuft
if ! nc -z localhost 3333 2>/dev/null; then
    echo "OpenOCD läuft nicht auf Port 3333."
    echo "Starten mit: openocd -f tools/openocd.cfg &"
    echo "Möchtest du OpenOCD jetzt starten? (j/n)"
    read -r answer
    if [[ "$answer" =~ ^[JjYy] ]]; then
        echo "Starte OpenOCD im Hintergrund..."
        openocd -f tools/openocd.cfg > /tmp/openocd.log 2>&1 &
        OPENOCD_PID=$!
        echo "OpenOCD PID: $OPENOCD_PID"
        sleep 2
    else
        echo "Bitte starte OpenOCD manuell und führe dieses Skript erneut aus."
        exit 1
    fi
else
    echo "✓ OpenOCD läuft auf Port 3333"
    OPENOCD_PID=""
fi

echo ""
echo "Starte GDB zum Auslesen des GPIOA->ODR Registers..."
echo "Drücke Strg+C um zu stoppen"
echo ""

# Erstelle temporäres GDB Skript
TEMP_GDB=$(mktemp)
cat > "$TEMP_GDB" << 'EOF'
# Verbinde zu OpenOCD
target extended-remote :3333

# Lade Firmware
file build/firmware.elf
load

# Starte Programm
continue

# Definiere Funktion zum Anzeigen des ODR Registers
define show_odr
  printf "\n=== GPIOA ODR Register ===\n"
  printf "Hex: 0x%04X\n", GPIOA->ODR
  printf "Bin: "
  # Zeige 16 Bits, MSB zuerst
  for (int i = 15; i >= 0; i--)
    printf "%d", (GPIOA->ODR >> i) & 1
  end
  printf "\n"
  
  # Erkläre wichtige Bits für dieses Projekt
  printf "\nBit-Zuordnung:\n"
  printf "  PA0 (Bit 0):  Rohsignal-Ausgang (Taster gespiegelt)\n"
  printf "  PA1 (Bit 1):  Taster-Eingang (0=gedrückt, 1=nicht gedrückt)\n"
  printf "  PA2 (Bit 2):  Entprelltes Signal\n"
  printf "  PA3 (Bit 3):  Prellen-Indikator\n"
  printf "  PA13 (Bit 13): (JTAG/SWD, normalerweise HIGH)\n"
  printf "  PA14 (Bit 14): (JTAG/SWD, normalerweise HIGH)\n"
  printf "  PA15 (Bit 15): (JTAG/SWD, normalerweise HIGH)\n"
  printf "=======================\n"
end

# Hauptschleife
echo "Live-Überwachung gestartet. Alle 200ms Update.\n"
echo "Taster an PA1 drücken und Bits 0,1,2,3 beobachten!\n"

while 1
  show_odr
  # Warte 200ms
  shell sleep 0.2
end
EOF

# Starte GDB
gdb-multiarch -q -x "$TEMP_GDB" 2>/dev/null || true

# Aufräumen
rm -f "$TEMP_GDB"

# OpenOCD beenden wenn wir es gestartet haben
if [ -n "$OPENOCD_PID" ]; then
    echo ""
    echo "Beende OpenOCD (PID: $OPENOCD_PID)..."
    kill $OPENOCD_PID 2>/dev/null || true
fi

echo ""
echo "Fertig."