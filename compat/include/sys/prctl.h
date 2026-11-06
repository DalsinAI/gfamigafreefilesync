/* sys/prctl.h - thread names are a Linux thing; on AmigaOS the call does
 * nothing.
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_SYS_PRCTL_H
#define GFFFS_SYS_PRCTL_H
#define PR_SET_NAME 15
static inline int prctl(int option, ...) { (void)option; return 0; }
#endif
