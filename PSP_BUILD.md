# Building SM64 for PSP (modern pspdev toolchain)

This fork builds the PSP port of the Super Mario 64 PC port against a current
pspdev toolchain. It contains **no Nintendo assets**: you must supply your own
legally obtained US ROM, and the build extracts everything it needs from it.
Do not upload your ROM, the extracted assets, or built EBOOT files; they contain
copyrighted material.

Tested on CachyOS (Arch Linux) with the prebuilt pspdev toolchain (GCC 15.2.0),
running on a PSP 3001 with custom firmware.

## What this fork changes

- Saves and the settings file are written with the PSP's own file calls
  (`sceIoOpen` and friends), because the C library `fopen` failed to create
  files in testing.
- Build fixes for newer compilers: `tools/armips.cpp` includes `<cstdint>`,
  `PSP_HEAP_SIZE_KB` replaces the removed `PSP_HEAP_SIZE_MAX`, and the PSP
  compiler flags add `-fcommon` and `-Wno-error=incompatible-pointer-types`.

## Requirements

1. The pspdev toolchain, with `psp-gcc` on your PATH.
   Download the prebuilt release from the pspdev project's GitHub releases page
   (`pspdev-ubuntu-latest-x86_64.tar.gz`), extract it to `~/pspdev`, then set:
   - bash/zsh: `export PSPDEV=$HOME/pspdev` and `export PATH=$PATH:$PSPDEV/bin`
   - fish: `set -Ux PSPDEV $HOME/pspdev` and `fish_add_path $PSPDEV/bin`
2. On Arch: `sudo pacman -S --needed base-devel git python audiofile`
   (the `audiofile` library is needed by one of the build tools).
3. A US SM64 ROM in big-endian `.z64` format, with SHA-1
   `9bef1128717f958171a4afac3ed78ee2bb4e86ce`, saved in the project folder as
   `baserom.us.z64`.

## Build

```
make TARGET_PSP=1 -j4
cp -r psp/textures/* textures/
make TARGET_PSP=1 -j4 pbp
```

The result is the folder `build/us_psp/mario64/`, containing `EBOOT.PBP` and
`snd_eng.prx`. Copy the whole folder to `PSP/GAME/` on your memory stick. Both
files must stay together.

## Notes

- The build prints a warning that the "stubs are out of order". The game ran
  fine in testing, but I have not investigated it further.
- The save file (`sm64_save_file.bin`) and the settings file (`sm64config.txt`)
  are created in the game's own folder on the memory stick.
- Eject the memory stick from your computer before leaving USB mode on the PSP.
  An unclean disconnect can corrupt the card.

## Credits

Built on the SM64 decompilation project, the SM64 PC port, and the PSP port by
gyrovorbis, all of whom did the hard work. This project is not affiliated with
or endorsed by Nintendo.

## How this was made

The PSP-specific changes in this fork (the save and settings fixes and the
build fixes for newer compilers) were developed with the help of an AI
assistant, Claude by Anthropic. I tested the results by building the project
and running it on a real PSP 3001. AI-assisted code can contain mistakes, so
review it before relying on it.
