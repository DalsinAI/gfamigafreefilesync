/* langinfo.h - the one item FreeFileSync asks for: the first day of the
 * week (1 Sunday .. 7 Saturday). Monday, as locale.library's UK default.
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_LANGINFO_H
#define GFFFS_LANGINFO_H
#define _NL_TIME_FIRST_WEEKDAY 0x20068
static inline const char* nl_langinfo(int item) { (void)item; return "\2"; }
#endif
