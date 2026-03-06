# Setup Linux

Diese Anleitung richtet sich an Linux-Nutzer (Ubuntu/Debian aehnlich).

## 1) Abhaengigkeiten installieren

```bash
sudo apt update
sudo apt install -y build-essential make openocd gdb-multiarch wget tar
```

## 2) ARM Toolchain 12.3.1 installieren

Das Makefile erwartet standardmaessig:

- `/opt/gcc-arm-12.3.rel1/bin/arm-none-eabi-gcc`

Direkter Download-Link (12.3.rel1):

- `https://developer.arm.com/-/media/files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz?rev=dccb66bb394240a98b87f0f24e70e87d&revision=dccb66bb-3942-40a9-8b87-f0f24e70e87d&hash=82F2CF5F30A9A32E8CF04BBF2C562FED`

Uebersichtsseite fuer Arm GNU Toolchain Downloads:

- `https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads`

Beispiel:

```bash
cd /tmp
wget https://developer.arm.com/-/media/Files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz
sudo mkdir -p /opt/gcc-arm-12.3.rel1
sudo tar -xf arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz --strip-components=1 -C /opt/gcc-arm-12.3.rel1
```

## 3) Umgebung pruefen

Im Projektordner, z. B. `03_Taster`:

```bash
make doctor
```

Der Check prueft Toolchain, GDB, OpenOCD und die gemeinsamen Vendor-Header.
Falls Toolchain-GDB unter Linux wegen `libncursesw.so.5` nicht startet:

```bash
make doctor GDB=gdb-multiarch
```

## 4) Build

```bash
make clean all
```

## 5) Flash

```bash
make flash
```
