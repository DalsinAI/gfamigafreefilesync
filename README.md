# GF Amiga FreeFileSync

FreeFileSync for AmigaOS 3 (68040, soft float with the FPU taken when there is one), on the GFOpenWX Toolkit (wxWidgets on GFOpenLayout).

FreeFileSync 14.9 is fetched at build time at the commit `ffs.lock` pins, and `patches/` is applied to it; nothing third-party is kept in this repository.

## Status

0.1.0 RC1 (6 November 2026). Lab-tested on AmigaOS 3.2 (68040): a batch mirror between two folders, a second sync after changes (only the changed files go), and the main window.

This first version compares and syncs folders on the Amiga's own volumes (hard drives, CF and SD cards, network shares a file system mounts). FreeFileSync's FTP, SFTP and Google Drive clients need libcurl, libssh2 and OpenSSL, and are not in this build.

## Building

```
STOVE=~/AmiCradle/stoves/os32-gcc16-open6 OUT=out ./build-amiga.sh
```

The first run fetches the GFOpenWX Toolkit and fpuauto at the commits `ffs.lock` pins and builds the toolkit SDK, then FreeFileSync for what is left of 840 s; run it again and it goes on from where it stopped. `out/FreeFileSync/` (the program and its `Resources/`) appears when it links.

## What the port changes

| Where | What |
|---|---|
| `patches/` | FreeFileSync's own files: Amiga paths (`Work:Dir/File`, names the same in any case), the program's drawer, GTK's calls left out, the colours from the GFOpenPrefs Look |
| `src/` | in place of FreeFileSync's Linux files: system information, running commands (SystemTags, Workbench's "open"), the recycle bin (the volume's Trashcan drawer), icons, task priority; the network parts left out (`net_none.cpp`) |
| `src/amiga_glib.c`, `src/amiga_posix.c` | the GLib and POSIX calls FreeFileSync makes that AmigaOS and libnix lack: case and Unicode forms of names, file dates through SetFileDate, free space through Info |
| `compat/include/` | the headers those calls need, and a plain-text stand-in for wxRichTextCtrl |
| `scripts/` | the 68040 soft-float link specs, the tool icon |
| `tools/` | the release archive, packed with AmiCradle's own LhA writer |

## Installing

Unpack `GFAmigaFreeFileSync-0.1.0-RC1.lha` anywhere (`lha x GFAmigaFreeFileSync-0.1.0-RC1.lha Work:`) and double-click `FreeFileSync`, or start it from a Shell. A batch job runs with its file as the argument: `FreeFileSync Work:Jobs/mirror.ffs_batch`. FreeFileSync keeps its settings and logs in `Settings/` inside its drawer.

## Licence

FreeFileSync is GPL 3; the patches and the files here are under the same licence (LICENSE).

Designed, tested and delivered by The AmiCradle Team.
Copyright (c) 2026 AmiCradle Team
