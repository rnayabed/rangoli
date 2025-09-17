#!/bin/bash

# Rangoli Keyboard Mapper - udev Rules Installation Script
# This script installs udev rules to allow non-root access to Rangoli keyboards

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
UDEV_RULES_FILE="$SCRIPT_DIR/udev/99-rangoli-keyboards.rules"
UDEV_RULES_DEST="/etc/udev/rules.d/99-rangoli-keyboards.rules"

echo "Rangoli Keyboard Mapper - udev Rules Installation"
echo "=================================================="

# Check if running as root
if [[ $EUID -eq 0 ]]; then
    echo "Error: This script should NOT be run as root."
    echo "It will automatically ask for sudo permission when needed."
    exit 1
fi

# Check if udev rules file exists
if [[ ! -f "$UDEV_RULES_FILE" ]]; then
    echo "Error: udev rules file not found: $UDEV_RULES_FILE"
    exit 1
fi

echo "Installing udev rules for Rangoli keyboards..."

# Copy udev rules file
sudo cp "$UDEV_RULES_FILE" "$UDEV_RULES_DEST"
echo "✓ udev rules copied to $UDEV_RULES_DEST"

# Set correct permissions
sudo chmod 644 "$UDEV_RULES_DEST"
echo "✓ Permissions set"

# Reload udev rules
sudo udevadm control --reload-rules
echo "✓ udev rules reloaded"

# Trigger udev
sudo udevadm trigger
echo "✓ udev trigger executed"

# Add user to appropriate group for remote access support
# Different distributions use different groups for HID access
HID_GROUP=""
if getent group plugdev > /dev/null 2>&1; then
    HID_GROUP="plugdev"
elif getent group input > /dev/null 2>&1; then
    HID_GROUP="input"
fi

if [ -n "$HID_GROUP" ]; then
    if ! groups "$USER" | grep -q "$HID_GROUP"; then
        echo
        echo "Adding user '$USER' to '$HID_GROUP' group for SSH access..."
        sudo usermod -aG "$HID_GROUP" "$USER"
        echo "✓ User added to $HID_GROUP group"
        echo
        echo "IMPORTANT: You must log out and log back in (or restart your system)"
        echo "           for the group membership to take effect."
    else
        echo "✓ User is already in the $HID_GROUP group"
    fi
else
    echo "⚠ Warning: No suitable HID group (plugdev/input) found."
    echo "  SSH access may not work."
fi

echo
echo "Installation completed!"
echo
echo "Next steps:"
echo "1. Unplug and replug your Rangoli keyboard"
echo "2. Or restart your system"
echo "3. Rangoli should now work without sudo"
echo
echo "If you connect via SSH, make sure you have logged out and back in"
echo "so that the group membership takes effect."