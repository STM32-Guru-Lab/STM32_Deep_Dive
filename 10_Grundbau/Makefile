# ============================================================================
# STM32 Grundbau - Makefile
# Projekt: 10_Grundbau - Saubere Basis für zukünftige Projekte
# Autor: Pavel
# Ziel: Modulare Treiber-Struktur mit Runtime Clock-Konfiguration
# ============================================================================

# ============================================================================
# Toolchain-Pfade
# ============================================================================
ARM_TOOLCHAIN := /opt/gcc-arm-12.3.rel1
CC            := $(ARM_TOOLCHAIN)/bin/arm-none-eabi-gcc
OBJCOPY       := $(ARM_TOOLCHAIN)/bin/arm-none-eabi-objcopy
OBJDUMP       := $(ARM_TOOLCHAIN)/bin/arm-none-eabi-objdump
SIZE          := $(ARM_TOOLCHAIN)/bin/arm-none-eabi-size
GDB           ?= $(ARM_TOOLCHAIN)/bin/arm-none-eabi-gdb
OPENOCD       ?= /usr/bin/openocd
OPENOCD_CFG   := tools/openocd.cfg
GDBINIT       ?= tools/gdbinit

# ============================================================================
# Toolchain-Version pruefen
# ============================================================================
GCC_VERSION := $(shell $(CC) --version | head -n1 | sed 's/.* \([0-9]\+\.[0-9]\+\.[0-9]\+\) .*/\1/')
ifneq ($(GCC_VERSION),12.3.1)
$(error Wrong GCC version. Expected 12.3.1, got $(GCC_VERSION))
endif

# ============================================================================
# Projekt-Konfiguration
# ============================================================================
PROJECT     := firmware
BUILD_DIR   := build
OBJ_DIR     := $(BUILD_DIR)/obj
MAP_FILE    := $(BUILD_DIR)/$(PROJECT).map
TARGET      := STM32F103C6
BUILD       ?= debug

ifeq ($(BUILD),debug)
OPT_CFLAGS  := -Og -g3
OPT_ASFLAGS := -g
else ifeq ($(BUILD),release)
OPT_CFLAGS  := -Os -g0 -DNDEBUG
OPT_ASFLAGS := -g0
else
$(error Invalid BUILD='$(BUILD)'. Use BUILD=debug or BUILD=release)
endif

# ============================================================================
# Compiler Flags
# ============================================================================
CFLAGS  := -mcpu=cortex-m3 -mthumb $(OPT_CFLAGS)
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -DSTM32F103x6
CFLAGS += -I./src
CFLAGS += -I./src/drivers/system
CFLAGS += -I./src/drivers/gpio
CFLAGS += -I./src/drivers/interrupt
CFLAGS += -I./src/drivers/systick
CFLAGS += -I./src/util
CFLAGS += -I../vendor/STM32F1036t6/CMSIS/Core/Include
CFLAGS += -I../vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-common -std=c11
CFLAGS += -MMD -MP

# Assembler Flags
ASFLAGS := -mcpu=cortex-m3 -mthumb $(OPT_ASFLAGS)
ASFLAGS += -MMD -MP

# ============================================================================
# Linker Flags
# ============================================================================
LDFLAGS := -mcpu=cortex-m3 -mthumb
LDFLAGS += -Tlinker/stm32f103c6t6.ld
LDFLAGS += -Wl,--gc-sections -Wl,--print-memory-usage
LDFLAGS += -Wl,-Map=$(MAP_FILE),--cref
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs --specs=nosys.specs

# ============================================================================
# Quell-Dateien (modulare Struktur)
# ============================================================================
SRC_C := src/main.c \
         src/drivers/system/system.c \
         src/drivers/system/clock.c \
         src/drivers/gpio/gpio.c \
         src/util/delay.c

SRC_S := src/startup_stm32f103c6.s

