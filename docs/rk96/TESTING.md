# Testing on other keyboards

This branch has only been tested on the single-colour RK96 (`258a:00a4`). This page
explains how to try it on another model, and what evidence is needed before each
feature can be switched on for that model.

Please report results with:

- the keyboard's name, and its USB ID from `lsusb`
- whether it is RGB or single-colour
- your OS and Qt version
- what worked, what didn't, and what the keyboard actually did

## Before you start

- **Keep the official software handy.** If anything goes wrong, applying a profile
  from the official software (or from upstream Rangoli) rewrites the same settings
  and puts the keyboard back.
- Note your current key map and macros. **Applying a profile that has macros
  replaces all macros stored on the keyboard.**

## Checklist

### 1. Basic (expected to work on any supported 258a board)

- [ ] It builds and starts. The keyboard is detected.
- [ ] Built-in light modes, brightness, speed and sleep apply.
- [ ] The key map applies. Remap a key, Apply, and check it.
- [ ] The tray menu lists the profiles. Choosing one applies it.
- [ ] Restart Rangoli with no default profile set. The last selected profile is
      used.

### 2. Custom lighting and My Effects

- [ ] Custom mode lights **exactly** the keys picked. Wrong keys on a single-colour
      board means its `bufferIndex` values or byte layout differ.
- [ ] Save as a new effect, rename it, duplicate it, delete it.
- [ ] Two profiles using the same effect both change when the effect is edited.

### 3. Animated effects (single-colour boards only)

Animation is enabled for every `"rgb": false` board, but has only been confirmed on
the RK96.

- [ ] Turn on animation for a custom effect, pick **Sequence**, and click a few keys.
      The physical keyboard plays the same thing as the preview.
- [ ] Try wave, ripple, breathing and twinkle. Check that brightness levels look
      smooth, not just on/off.
- [ ] Stop from the tray. The keyboard returns to its saved lighting.
- [ ] Quit Rangoli while an animation plays. The keyboard returns to its saved
      lighting.
- [ ] Click Apply while an animation plays. The save goes through and the
      animation resumes.

If the keyboard shows nothing, flickers, or freezes, set animation off and report
it. A capture of the official software's own host-driven effects for that model
(see [CAPTURING.md](CAPTURING.md)) would show its live frame format.

**To enable animation on RGB boards:**

1. Capture the official software running a host-driven effect (such as the audio
   visualiser) on an RGB board.
2. From the capture, confirm the packet count (`0a <N> i`) and the per-LED layout.
   It is probably 3 bytes per LED after one pad byte.
3. Update `sendLiveFrame()` and `HIDConnectionWorker::sendLiveFrame()` to use that
   layout, and render colour instead of brightness.
4. Relax `animationSupported()`.

### 4. Macros

Macros are switched off for every model except the RK96, the only one where the
format has been confirmed. To check another model:

1. On Windows, follow [CAPTURING.md](CAPTURING.md) to capture the official software
   saving one short macro (for example `abc`) to one key, then a second capture with
   Shift and Ctrl in it.
2. Run `python3 -I tools/capture/macrodecode.py capture.pcapng`.
   - It should find 31 pages of `0a 1f`, a header of `0400`, and your macro in
     slot 1 with sensible events.
   - If it reports a different page count, or the events look scrambled, the model
     uses a different layout. Please share the decoded output.
3. Check the key map entry for the macro key: `02 <slot> 01 <cycles>`. Note any
   other "cycle" options the software offers, and capture them too.
4. If everything matches, add `"macrosEnabled": true` to the model's keyboard JSON
   (`keyboards/258a/configs/<pid>.json`, next to `"rgb"`), and try a macro from
   Rangoli:
   - [ ] Record `abc`, put it on a spare key, Apply. Pressing the key types `abc`.
   - [ ] A macro with Shift (`Hi!`) and with Ctrl+something works.
   - [ ] Set a delay of 5000 ms between two events and time it. If it takes about
         5 s, the RK96's 6.94 ms unit applies. If it takes about 7.2 s, the model
         uses 10 ms units, like the official software assumes. Any other time
         means a different unit.

If it all works, send the JSON change with your results so the model can be
enabled for everyone. If anything differs, leave the flag off and share the decoded
capture output. The format may need per-model settings.
