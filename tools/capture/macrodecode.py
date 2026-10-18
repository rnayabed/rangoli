# Decodes the macro area written in a USBPcap capture of the official RK software.
#
#   python3 -I macrodecode.py capture.pcapng
#
# Prints every non-empty macro slot as press (dn) / release (up) events with the
# delay after each event in keyboard units (about 6.94 ms each on the RK96).
# See docs/rk96/PROTOCOL.md for the format.
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import usbpcap

SLOT_COUNT, SLOT_BYTES, AREA_HEADER, AREA_PACKETS = 10, 192, 2, 31

# HID keyboard usages (page 0x07) for readable output
NAMES = {0x04 + i: chr(ord('a') + i) for i in range(26)}
NAMES.update({0x1e + i: str((i + 1) % 10) for i in range(10)})
NAMES.update({0x28: 'enter', 0x29: 'esc', 0x2a: 'backspace', 0x2b: 'tab', 0x2c: 'space',
              0x2d: '-', 0x2e: '=', 0x2f: '[', 0x30: ']', 0x31: '\\', 0x33: ';', 0x34: "'",
              0x35: '`', 0x36: ',', 0x37: '.', 0x38: '/'})
NAMES.update({0x3a + i: f'f{i + 1}' for i in range(12)})
MODIFIERS = {0x01: 'ctrl', 0x02: 'shift', 0x04: 'alt', 0x08: 'win',
             0x10: 'rctrl', 0x20: 'rshift', 0x40: 'ralt', 0x80: 'rwin'}

pages = {}
for n, ts, pkt in usbpcap.packets(sys.argv[1]):
    h, p = usbpcap.usbpcap(pkt)
    # SET_REPORT control transfers: 8 byte setup packet, then 0a 1f <page> + 62 bytes
    if h['xfer'] == 2 and len(p) > 11 and p[0] == 0x21 and p[1] == 0x09 and p[9] == AREA_PACKETS:
        pages[p[10]] = p[11:73]  # the last write of a page wins

if len(pages) != AREA_PACKETS:
    sys.exit(f'Found {len(pages)} of {AREA_PACKETS} macro area pages, '
             'was the capture running while Apply was clicked?')

area = b''.join(pages[i] for i in range(1, AREA_PACKETS + 1))
print('header', area[:AREA_HEADER].hex())

for slot in range(SLOT_COUNT):
    data = area[AREA_HEADER + slot * SLOT_BYTES:AREA_HEADER + (slot + 1) * SLOT_BYTES]
    if not any(data):
        continue

    events = []
    for i in range(0, SLOT_BYTES, 3):
        b0, b1, code = data[i:i + 3]
        if not (b0 or b1 or code):
            break

        delay = (b0 & 0x7f) | ((b1 & 0x0f) << 7)
        kind = b1 >> 4
        if kind == 0xf:
            name = MODIFIERS.get(code, hex(code))
        else:
            name = NAMES.get(code, hex(code))
            if kind:
                name += f' (unknown flag {kind:#x})'
        events.append(f"{'up' if b0 & 0x80 else 'dn'} {name} +{delay}")

    print(f'slot {slot + 1}:', ', '.join(events))