# Objekt-Dateien (im Build-Verzeichnis)
OBJ_C := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC_C))
OBJ_S := $(patsubst src/%.s,$(OBJ_DIR)/%.o,$(SRC_S))
OBJS  := $(OBJ_C) $(OBJ_S)
DEPS  := $(OBJS:.o=.d)

# ============================================================================
# Haupt-Targets
# ============================================================================
all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).disasm
	@echo "\n=== Größeninformation ==="
	@$(SIZE) $(BUILD_DIR)/$(PROJECT).elf

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS) linker/stm32f103c6t6.ld | $(BUILD_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	@echo "✓ ELF erstellt: $@"
	@echo "✓ Memory-Map erstellt: $(MAP_FILE)"

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
	@echo "✓ Binary erstellt: $@"

$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@
	@echo "✓ HEX erstellt: $@"

$(BUILD_DIR)/$(PROJECT).disasm: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJDUMP) -d $< > $@
	@echo "✓ Disassembly erstellt: $@"

# ============================================================================
# Compile-Regeln (TAB fuer Rezepte)
# ============================================================================
# C zu Objekt (mit Unterverzeichnissen)
$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assembler zu Objekt
$(OBJ_DIR)/%.o: src/%.s | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

# Verzeichnisse erstellen
$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

# ============================================================================
# Flash- und Debug-Targets
# ============================================================================
check-openocd:
	@test -x "$(OPENOCD)" || command -v "$(OPENOCD)" >/dev/null 2>&1 || { \
		echo "Error: OpenOCD nicht gefunden: $(OPENOCD)"; \
		echo "Installiere OpenOCD (z.B. apt install openocd)"; \
		echo "oder setze den Pfad explizit: make OPENOCD=/pfad/zu/openocd flash"; \
		exit 127; \
	}
	@test -f $(OPENOCD_CFG) || { \
		echo "Error: OpenOCD Config fehlt: $(OPENOCD_CFG)"; \
		exit 1; \
	}

check-gdb:
	@$(GDB) --version >/dev/null 2>&1 || { \
		echo "Error: GDB nicht startbar: $(GDB)"; \
		echo "Hinweis: Dein Toolchain-GDB braucht oft libncursesw.so.5."; \
		echo "Option A: fehlende Bibliothek installieren"; \
		echo "Option B: make GDB=gdb-multiarch debug"; \
		exit 127; \
	}
	@test -f $(GDBINIT) || { \
		echo "Error: GDB Init fehlt: $(GDBINIT)"; \
		exit 1; \
	}

check-vendor:
	@test -f ../vendor/STM32F1036t6/CMSIS/Core/Include/core_cm3.h || { \
		echo "Error: CMSIS Core Header fehlt: ../vendor/STM32F1036t6/CMSIS/Core/Include/core_cm3.h"; \
		exit 1; \
	}
	@test -f ../vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include/stm32f103x6.h || { \
		echo "Error: Device Header fehlt: ../vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include/stm32f103x6.h"; \
		exit 1; \
	}

doctor: check-openocd check-gdb check-vendor
	@echo "=== Environment Check ==="
	@echo "Compiler: $(CC)"
	@$(CC) --version | head -n1
	@echo "GDB: $(GDB)"
	@$(GDB) --version | head -n1
	@echo "OpenOCD: $(OPENOCD)"
	@$(OPENOCD) --version | head -n1
	@echo "OpenOCD config: $(OPENOCD_CFG)"
	@echo "GDB init: $(GDBINIT)"
	@echo "Vendor Core Include: ../vendor/STM32F1036t6/CMSIS/Core/Include"
	@echo "Vendor Device Include: ../vendor/STM32F1036t6/CMSIS/Device/ST/STM32F1xx/Include"
	@echo "Doctor check finished successfully."

# ============================================================================
# Device Information Targets
# ============================================================================

