/* amiga_posix.c - the POSIX calls FreeFileSync makes that libnix lacks
 * (see compat/include/gfffs_amiga.h and sys/vfs.h for each one's rule).
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#include <gfffs_amiga.h>
#include <sys/vfs.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <exec/execbase.h>

/* seconds from 1 Jan 1970 to 1 Jan 1978, the AmigaOS date's start */
#define AMIGA_EPOCH 252460800L

static void toDateStamp(time_t t, struct DateStamp* ds)
{
    long s = (long)(t - AMIGA_EPOCH);
    if (s < 0) s = 0;
    ds->ds_Days = s / 86400;
    ds->ds_Minute = (s % 86400) / 60;
    ds->ds_Tick = (s % 60) * TICKS_PER_SECOND;
}

int utimensat(int dirfd, const char* path, const struct timespec times[2], int flags)
{
    struct DateStamp ds;
    struct stat st;
    time_t want;
    (void)dirfd; (void)flags;
    if (!path || !*path) { errno = ENOENT; return -1; }
    want = times ? times[1].tv_sec : time(NULL);
    /* the date is the volume's local time and libnix's stat() turns it
     * into Unix time with its own zone: set, read back, correct by the
     * difference, so stat() gives back exactly the time asked for */
    toDateStamp(want, &ds);
    if (!SetFileDate((CONST_STRPTR)path, &ds)) { errno = IoErr() == ERROR_OBJECT_NOT_FOUND ? ENOENT : EACCES; return -1; }
    if (stat(path, &st) == 0 && st.st_mtime != want) {
        toDateStamp(want - (st.st_mtime - want), &ds);
        if (!SetFileDate((CONST_STRPTR)path, &ds)) { errno = EACCES; return -1; }
    }
    return 0;
}

int futimens(int fd, const struct timespec times[2])
{
    (void)fd; (void)times;
    errno = ENOSYS;
    return -1;
}

int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
    (void)fd; (void)offset; (void)len; (void)advice;
    return 0;
}

int lchown(const char* path, uid_t owner, gid_t group)
{
    (void)path; (void)owner; (void)group;
    return 0;
}

int getentropy(void* buffer, size_t length)
{
    static unsigned long long s;
    unsigned char* p = (unsigned char*)buffer;
    struct DateStamp ds;
    size_t i;
    if (length > 256) { errno = EIO; return -1; }
    DateStamp(&ds);
    s ^= ((unsigned long long)ds.ds_Days << 40) ^ ((unsigned long long)ds.ds_Minute << 20) ^ ds.ds_Tick;
    s ^= (unsigned long long)(unsigned long)FindTask(NULL) << 8;
    s ^= (unsigned long long)clock() << 24;
    for (i = 0; i < length; i++) {
        /* xorshift64*, its top byte per byte */
        s ^= s >> 12; s ^= s << 25; s ^= s >> 27;
        if (!s) s = 0x9e3779b97f4a7c15ULL;
        p[i] = (unsigned char)((s * 0x2545f4914f6cdd1dULL) >> 56);
    }
    return 0;
}

int statfs(const char* path, struct statfs* buf)
{
    BPTR lock = Lock((CONST_STRPTR)path, SHARED_LOCK);
    struct InfoData* id;
    if (!lock) { errno = ENOENT; return -1; }
    id = (struct InfoData*)AllocVec(sizeof *id, MEMF_PUBLIC);
    if (!id) { UnLock(lock); errno = ENOMEM; return -1; }
    if (!Info(lock, id)) { FreeVec(id); UnLock(lock); errno = EIO; return -1; }
    memset(buf, 0, sizeof *buf);
    buf->f_type = id->id_DiskType;
    buf->f_bsize = buf->f_frsize = id->id_BytesPerBlock;
    buf->f_blocks = (unsigned long)id->id_NumBlocks;
    buf->f_bfree = buf->f_bavail = (unsigned long)(id->id_NumBlocks - id->id_NumBlocksUsed);
    buf->f_namelen = 107;
    FreeVec(id);
    UnLock(lock);
    return 0;
}

/* timegm: UTC broken-down time to seconds since 1970 (days from the civil
 * date, Howard Hinnant's algorithm), with tm_mon/tm_sec out of range
 * folded first as mktime does */
time_t timegm(struct tm* tm)
{
    long long y = tm->tm_year + 1900LL;
    long long mon = tm->tm_mon;
    y += mon / 12; mon %= 12;
    if (mon < 0) { mon += 12; --y; }
    {
        const long long m = mon + 1;              /* 1..12 */
        const long long yy = y - (m <= 2);
        const long long era = (yy >= 0 ? yy : yy - 399) / 400;
        const long long yoe = yy - era * 400;
        const long long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + tm->tm_mday - 1;
        const long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        const long long days = era * 146097 + doe - 719468;
        const long long t = days * 86400 + tm->tm_hour * 3600LL + tm->tm_min * 60LL + tm->tm_sec;
        return (time_t)t;
    }
}

pid_t getsid(pid_t pid)
{
    return pid == 0 ? getpid() : pid;
}

static int inList(struct List* l, struct Node* t)
{
    struct Node* n;
    for (n = l->lh_Head; n->ln_Succ; n = n->ln_Succ)
        if (n == t) return 1;
    return 0;
}

int gfffs_task_exists(pid_t pid)
{
    struct Node* t = (struct Node*)pid;
    int found;
    if (pid == 0) return 0;
    if ((struct Task*)t == FindTask(NULL)) return 1;
    Forbid(); /* only compares addresses: the pid is never read through */
    found = inList(&SysBase->TaskReady, t) || inList(&SysBase->TaskWait, t);
    Permit();
    return found;
}
