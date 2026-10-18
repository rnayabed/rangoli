# Protocol notes (VID 0x258a, as observed on the RK96 `258a:00a4`)

This document covers the parts of the Royal Kludge protocol this branch relies on.
It focuses on what is **new to Rangoli**: live lighting and the macro area. Anything
marked *(RK96)* has only been observed on the single-colour RK96. Other models may
differ.

All configuration happens through **HID feature reports** (`SET_REPORT`,
`bmRequestType 0x21`, `bRequest 0x09`) to the keyboard's configuration interface:

- USB interface 1 on the RK96. On Linux this is a separate `hidraw` node from the
  normal keyboard input on interface 0.
- Every report uses **report ID `0x0a`** and is **65 bytes** including the report ID.
- Most multi-packet transfers share the same header:

```
byte 0   0x0a          report id
byte 1   N             number of packets in this transfer
byte 2   i             this packet's number, 1..N
byte 3.. data          62 bytes
```

### Pacing

The official software sends reports roughly 11 ms apart. On the RK96, reports sent
much faster than that are dropped or time out, so Rangoli waits 11 ms after every
report. It also retries a refused report up to twice, 50 ms apart.

## Light mode (unchanged from upstream, listed for reference)

This is one report: `0a 01 01 02 29 <mode> 00 <animation speed> <brightness> <r> <g> <b> <random colour> <sleep> ...`.

Re-sending this report also matters for live lighting: it makes the keyboard **leave
live lighting** and go back to its saved effect (see below).

## Custom light mode (static, saved to flash)

- Upstream sends 7 packets of `0a 07 i`. The first packet's data starts with
  `03 7e 01`.
- RGB boards: 3 bytes per LED, at `bufferIndex * 3`.
- **Single-colour boards *(RK96)*: 1 byte per LED, at `bufferIndex`.** Upstream wrote
  RGB triplets here too, which lit the wrong keys. This branch writes `0xff` for a
  lit key.
- For comparison, the official software sends this RK96 transfer as 3 packets
  (`0a 03 i`, the same `03 7e 01` start). The keyboard accepts Rangoli's 7-packet
  version as well.

## Live lighting *(RK96)*

This is how the official software drives its host-side effects (for example its
music visualiser). Nothing is written to flash. The keyboard shows each frame until
the next one arrives.

```
3 packets: 0a 03 <i = 1..3> + 62 data bytes      (186 data bytes per frame)

frame byte 0                 padding, always 0
frame byte bufferIndex + 1   brightness of that LED, 0 (off) .. 250 (full) on the RK96
```

- In other words, the frame is the same 1-byte-per-LED layout as the static
  buffer, shifted by one pad byte. The first packet does **not** carry the
  `03 7e 01` start.
- How it was confirmed:
  - Captures of the official software streaming effects, and of it setting static
    custom lighting.
  - Frames written from Linux at about 25 fps on the RK96 throughout development.
- **Spacing:** the 3 packets of a frame must be at least about 11 ms apart.
  Rangoli sends one frame at a time and waits for it to finish before rendering the
  next, which gives about 25 fps.
- **Leaving live mode:** re-send the light mode report (above). The keyboard then
  returns to its saved lighting.
- **Don't interleave:** a save (any of the flash transfers) must not be mixed in
  with live frames. Rangoli pauses playback, saves, and then resumes.
- **The keyboard's sleep timer still applies.** If the keyboard hasn't been typed
  on for its sleep time, the LEDs stay off until a key is pressed, even while frames
  are being sent.
- **RGB boards:** almost certainly 3 bytes per LED, which would need more packets
  per frame. This hasn't been captured, so animation is disabled for
  `"rgb": true` boards.

## Key map (unchanged from upstream except macro entries)

- 9 packets of `0a 09 i`. The first packet's data starts with `01 f8`.
- After that, 4 bytes per key at `bufferIndex * 4`.
- Upstream's normal key entries are `00 <modifier bits> <HID usage> 00`.

