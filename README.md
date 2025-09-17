<div align="center">
<img src="https://raw.githubusercontent.com/rnayabed/rangoli/master/icons/rangoli.svg" height="150" alt="Rangoli">

# Rangoli / रंगोली / রঙ্গোলি

*Free, Open Source, Lightweight, Cross-platform Software for Royal Kludge Keyboards*

![version](https://img.shields.io/badge/Version-2.0-green)
![Total Downloads](https://img.shields.io/github/downloads/rnayabed/rangoli/total?label=Total%20Downloads)
[![Tip Me via PayPal](https://img.shields.io/badge/PayPal-Tip%20me-blue.svg?logo=paypal)](https://www.paypal.me/rnayabed)
[![Tip Me via Ko-Fi](https://img.shields.io/badge/Ko--Fi-Tip%20me-red.svg?logo=kofi)](https://ko-fi.com/rnayabed)

<img src="https://raw.githubusercontent.com/rnayabed/rangoli/master/screenshots/main.png" alt="Rangoli Main Screenshot">

### [Website](https://rnayabed.github.io/rangoli_website)

### [Demonstration Video](https://youtu.be/MTGICKC4G5U)

### [Screenshots](https://github.com/rnayabed/rangoli/blob/master/screenshots/README.md#screenshots)

### [Keyboards Support Status List](https://github.com/rnayabed/rangoli/blob/master/keyboards-list.md)

### [Download latest release](https://github.com/rnayabed/rangoli/releases/latest)

**If you enjoy using Rangoli, please consider [donating](https://github.com/rnayabed/rangoli#donate)**
</div>

## Purpose

[Royal Kludge Keyboards](https://rkgamingstore.com/) are very popular in the budget mechanical keyboard community. Sometimes it is the only viable option for students especially from developing countries like me. However, they are infamous for poor software support.

There are efforts to get 3rd party firmware like [QMK](https://qmk.fm/) to run on some of these keyboards, but not every keyboard is supported. Also there are keyboards with the same model name but different processors. The entire process is risky and not recommended for begineers.

I decided to try another approach to this problem. I reverse engineered the protocol these keyboards use with their default firmware. I did this by using [Wireshark](https://www.wireshark.org/) to capture and observe USB packets sent from my PC to my keyboard after editing settings in the RK Software.

Therefore, instead of changing the keyboard firmware, this software pretends to be "RK Software".It is plug and play!

## Pros over RK Software
- Free and Open Source.
- Cross-platform. Rangoli runs on Linux, Windows and MacOS.
- Start On Boot. You can configure Rangoli to start at system boot and also select a profile to be applied on startup.
- Functional System Tray Icon. Although the RK Software does show up in System Tray, it is very limited and opens up the main window for even basic tasks like selecting a profile. Rangoli offers more functionality, including the ability to apply a profile per keyboard.
- Modern. Far more user friendly and has basic window behaviours including being able to resize, maximise, etc - something which the RK Software lacks.
- Colour Picker. You can select colour of your choice directly from your screen. Hex, RGB, HSV, HSL are also supported.
- Customisable. Rangoli offers users to change it's theme colours and also offers light and dark themes.

## Supported features
- Custom Key Map
- All built-in light modes
- Per key RGB
- Per key LED Brightness, Animation and Sleep

## Planned features
- Music Mode
- Custom RGB Patterns

## Features that will not be worked on
- Macros. This is something that can be done directly on OS level. There are tools like [AutoHotkey](https://www.autohotkey.com/) and [AutoKey](https://github.com/autokey/autokey) that does this far better than RK Software itself.
- Keyboard Firmware update. I was not able to reverse engineer this aspect of the RK Software because the function itself is broken.

## Disclaimer

**Rangoli is NOT official software. It is a hobby project.**

There is no guarantee that it will work with your keyboard. There is no official documentation for the RK Software protocol, so I had to reverse engineer it based on the RK71RGB N Keyboard. Other keyboards are not guaranteed to work. I am not responsible if you brick your keyboard. 

## Compile and Run from source

### Prerequisites

- Qt >= 6.5
- CMake >= 3.16
- GCC >= 9.4.0 / Clang >= 5
- Ninja >= 1.10.2

#### Fedora Linux

Install the required packages:

```bash
# Development tools and Qt6
sudo dnf install cmake ninja-build gcc-c++ git

# Qt6 packages
sudo dnf install qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtsvg-devel

# HID development libraries (for hidapi)
sudo dnf install libudev-devel libusb1-devel

# Optional: Additional Qt6 tools
sudo dnf install qt6-qttools-devel
```

#### Other Linux Distributions

For Ubuntu/Debian:
```bash
sudo apt install build-essential cmake ninja-build git
sudo apt install qt6-base-dev qt6-declarative-dev qt6-svg-dev
sudo apt install libudev-dev libusb-1.0-0-dev
```

### Clone

Rangoli uses [hidapi](https://github.com/libusb/hidapi). It has been included as a submodule.
Therefore, you need to clone the repository with `--recurse-submodules`.

```
git clone --recurse-submodules https://github.com/rnayabed/rangoli.git
```

In case you forget to do that, you need to run the following command:

```
git submodule update --init
```

### Compile 

```
cmake -B build -G Ninja -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Run
- Linux: `./build/src/rangoli`
- Windows: `.\build\src\rangoli.exe`
- MacOS: `open ./build/src/rangoli.app`

### Linux Permissions

On Linux, Rangoli requires access to HID devices. If you get a segmentation fault when running without sudo, you need to install udev rules:

```bash
# Run the installation script
./install-udev-rules.sh

# Then unplug and replug your keyboard, or restart your system
```

For detailed instructions and troubleshooting, see:
- [Linux Permissions Guide (English)](LINUX_PERMISSIONS_EN.md)
- [Linux Berechtigungen (Deutsch)](LINUX_PERMISSIONS.md)

## Bugs and Support

If you want to report an issue or sugggest a feature, please report it by creating a [GitHub issue](https://github.com/rnayabed/rangoli/issues).

You may also contact me via any of the following platforms:
- E-Mail: [debayansutradhar3@gmail.com](mailto:debayansutradhar3@gmail.com)
- Matrix: @dubbadhar:matrix.org
- Discord: rnayabed#0784

## Donate

It takes a lot of time and effort to build quality software. Anything will be highly appreciated <3

- UPI: 9903299105@upi
- [Ko-Fi](https://ko-fi.com/rnayabed)
- [Paypal](https://paypal.me/rnayabed)

## Credits

Developed by [Debayan Sutradhar](https://github.com/rnayabed). All Rights Reserved.

Logo by [Sab GFX](https://twitter.com/Sab_gfx).

## License

Rangoli is licensed to [GNU General Public License v3.0](https://github.com/rnayabed/rangoli/blob/master/LICENSE).

```
Rangoli - Free, Open Source, Lightweight, Cross-platform Software for Royal Kludge Keyboards
Copyright (C) 2023 Debayan Sutradhar (rnayabed)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
```
