# RK96 single-colour branch: overview of the changes

This branch (`rk96-single-colour`) adds a number of features and fixes to Rangoli 2.0.
It was developed and tested on **one keyboard only**:

| | |
|---|---|
| Keyboard | Royal Kludge RK96, single-colour (blue) backlight |
| USB ID | `258a:00a4` |
| OS | Linux Mint 22.3 (Ubuntu 24.04 base), X11 |
| Qt | 6.4.2 (the distribution's packages) |
| Official software used for captures | RK Keyboard Software on Windows, with Wireshark + USBPcap |

**Nothing here has been tested on any other model, on RGB keyboards, on Windows or
on macOS.** Some of it is very likely to work elsewhere because it uses the same
protocol Rangoli already uses. Some of it (live lighting and macros) relies on
details that were only observed on the RK96. The table under
[Test status](#test-status) spells out which is which. The changes
were made with an AI coding assistant (Claude) working with the keyboard's owner,
who did all the hardware testing and the Windows captures.

Contact: gdogg1587 on GitHub, or meh1587@protonmail.com. Reports from other
models are very welcome; [TESTING.md](TESTING.md) lists what to try.

Related documents:

- [PROTOCOL.md](PROTOCOL.md): the protocol details this branch relies on, including
  the parts that are new to Rangoli (live lighting, the macro area).
- [CAPTURING.md](CAPTURING.md): how the captures were made and decoded, so the same
  can be done for other models.
- [TESTING.md](TESTING.md): a checklist for trying the branch on another keyboard,
  and what is needed before each feature can be enabled for it.

## Commits

The branch starts at upstream `503cdc3` (Merge pull request #35) and is a straight
line of commits. Each one builds and works on its own:

| Commit | Summary |
|---|---|
| `00c0fe8` | Fix custom lighting on single-colour boards and build with Qt 6.4 |
| `0f18c8c` | Add saved custom lighting effects ("My Effects") |
| `37bbfcd` | Add animated custom lighting effects |
| `e907697` | Add fade in, reverse direction and multi-key sequence steps |
| `741a931` | Move the tray icon to C++ and add play/stop animation to it |
| `98bb466` | Use the last selected profile when no default profile is set |
| `b62f6de` | Add trail length and fade shapes (comet) to animated effects |
| `46212e4` | Add shuffle to sequence animations |
| `b49854b` | Record the timing of key presses as sequence pauses |
| `dcb2f2c` | Label the apply and undo buttons and open a wider window |
| `b70c541` | Add a background layer to animated effects |
| `8cd2543` | Add keyboard macros |
| `ce6dc74` | Limit macros to keyboards whose macro format is confirmed |
| `8ad829d` | Allow only one copy of Rangoli at a time |

The full commit messages explain each change in more detail.

## The changes, feature by feature

### 1. Fixes (`00c0fe8`, `98bb466`)

- **Custom lighting on single-colour keyboards.** Upstream writes custom light mode
  data as RGB triplets at `bufferIndex * 3`. Single-colour boards expect **one byte
  per LED** at `bufferIndex`, so upstream lit the wrong keys on the RK96. Boards
  with `"rgb": false` in their keyboard JSON now get one byte per LED. RGB boards are
  unchanged.
- **Builds with Qt 6.4.** This is what Ubuntu 24.04 and Linux Mint 22 ship:
  - The minimum Qt version is lowered.
  - `Material.roundedScale` (Qt 6.5+) is no longer used.
  - `QApplication` and Qt Widgets are used for the tray.
- **Startup profile.** With no explicit default profile, upstream picked whichever
  profile came first in settings order. That order is effectively random, because
  profiles are keyed by UUID. The last selected profile is now remembered
  (`last_profile`) and used as the fallback.
- **Saves are paced and retried.** Feature reports are sent at least 11 ms apart,
  which is about the official software's pace. A refused report is tried up to 3
  times, and a failed save no longer leaks the device handle and buffers.
- Fix `CustomLightModeKeys::add()`, which called `beginResetModel()` without a
  matching `endResetModel()`.
- **Only one copy of Rangoli runs at a time** (`8ad829d`). Several copies streaming
  live lighting and saves to the same keyboard flooded it with reports until it
  stopped responding. This happened during testing, with an installed copy and two
  AppImage copies running at once. The keyboard recovered after it was unplugged
  and charged.
  - A second copy started at login exits quietly. Any other second copy brings the
    running copy's window forward.
  - The lock is per user, so it covers installed copies, AppImages and builds that
    use a different settings folder.
  - A second copy also no longer wipes the running copy's `~/rangoli.log`.

### 2. Saved custom effects, "My Effects" (`0f18c8c`)

- A custom light mode pattern can be saved as a named effect. Saved effects appear
  in a "My Effects" section under the built-in modes, and can be renamed,
  duplicated and deleted.
- Effects are stored per keyboard model in QSettings under
  `custom_effects/<vid:pid>/`.
- A profile refers to its effect by id, so editing an effect updates every profile
  that uses it.
- Leaving an effect with unsaved edits asks whether to save, discard or cancel.

This is pure application logic and should work on any keyboard that supports custom
lighting, RGB or not.

### 3. Animated custom effects (`37bbfcd`, `e907697`, `b62f6de`, `46212e4`, `b49854b`, `b70c541`)

Custom mode effects can be animated. The keyboard has no built-in way to store an
animation, so **Rangoli plays it live**: it streams frames to the keyboard about
25 times a second, using the keyboard's live lighting command (see
[PROTOCOL.md](PROTOCOL.md#live-lighting)). That is the same mechanism the official
software uses for its "music" and other software-driven effects. Because animations
are never written to the keyboard's flash, they cause no flash wear.

This also means **the keyboard itself only stores a still version of an animated
effect**, with every key in the effect lit. That still version is what shows
before Rangoli starts (at boot, on the login screen), on another computer, and
after Rangoli quits. The animation only plays while Rangoli is running.

Animation settings:

- **Patterns:** sequence, wave (left/right, top/bottom), ripple, breathing and twinkle.
- **Timing and brightness:** speed, fade in, fade out, brightness, loop mode (repeat,
  back and forth, once) and a pause between loops.
- **Fade shape:** linear, smooth or comet.
- **Trail length:** set in keys, so the trail stays the same length at any speed.
- **Reverse:** sequences play backwards, waves run the other way and ripples move
  inwards.

Sequences are an ordered list of steps:

- A step can light one key, several keys at once, or be a pause.
- A key can appear more than once.
- Steps can be clicked on the keyboard picture, typed (for example `asdffdsa`), or
  recorded from the physical keyboard. With **Keep timing**, recording turns the
  gaps between presses into pauses (capped at 5 s).
- **Shuffle** plays the steps in a new random order every loop. Pauses keep their
  place.

The **background layer** is a second, always-repeating pattern that plays under the
main animation, with its own pattern, speed, fade, brightness and pause. Where both
layers light a key, the brighter one shows.

The editor previews the animation on the keyboard picture.

How playback behaves:

- Playback runs from the controller, and each frame waits for the previous one to be
  sent.
- Saving to the keyboard pauses playback until the save has gone through, because
  saves and live frames must never interleave.
- When playback stops, Rangoli re-sends the light mode buffer. This happens on stop,
  on quit, and on logout (`SIGTERM`), and it makes the keyboard leave live mode and
  show its saved lighting again.
- An animated default profile starts playing when Rangoli launches.

**Animation is only enabled for single-colour keyboards**
(`KeyboardConfiguratorController::animationSupported()`), because the live frame
format has only been confirmed on the RK96. RGB boards almost certainly use 3 bytes
per LED in live frames as well. Enabling them needs a capture from an RGB board; see
[TESTING.md](TESTING.md).

### 4. System tray (`741a931`)

With Qt 6.4, `Qt.labs.platform` didn't show menu items added after the tray menu was
created, so the profile list never appeared. The tray is now a C++
`QSystemTrayIcon` with a `QMenu` that is rebuilt every time it opens. It contains:

- the profiles, with the current one checked
- Stop / Play animation
- Open Rangoli
- Quit

A stopped animation can be resumed from the tray or from the configurator without
writing to the keyboard again.

### 5. Clearer header and a wider window (`dcb2f2c`)

- The floppy-disk icon looked like "save this effect", but it actually saves the
  profile and sends it to the keyboard. It is now a text button, **Apply to
  keyboard**. The reset icon is now **Undo changes**.
- The header shows **Effect not saved** when the selected effect has edits that
  haven't been saved to My Effects.
- The window opens at 1240×860, capped to the screen size, so the larger keyboard
  pictures fit without scrolling.

### 6. Keyboard macros (`8cd2543`, `ce6dc74`)

> Upstream's README lists macros under "Features that will not be worked on", and
> recommends OS-level tools instead. This branch adds them anyway because the
> keyboard stores and plays macros itself. That means they work on any computer
> (including ones without Rangoli or AutoHotkey) and on the login screen. If upstream prefers not to take this feature, it is a single commit and
> can be dropped.

- **Macros are stored on the keyboard.** The RK96 has 10 macro slots of up to 64
  events each (a press or a release, then a delay). The format was decoded from
  captures of the official software; see [PROTOCOL.md](PROTOCOL.md#macros).
- **A macro editor** (the Macros button in the left menu) can:
  - record keys from the physical keyboard, optionally keeping the real timing
  - add typed text, using `{Name}` for special keys, for example `Hello{Tab}{Enter}`
    (Shift is added automatically for capitals and symbols)
  - edit the delay of each event, or set all delays at once
  - remove events and rename the macro
- **Assigning a macro to a key:** in the key map editor, click a key and choose
  **Play macro**.
- **The macro area is only sent when it changes.** Its hash is stored in QSettings
  (`macros/<vid:pid>/written`), so the flash isn't rewritten on every Apply. When it
  is sent, it goes before the key map, in the same order as the official software.
- **Known limitation:** Rangoli writes the whole macro area and key map, so macros
  made in the official software are replaced the next time Rangoli sends macros.
  Rangoli can't read the keyboard's current macros (reading back hasn't been
  investigated).
- **Only on confirmed models:** macros are only offered for keyboards with
  `"macrosEnabled": true` in their keyboard JSON. So far that is only the RK96
  (`258a/configs/a4.json`). On every other model, the Macros button and "Play macro"
  are hidden, the macro area is never sent, and a profile's macro keys are left
  at their default key. Enabling a model is a one-line change once it has been
  checked; see [TESTING.md](TESTING.md#4-macros).

## Test status

✅ = tested and working on the RK96 (`258a:00a4`). Nothing has been tested
anywhere else.

| Feature | RK96 single-colour | Other single-colour 258a boards | RGB 258a boards | Notes |
|---|:-:|:-:|:-:|---|
| Single-colour custom lighting fix | ✅ | Likely | n/a (unchanged) | Only affects `"rgb": false` boards |
| Qt 6.4 build | ✅ (Linux) | – | – | Windows and macOS builds not tried |
| My Effects | ✅ | Likely | Likely | Application logic only |
| Animated effects (live lighting) | ✅ | Likely, untested | **Disabled** | RGB needs a capture of a live frame |
| Tray, profile fallback, header, window | ✅ (Linux, Cinnamon) | Likely | Likely | Tray behaviour depends on the desktop |
| Macros | ✅ | **Off** | **Off** | Switched on per model with `macrosEnabled` once its format is confirmed |

"Likely" means the feature uses protocol parts upstream already uses for these
boards, so it is expected to work, but nobody has tried it.

## Risks to be aware of

- **Flash writes.** Saves (light mode, custom lighting, key map, macro area) are
  written to the keyboard's flash, just as they are with upstream and the official
  software. Live animation frames are not. Macros are only rewritten when they
  change, to keep flash writes to a minimum.
- **Writing the macro area to an untested model.** If a model stores macros
  differently, sending the RK96 layout could leave junk in its macro memory. That
  junk would only play if a key were mapped to it. This is why macros are switched
  off (`macrosEnabled`) for every model except the RK96.
- **The live lighting timeout.** Reports sent much less than about 11 ms apart are
  dropped by the RK96. Other models may need a different spacing.

## Building

Nothing changes from upstream, except that Qt 6.4 is now enough:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