### Macro key entries *(RK96)*

```
02 <slot> <mode> <cycles>
```

| Byte | Meaning |
|---|---|
| `02` | this key plays a macro |
| `slot` | 0-based macro slot (F12 → slot 0 was `02 00 01 01` in the capture) |
| `mode` | `01` = play a number of times. This is the only value seen; the RK96 software offers no "repeat while held" option |
| `cycles` | how many times to play the macro; Rangoli always writes `01` |

## Macros *(RK96)*

The official software sends the **macro area** first, then the key map that points
into it:

```
31 packets: 0a 1f <i = 1..31> + 62 data bytes     (1922-byte macro area)
```

### Macro area layout

```
offset 0      04 00                     header (always 04 00 in every capture)
offset 2      slot 0: 64 events x 3 bytes = 192 bytes
offset 194    slot 1
...
offset 1730   slot 9                    (10 slots, ends at byte 1922)
```

Unused events and slots are zero. A slot ends at its first all-zero event.

### Event format (3 bytes)

```
byte 0   bit 7       1 = release, 0 = press
         bits 0..6   delay, low 7 bits
byte 1   bits 4..7   0x0 = normal key, 0xf = modifier
         bits 0..3   delay, high 4 bits
byte 2               normal key: HID keyboard usage (page 0x07), e.g. 0x04 = a
                     modifier:   HID modifier bit
                       01 Left Ctrl   02 Left Shift   04 Left Alt   08 Left Win
                       10 Right Ctrl  20 Right Shift  40 Right Alt  80 Right Win
```

- **Delay** is the wait *after* the event: `((byte1 & 0x0f) << 7) | (byte0 & 0x7f)`.
  That is 11 bits, 0 to 2047 units.
- **The unit was measured on the RK96 at about 6.94 ms.** 100 units took about
  694 ms, and 356 units about 2.46 s, timed from the input reports. The official
  software writes `ms / 10` (it wrote 3 for a 30 ms delay), so its delays play back
  about 30% shorter than shown.
- **The last event** of a macro has delay 0.
- **Modifiers have to use the modifier flag.** Storing Shift as an ordinary key
  (usage `0xe1`) does nothing. The RK96 sends macro keystrokes on the configuration
  interface as a bitmap report (report ID `06`), not as boot-protocol reports, and a
  modifier has to be set through the modifier byte.
- **Delay 0 between events** can make the computer see keys out of order. For
  example, Ctrl down and Tab down in the same instant may arrive as Tab, then Ctrl.
  Rangoli therefore gives every event except the last at least 1 unit.

### Example

Decoded output of `tools/capture/macrodecode.py` for the official software's
version of `Hi!~!` followed by Ctrl+Tab, "default timing":

```
slot 2: dn shift +3, dn h +3, up h +3, up shift +3, dn i +3, up i +3,
        dn shift +3, dn 1 +3, dn ` +3, up ` +3, up 1 +3, up shift +3,
        dn shift +3, dn 1 +3, up 1 +3, up shift +3,
        dn ctrl +3, dn tab +3, up tab +3, up ctrl +0
```

Its first event, Shift down with 3 units, is stored as `03 f0 02`.

### How the official software uses slots

- It packs macros into slots in list order, and rewrites the slot numbers in the
  key map to match.
- It always sends the entire macro area, followed by the entire key map.
- Rangoli does the same. Because Rangoli can't read the keyboard's current macros,
  it **replaces** macros made by the official software. Rangoli only sends the area
  when its own copy changes (it compares against a stored hash).

### Unknowns

- Whether the area can be read back from the keyboard. Not investigated.
- Other `mode` values, such as repeat while held or toggle. These may exist on other
  models' software.
- Whether other models have the same slot count (10), slot size (64 events), header
  (`04 00`) and delay unit. **Assume nothing until it has been captured.**
- Mouse buttons or media keys in macros (the official software may offer them).
