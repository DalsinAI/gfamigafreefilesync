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
/* Linux errno names FreeFileSync's error text knows; libnix never sets
 * them, so each only needs its own number for the switch cases */
#ifndef ERESTART
#define ERESTART 1001
#endif
#ifndef EUCLEAN
#define EUCLEAN 1002
#endif
#ifndef ENOTNAM
#define ENOTNAM 1003
#endif
#ifndef ENAVAIL
#define ENAVAIL 1004
#endif
#ifndef EISNAM
#define EISNAM 1005
#endif
#ifndef EREMOTEIO
#define EREMOTEIO 1006
#endif
#ifndef EMEDIUMTYPE
#define EMEDIUMTYPE 1007
#endif
#ifndef ENOKEY
#define ENOKEY 1008
#endif
#ifndef EKEYEXPIRED
#define EKEYEXPIRED 1009
#endif
#ifndef EKEYREVOKED
#define EKEYREVOKED 1010
#endif
#ifndef EKEYREJECTED
#define EKEYREJECTED 1011
#endif
#ifndef ERFKILL
#define ERFKILL 1012
#endif
#ifndef EHWPOISON
#define EHWPOISON 1013
#endif

/* temporary files go to T: */
#ifndef P_tmpdir
#define P_tmpdir "T:"
#endif

/* libnix's struct stat has whole seconds only (st_mtime) */
#define GFFFS_ST_MTIM(st) (timespec{ (st).st_mtime, 0 })

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
/* UTC struct tm to time_t */
time_t timegm(struct tm* tm);
/* no sessions: each process its own, so a lock's owner is "alive" while
 * gfffs_task_exists says so */
pid_t getsid(pid_t pid);
/* 1 while exec lists the Task a pid names (getpid() is the Task's address) */
int gfffs_task_exists(pid_t pid);
#ifdef __cplusplus
}
#endif
#endif