# Prüft ob Device verbunden ist (ohne Fehler bei Nicht-Verbindung)
check-device-connection:
	@if $(OPENOCD) -f $(OPENOCD_CFG) \
	    -c "init; halt; echo 'connected'; shutdown" \
	    2>/dev/null | grep -q "connected"; then \
	    echo "✓ Device connected"; \
	    exit 0; \
	else \
	    echo "✗ Device not connected or wrong target"; \
	    exit 1; \
	fi

# Einfache Device-Verbindungsprüfung
device-check: check-openocd
	@echo "=== Device Connection Check ==="
	@make --no-print-directory check-device-connection || true

flash: check-openocd $(BUILD_DIR)/$(PROJECT).bin
	$(OPENOCD) -f $(OPENOCD_CFG) \
	    -c "init; halt; program $(BUILD_DIR)/$(PROJECT).bin 0x08000000 verify; reset run; shutdown"

erase: check-openocd
	$(OPENOCD) -f $(OPENOCD_CFG) \
	    -c "init; halt; stm32f1x mass_erase 0; reset run; shutdown"

reset: check-openocd
	$(OPENOCD) -f $(OPENOCD_CFG) \
	    -c "init; reset run; shutdown"

debug: check-gdb check-openocd $(BUILD_DIR)/$(PROJECT).elf
	@echo "Starte OpenOCD (Hintergrund) und GDB..."
	@set -e; \
	if command -v ss >/dev/null 2>&1 && ss -lnt | grep -qE 'LISTEN.+:3333([[:space:]]|$$)'; then \
		echo "Hinweis: OpenOCD läuft bereits, nutze bestehenden Server."; \
		$(GDB) -x $(GDBINIT) $(BUILD_DIR)/$(PROJECT).elf; \
	else \
		$(OPENOCD) -f $(OPENOCD_CFG) >/tmp/openocd-$(PROJECT).log 2>&1 & \
		OPENOCD_PID=$$!; \
		trap 'kill $$OPENOCD_PID >/dev/null 2>&1 || true' EXIT INT TERM; \
		sleep 0.5; \
		$(GDB) -x $(GDBINIT) $(BUILD_DIR)/$(PROJECT).elf; \
	fi

# ============================================================================
# Utility Targets
# ============================================================================
clean:
	rm -rf $(BUILD_DIR)

size: $(BUILD_DIR)/$(PROJECT).elf
	@$(SIZE) $<

disasm: $(BUILD_DIR)/$(PROJECT).disasm
	@echo "Disassembly in $(BUILD_DIR)/$(PROJECT).disasm"

hex: $(BUILD_DIR)/$(PROJECT).hex
	@echo "HEX in $(BUILD_DIR)/$(PROJECT).hex"

# ============================================================================
# Help Target
# ============================================================================
help:
	@echo "STM32 Grundbau Makefile - Available Targets:"
	@echo ""
	@echo "  Build Targets:"
	@echo "    all          - Build everything (ELF, binary, disasm)"
	@echo "    clean        - Remove build directory"
	@echo "    size         - Show size of firmware"
	@echo "    disasm       - Generate disassembly"
	@echo "    hex          - Generate HEX file"
	@echo ""
	@echo "  Flash & Debug Targets:"
	@echo "    flash        - Flash firmware to device"
	@echo "    erase        - Mass erase flash"
	@echo "    reset        - Reset device"
	@echo "    debug        - Start GDB debugging session"
	@echo ""
	@echo "  Device Information Targets:"
	@echo "    device-check - Check if device is connected"
	@echo ""
	@echo "  Environment Targets:"
	@echo "    doctor       - Check toolchain and environment"
	@echo ""
	@echo "  Examples:"
	@echo "    make all BUILD=release    - Build optimized release"
	@echo "    make device-check         - Test connection"
	@echo "    make doctor               - Check environment"

# ============================================================================
# Meta-Targets
# ============================================================================
.PHONY: all flash erase reset debug clean size disasm hex doctor check-openocd check-gdb check-vendor device-check check-device-connection help

# Auto-Dependencies für C/ASM-Dateien
-include $(DEPS)
