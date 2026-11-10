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
| `src/` | in place of FreeFileSync's Linux files: system information, running commands (SystemTags, Workbench's "open"), the recycle bin (the volume's Trashcan drawer), icons, task priority, the ARexx port (`rexx_amiga.cpp`); the network parts left out (`net_none.cpp`) |
| `src/amiga_glib.c`, `src/amiga_posix.c` | the GLib and POSIX calls FreeFileSync makes that AmigaOS and libnix lack: case and Unicode forms of names, file dates through SetFileDate, free space through Info |
| `compat/include/` | the headers those calls need, and a plain-text stand-in for wxRichTextCtrl |
| `scripts/` | the 68040 soft-float link specs, the tool icon |
| `tools/` | the release archive, packed with AmiCradle's own LhA writer |

## ARexx

FreeFileSync has an ARexx port, `FREEFILESYNC.1` (`FREEFILESYNC.2` for a second copy), from the GFOpenWX Toolkit's `wxARexxPort` on arexx.class (`src/rexx_amiga.cpp`, patch 0009).

| Command | Does |
| --- | --- |
| `OPEN FILE/A` | load a configuration (`.ffs_gui` or `.ffs_batch`) |
| `COMPARE` | compare, as the Compare button; answers once done |
| `SYNC` | synchronise, as the Synchronize button; answers once done |
| `BUSY` | RESULT: 1 while comparing or synchronising, else 0 |
| `QUIT`, `ACTIVATE`, `DEACTIVATE`, `VERSION`, `HELP`, `FAULT` | the toolkit's standard commands |

While a comparison or a synchronisation runs, `OPEN`, `COMPARE` and `SYNC` answer RC 10.

```
/* FreeFileSync: back up Work: */
ADDRESS 'FREEFILESYNC.1'
'OPEN Work:Backup.ffs_gui'
'COMPARE'
'SYNC'
```

`Rexx/Sync.rexx` (in the package's Rexx drawer) is an example. `tests/arexx.rexx` (in the tests drawer beside it) checks the whole port: `rx tests/arexx.rexx` with the program running; it QUITs the program at the end.

Return codes, as OS 3.2's arexx.class: 0 OK, 5 warning, 10 error, 20 unknown command or failure.

## Installing

Unpack `GFAmigaFreeFileSync-0.1.0-RC1.lha` anywhere (`lha x GFAmigaFreeFileSync-0.1.0-RC1.lha Work:`) and double-click `FreeFileSync`, or start it from a Shell. A batch job runs with its file as the argument: `FreeFileSync Work:Jobs/mirror.ffs_batch`. FreeFileSync keeps its settings and logs in `Settings/` inside its drawer.

## Licence

FreeFileSync is GPL 3; the patches and the files here are under the same licence (LICENSE).

Designed, tested and delivered by The AmiCradle Team.
Copyright (c) 2026 AmiCradle Team
