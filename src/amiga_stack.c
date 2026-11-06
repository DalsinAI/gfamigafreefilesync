/* A Shell gives 4 KB of stack; libnix's startup sets this much up instead.
 * wxWidgets' event handling and FreeFileSync's grids nest deeply.
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
unsigned long __stack = 1024 * 1024;
static const char version[] __attribute__((used)) = "$VER: FreeFileSync 14.9 (06.11.2026)";
