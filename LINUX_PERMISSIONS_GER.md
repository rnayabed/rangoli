# Linux-Berechtigungen für Rangoli Keyboard Mapper

## Probleme und Lösungen

### Problem 1: Speicherzugriffsfehler durch HID-Berechtigungen

Unter Linux kann Rangoli ohne sudo mit einem "Speicherzugriffsfehler" abstürzen, wenn es versucht, auf HID-Geräte (Tastaturen) zuzugreifen. Dies liegt daran, dass normale Benutzer standardmäßig keinen direkten Zugriff auf HID-Geräte haben.

### Problem 2: Qt6 Wayland Menü-Absturz

Auf einigen Linux-Systemen mit Wayland kann Rangoli aufgrund eines Qt6-Bugs bei der Menü-Erstellung abstürzen. Das Programm erkennt dies automatisch und wechselt zum X11-Backend.

## Lösung

Die Lösung besteht darin, udev-Regeln zu installieren, die Ihrem Benutzer Zugriff auf Rangoli-Tastaturen gewähren, ohne sudo zu benötigen.

### Automatische Installation (Empfohlen)

1. **Führen Sie das Installationsscript aus:**
   ```bash
   ./install-udev-rules.sh
   ```

2. **Stecken Sie Ihre Tastatur ab und wieder an** oder starten Sie das System neu.

3. **Rangoli sollte jetzt ohne sudo funktionieren und automatisch Wayland-Kompatibilität handhaben.**

### Manuelle Installation

Falls Sie die udev-Regeln manuell installieren möchten:

1. **Kopieren Sie die udev-Regeln:**
   ```bash
   sudo cp udev/99-rangoli-keyboards.rules /etc/udev/rules.d/
   sudo chmod 644 /etc/udev/rules.d/99-rangoli-keyboards.rules
   ```

2. **Laden Sie die udev-Regeln neu:**
   ```bash
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```

3. **Fügen Sie Ihren Benutzer zur entsprechenden Gruppe hinzu (für SSH-Zugriff):**
   ```bash
   # Ubuntu/Debian:
   sudo usermod -aG plugdev $USER
   
   # Fedora/RHEL/CentOS:
   sudo usermod -aG input $USER
   ```

4. **Melden Sie sich ab und wieder an** (oder starten Sie das System neu).

5. **Stecken Sie Ihre Tastatur ab und wieder an.**

## Unterstützte Geräte

Die udev-Regeln unterstützen alle Rangoli-Tastaturen mit der Vendor ID `258a`. Dies umfasst alle derzeit unterstützten Modelle.

## Fehlerbehebung

### Rangoli stürzt immer noch ab

1. **Überprüfen Sie, ob die udev-Regeln installiert sind:**
   ```bash
   ls -la /etc/udev/rules.d/99-rangoli-keyboards.rules
   ```

2. **Überprüfen Sie Ihre Gruppenmitgliedschaft:**
   ```bash
   groups $USER
   ```
   Sie sollten `plugdev` (Ubuntu/Debian) oder `input` (Fedora/RHEL) in der Liste sehen.

3. **Überprüfen Sie, ob Ihre Tastatur erkannt wird:**
   ```bash
   lsusb | grep 258a
   ```

4. **Starten Sie das System neu** anstatt nur die Tastatur abzustecken.

### Für SSH/Remote-Zugriff

Wenn Sie Rangoli über SSH verwenden möchten:

1. **Stellen Sie sicher, dass Sie in der entsprechenden Gruppe sind:**
   ```bash
   # Ubuntu/Debian:
   groups $USER | grep plugdev
   
   # Fedora/RHEL:
   groups $USER | grep input
   ```

2. **Falls nicht, fügen Sie sich hinzu:**
   ```bash
   # Ubuntu/Debian:
   sudo usermod -aG plugdev $USER
   
   # Fedora/RHEL:
   sudo usermod -aG input $USER
   ```

3. **Melden Sie sich ab und wieder an:**
   ```bash
   exit  # SSH-Sitzung beenden
   # Dann erneut per SSH verbinden
   ```

### Debugging

Um zu überprüfen, ob die udev-Regeln funktionieren:

1. **Überwachen Sie udev-Ereignisse:**
   ```bash
   sudo udevadm monitor --property
   ```

2. **Stecken Sie Ihre Tastatur ab und wieder an** und beobachten Sie die Ausgabe.

3. **Überprüfen Sie die Geräteeigenschaften:**
   ```bash
   # Finden Sie Ihr Gerät
   lsusb | grep 258a
   
   # Überprüfen Sie die udev-Eigenschaften (ersetzen Sie X:Y mit Bus:Device)
   udevadm info -a -p $(udevadm info -q path -n /dev/bus/usb/X/Y)
   ```

## Technische Details

### Was die udev-Regeln tun

Die [`udev/99-rangoli-keyboards.rules`](udev/99-rangoli-keyboards.rules) Datei enthält Regeln, die:

1. **Alle HID-Geräte mit Vendor ID 258a identifizieren**
2. **TAG+="uaccess" hinzufügen** - Gewährt Zugriff für physisch anwesende Benutzer
3. **GROUP="plugdev"/"input", MODE="660" setzen** - Ermöglicht Zugriff für Gruppenmitglieder (wichtig für SSH)

### Warum 99-rangoli-keyboards.rules?

- **99-** stellt sicher, dass unsere Regeln nach den Standard-Systemregeln ausgeführt werden
- **rangoli-keyboards** macht den Zweck der Datei klar
- **.rules** ist die erforderliche Erweiterung für udev-Regeln

### Sicherheitsüberlegungen

Diese Lösung:
- ✅ Gewährt nur Zugriff auf Rangoli-Tastaturen (Vendor ID 258a)
- ✅ Erfordert physischen Zugang oder plugdev-Gruppenmitgliedschaft
- ✅ Folgt Linux-Sicherheitsstandards
- ✅ Vermeidet die Notwendigkeit, als root zu laufen

## Alternative Lösungen

### Temporäre Lösung (nicht empfohlen)

Sie können Rangoli temporär mit sudo ausführen:
```bash
sudo ./build/src/rangoli
```

**Nachteile:**
- Sicherheitsrisiko (Programm läuft als root)
- Unbequem (sudo bei jedem Start erforderlich)
- Kann zu Dateiberechtigungsproblemen führen

### Systemweite Berechtigung (nicht empfohlen)

Sie könnten allen Benutzern Zugriff auf alle HID-Geräte gewähren, aber das ist ein Sicherheitsrisiko.

## Support

Falls Sie weiterhin Probleme haben:

1. **Überprüfen Sie die Systemlogs:**
   ```bash
   journalctl -f
   ```

2. **Führen Sie Rangoli im Debug-Modus aus** (falls verfügbar)

3. **Erstellen Sie ein Issue** mit folgenden Informationen:
   - Linux-Distribution und Version
   - Ausgabe von `lsusb | grep 258a`
   - Ausgabe von `groups $USER`
   - Inhalt von `/etc/udev/rules.d/99-rangoli-keyboards.rules`
   - Relevante Systemlogs