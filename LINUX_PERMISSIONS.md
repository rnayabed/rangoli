# Linux Permissions for Rangoli Keyboard Mapper

## Problems and Solutions

### Problem 1: Segmentation Fault due to HID Permissions

On Linux, Rangoli may crash with a "Segmentation fault (core dumped)" when running without sudo while trying to access HID devices (keyboards). This happens because regular users don't have direct access to HID devices by default.

### Problem 2: Qt6 Wayland Menu Crash

On some Linux systems running Wayland, Rangoli may crash due to a Qt6 bug in menu creation. The program automatically detects this and falls back to X11 backend.

## Solution

The solution is to install udev rules that grant your user access to Rangoli keyboards without requiring sudo.

### Automatic Installation (Recommended)

1. **Run the installation script:**
   ```bash
   ./install-udev-rules.sh
   ```

2. **Unplug and replug your keyboard** or restart your system.

3. **Rangoli should now work without sudo and automatically handle Wayland compatibility.**

### Manual Installation

If you prefer to install the udev rules manually:

1. **Copy the udev rules:**
   ```bash
   sudo cp udev/99-rangoli-keyboards.rules /etc/udev/rules.d/
   sudo chmod 644 /etc/udev/rules.d/99-rangoli-keyboards.rules
   ```

2. **Reload udev rules:**
   ```bash
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```

3. **Add your user to the appropriate group (for SSH access):**
   ```bash
   # Ubuntu/Debian:
   sudo usermod -aG plugdev $USER
   
   # Fedora/RHEL/CentOS:
   sudo usermod -aG input $USER
   ```

4. **Log out and log back in** (or restart your system).

5. **Unplug and replug your keyboard.**

## Supported Devices

The udev rules support all Rangoli keyboards with vendor ID `258a`. This includes all currently supported models.

## Troubleshooting

### Rangoli still crashes

1. **Check if udev rules are installed:**
   ```bash
   ls -la /etc/udev/rules.d/99-rangoli-keyboards.rules
   ```

2. **Check your group membership:**
   ```bash
   groups $USER
   ```
   You should see `plugdev` (Ubuntu/Debian) or `input` (Fedora/RHEL) in the list.

3. **Check if your keyboard is detected:**
   ```bash
   lsusb | grep 258a
   ```

4. **Restart your system** instead of just unplugging the keyboard.

### For SSH/Remote Access

If you want to use Rangoli over SSH:

1. **Make sure you're in the appropriate group:**
   ```bash
   # Ubuntu/Debian:
   groups $USER | grep plugdev
   
   # Fedora/RHEL:
   groups $USER | grep input
   ```

2. **If not, add yourself:**
   ```bash
   # Ubuntu/Debian:
   sudo usermod -aG plugdev $USER
   
   # Fedora/RHEL:
   sudo usermod -aG input $USER
   ```

3. **Log out and log back in:**
   ```bash
   exit  # End SSH session
   # Then reconnect via SSH
   ```

### Debugging

To check if the udev rules are working:

1. **Monitor udev events:**
   ```bash
   sudo udevadm monitor --property
   ```

2. **Unplug and replug your keyboard** and observe the output.

3. **Check device properties:**
   ```bash
   # Find your device
   lsusb | grep 258a
   
   # Check udev properties (replace X:Y with Bus:Device)
   udevadm info -a -p $(udevadm info -q path -n /dev/bus/usb/X/Y)
   ```

## Technical Details

### What the udev rules do

The [`udev/99-rangoli-keyboards.rules`](udev/99-rangoli-keyboards.rules) file contains rules that:

1. **Identify all HID devices with vendor ID 258a**
2. **Add TAG+="uaccess"** - Grants access for physically present users
3. **Set GROUP="plugdev"/"input", MODE="660"** - Enables access for group members (important for SSH)

### Why 99-rangoli-keyboards.rules?

- **99-** ensures our rules run after standard system rules
- **rangoli-keyboards** makes the file's purpose clear
- **.rules** is the required extension for udev rules

### Security Considerations

This solution:
- ✅ Only grants access to Rangoli keyboards (vendor ID 258a)
- ✅ Requires physical access or plugdev group membership
- ✅ Follows Linux security standards
- ✅ Avoids the need to run as root

## Alternative Solutions

### Temporary Solution (not recommended)

You can temporarily run Rangoli with sudo:
```bash
sudo ./build/src/rangoli
```

**Disadvantages:**
- Security risk (program runs as root)
- Inconvenient (requires sudo every time)
- Can cause file permission issues

### System-wide Permission (not recommended)

You could grant all users access to all HID devices, but this is a security risk.

## Support

If you continue to have problems:

1. **Check system logs:**
   ```bash
   journalctl -f
   ```

2. **Run Rangoli in debug mode** (if available)

3. **Create an issue** with the following information:
   - Linux distribution and version
   - Output of `lsusb | grep 258a`
   - Output of `groups $USER`
   - Contents of `/etc/udev/rules.d/99-rangoli-keyboards.rules`
   - Relevant system logs