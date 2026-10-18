# How the captures were made

Everything new in this branch's protocol support was learned the same way upstream
Rangoli was built (see the main README's "Purpose" section). The keyboard's owner
recorded the USB traffic between the official RK software and the keyboard on
Windows, then compared captures of known actions. This document describes that
process so it can be repeated for other models.

## Is this legal?

*This is general information, not legal advice.*

- **No part of the official software is used, copied or shipped.** Rangoli contains
  no RK code, binaries, images or data files. Only the protocol's facts (which bytes
  mean what) are documented, and those were learned by watching data sent over USB
  to a keyboard the owner bought.
- Observing how a program you are licensed to use talks to your own hardware, in
  order to write a compatible program, is a long-established practice. Many
  jurisdictions explicitly protect it as reverse engineering for interoperability:
  - United States: 17 U.S.C. §1201(f), and case law such as *Sega v. Accolade*.
  - European Union: Article 6 of the Software Directive 2009/24/EC.
  - Upstream Rangoli and projects such as OpenRGB were built the same way.
- To stay on the safe side, contributors should:
  - **not** commit the official software, its installer, or files from its install
    folder (effect files, images, configs) to the repository
  - **not** decompile or disassemble the official software. Watching USB traffic is
    enough, and it is the only method used here.
  - **not** commit raw capture files without looking at them first (see "Privacy"
    below). Describing the format in text, as [PROTOCOL.md](PROTOCOL.md) does, is
    enough.
- If the official software's licence terms prohibit this in your country, or you
  are unsure, don't take captures. Report what the keyboard does with Rangoli
  instead.

## Privacy

A USB capture can contain **everything typed on every captured device**: passwords,
messages, anything. Before sharing a capture:

- Capture only the keyboard's USB device, or only its root hub.
- Keep the capture short. Start it, do the one action, stop it.
- Don't type anything private while it runs. Use a second keyboard or the mouse for
  anything else.
- Preferably share the decoded output of the tools below, which contains only the
  configuration reports, instead of the raw `.pcapng`.

## What you need (Windows)

1. **The official RK software** for your keyboard model, from Royal Kludge's site.
2. **Wireshark**, with **USBPcap** ticked during installation. USBPcap is the driver
   that lets Wireshark see USB traffic, and it needs a reboot after installing.
3. Python 3 (on any OS) to run the decoders in `tools/capture/`.

## Taking a capture

1. Plug in the keyboard and open the RK software. If it says the keyboard is
   offline or asleep, press a key on the keyboard to wake it.
2. Start Wireshark and pick the **USBPcap** interface the keyboard is on. If you're
   unsure which one, pick each in turn and type a key. The right one shows packets.
3. Start the capture.
4. **Do exactly one thing** in the RK software, and click **Apply**. For example, set
   one custom key colour, or create one macro and assign it to a key. Clicking
   Apply is what sends the data to the keyboard.
5. Check the result on the keyboard (does the key light, does the macro type?), then
   stop the capture.
6. **File → Save As**, in pcapng format, with a descriptive name such as
   `macro_abc_to_numplus.pcapng`.
7. **Write down exactly what you did:** which key, which settings, which timing mode,
   and any mistakes. Unexpected data is useful too. A capture with a typo in it
   taught us how Shift is stored.

### Captures that were taken for the RK96

| Capture | What was done | What it showed |
|---|---|---|
| custom lighting, one key | Set one key in custom mode, Apply | Static custom buffer, 1 byte per LED |
| custom lighting, several keys | As above with a few keys | `bufferIndex` → LED mapping |
| software effects (audio bar, others) | Played the official software's host-driven effects | The live lighting frames |
| `macro_helloworldtof12` | Macro "helloworld", default timing, on F12 | Macro area pages, the key map macro entry |
| `macro_abctonumplus` | Macro "abc", 30 ms timing, on numpad + | Delays, a second slot |
| `shiftandctrl` | "Hi.!" + Ctrl+Tab with recorded timing on F10, "Hi!~!" + Ctrl+Tab with default timing on F12 | Modifier events, 11-bit delays |

A good rule: **change one thing between two captures.** Comparing the two then shows
which bytes encode that one thing.

## Decoding

The tools in `tools/capture/` need only Python 3, not Wireshark. Run them with
`python3 -I`, so a capture file can't cause local Python code to run.

```sh
# Every SET_REPORT the software sent, with its data (lines starting req21/09)
python3 -I tools/capture/usbpcap.py capture.pcapng

# The macro slots in a capture, decoded
python3 -I tools/capture/macrodecode.py capture.pcapng
```

Example `usbpcap.py` output for a static custom lighting save:

```
2461  12.660 dev4 req21/09 val030a idx1 len65 0a0301037e0100000000000007000007...
2463  12.671 dev4 req21/09 val030a idx1 len65 0a030200000000000000000000000000...
2465  12.682 dev4 req21/09 val030a idx1 len65 0a030300000000000000000000000000...
```

- The third column is the USB device address, which differs per capture.
- `val030a` means a feature report (`03`) with report ID `0a`.
- The data starts with the report ID and the packet header described in
  [PROTOCOL.md](PROTOCOL.md).
- The timestamps show how far apart the software sends reports, about 11 ms here.

Wireshark can show the same thing. Use the display filter
`usb.setup.bRequest == 9`, and the data is under "Data Fragment".

## Verifying on Linux

Once a format is guessed from captures, it was confirmed by writing it from Linux
and watching what the keyboard did:

- Find the configuration interface's `hidraw` node:
  `ls /sys/class/hidraw/*/device/` for interface `:1.1`, or look for the node that
  has report ID `0x0a` in its report descriptor.
- Permissions come from Rangoli's udev rule (`uaccess` on the keyboard's VID/PID).
- Feature reports can be sent from Python with `fcntl.ioctl` and
  `HIDIOCSFEATURE(65)`, or with any hidapi binding.
- For macros, the keyboard's own output was logged by reading the `hidraw` nodes'
  input reports with timestamps. That is how the delay unit (about 6.94 ms) was
  measured. **Such a log records everything typed, so delete it afterwards.**

The throwaway scripts used for these tests aren't included, because they hard-code
device paths and capture names. The two decoders above are the parts worth reusing.
