# gramocli

A non-interactive, command-line-only derivative of
[GramoFile](http://www.opensourcepartners.nl/~costar/gramofile/) by Anne
Bezemer and Ton Le — a tool for cleaning up (tick/scratch reduction) and
track-splitting digitized gramophone/vinyl recordings. (The project's old
SourceForge page is gone; this is GramoFile's actual original homepage, per
Debian's package metadata.)

GramoFile's original ncurses-based interactive UI has been removed; the same
signal-processing filters and track-splitting logic are instead driven
directly from command-line arguments, suitable for scripting/batch use.

The original GramoFile README is preserved unchanged as
[README.gramofile](README.gramofile) for reference and attribution.

## Usage

```
./gramocli -i <infile.wav> -s <100..600> -f <filter1 filter2 ..> -o <out.wav>

  0 - simple median filter
  1 - simple mean filter
  2 - conditional median filter
  3 - double median filter
  4 - conditional median filter II
  5 - RMS filter

gramocli splits <infile.wav> into <out1.wav>, <out2.wav>, ... depending on
track pauses. Each out*.wav is processed with the selected filters (or left
unprocessed if none given). If no full path is given for <out.wav>, output
files are stored in the current directory.

Requirements for <infile.wav>:
  - RIFF, PCM 44100Hz, 16-bit, stereo
  - full path required
  - avoid excessive silence before the first / after the last track for
    reliable track detection (3-5 seconds works best)
  - record each side of an LP as a separate file, not both sides in one
```

## License

GPL-2.0-or-later, same as the original GramoFile — see [LICENSE](LICENSE).
Original copyright notices (`Copyright (C) 1998 J.A. Bezemer`) are preserved
in every source file carried over from GramoFile.

Modifications (removal of the ncurses UI, non-interactive CLI interface):
Copyright (C) 2024 ArchaicBlues.
