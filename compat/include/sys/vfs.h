/* sys/vfs.h - statfs() for AmigaOS: dos.library Info() on the volume the
 * path is on (src/amiga_posix.c).
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_SYS_VFS_H
#define GFFFS_SYS_VFS_H
#ifdef __cplusplus
extern "C" {
#endif
struct statfs {
    long f_type;
    long f_bsize;                   /* bytes a block */
    unsigned long long f_blocks;    /* blocks on the volume */
    unsigned long long f_bfree;     /* blocks free */
    unsigned long long f_bavail;    /* blocks free to this user */
    unsigned long long f_files, f_ffree;
    struct { int __val[2]; } f_fsid;
    long f_namelen, f_frsize, f_flags;
};
int statfs(const char* path, struct statfs* buf);
#ifdef __cplusplus
}
#endif
#endif
