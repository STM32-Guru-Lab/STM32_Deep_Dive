# Setup Windows mit WSL2

Diese Anleitung ist fuer Windows-Nutzer im Kurs.  
Empfohlener Weg: Build in WSL2, optionales Flashen per OpenOCD oder STM32CubeProgrammer.

## 1) WSL2 + Ubuntu installieren

PowerShell als Administrator:

```powershell
wsl --install -d Ubuntu
```

Nach dem Neustart Ubuntu starten und Benutzer anlegen.

## 2) Tools in Ubuntu installieren

```bash
sudo apt update
sudo apt install -y build-essential make openocd gdb-multiarch wget tar
```

## 3) ARM Toolchain 12.3.1 installieren

Das Makefile erwartet standardmaessig:

- `/opt/gcc-arm-12.3.rel1/bin/arm-none-eabi-gcc`

Beispiel:

```bash
cd /tmp
wget https://developer.arm.com/-/media/Files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz
sudo mkdir -p /opt/gcc-arm-12.3.rel1
sudo tar -xf arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz --strip-components=1 -C /opt/gcc-arm-12.3.rel1
```

## 4) Repo in WSL bearbeiten

Empfehlung: Projekt direkt im Linux-Dateisystem halten, z. B.:

```bash
mkdir -p ~/repos
cd ~/repos
git clone <REPO_URL>
```

Windows-Zugriff auf WSL-Dateien:

- `\\wsl$\Ubuntu\home\<user>\repos\...`

## 5) Umgebung pruefen

Im Projektordner, z. B. `03_Taster`:

```bash
make doctor
```

Falls Toolchain-GDB in WSL nicht startet:

```bash
make doctor GDB=gdb-multiarch
```

## 6) Build und Flash

```bash
make clean all
make flash
```

Falls OpenOCD in WSL den ST-Link nicht direkt erreicht:

- in WSL nur bauen
- unter Windows `build/firmware.bin` mit STM32CubeProgrammer flashen
