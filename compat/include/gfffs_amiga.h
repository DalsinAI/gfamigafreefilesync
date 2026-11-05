/* gfffs_amiga.h - included first in every FreeFileSync file built for
 * AmigaOS (-include). The POSIX names FreeFileSync uses that libnix lacks:
 * flags it may simply leave out, and the calls src/amiga_posix.c has.
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_AMIGA_H
#define GFFFS_AMIGA_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* AmigaOS has no exec(), so nothing to close on it; no "follow" either:
 * an AmigaDOS soft link is opened as its target */
#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 0
#endif
#ifndef AT_FDCWD
#define AT_FDCWD (-100)
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif
#ifndef POSIX_FADV_NORMAL
#define POSIX_FADV_NORMAL 0
#define POSIX_FADV_RANDOM 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_NOREUSE 5
#endif
#ifndef EOPNOTSUPP
#define EOPNOTSUPP 95
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* the modification time through dos.library SetFileDate (the access
 * time, times[0], has no place on AmigaOS) */
int utimensat(int dirfd, const char* path, const struct timespec times[2], int flags);
/* -1/ENOSYS: FreeFileSync sets the time by name after closing */
int futimens(int fd, const struct timespec times[2]);
/* a hint only: 0 */
int posix_fadvise(int fd, off_t offset, off_t len, int advice);
/* timer.device's E-clock, the date and the task, mixed */
int getentropy(void* buffer, size_t length);
/* no owners on AmigaOS: 0 */
int lchown(const char* path, uid_t owner, gid_t group);
#ifdef __cplusplus
}
#endif
#endif
