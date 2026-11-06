// recycler_amiga.cpp - zen/recycler.h for AmigaOS, in place of
// zen/recycler.cpp (Linux: GIO's trash).
//
// The Amiga's recycle bin is the Trashcan drawer at the root of a volume
// (Format makes it; Workbench's Empty Trash empties it). An item moves
// there by renaming, which keeps it on the same volume; a name already in
// the drawer gets ".1", ".2" and so on. A volume without a Trashcan
// drawer has no recycle bin: FreeFileSync then asks to delete for good.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include <zen/recycler.h>
#include <zen/file_access.h>
#include <zen/file_path.h>
#include "amiga_util.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>

using namespace zen;

namespace
{
bool isDrawer(const Zstring& path)
{
    BPTR lock = Lock(reinterpret_cast<CONST_STRPTR>(path.c_str()), SHARED_LOCK);
    if (!lock)
        return false;
    bool dir = false;
    if (struct FileInfoBlock* fib = static_cast<struct FileInfoBlock*>(AllocDosObject(DOS_FIB, nullptr)))
    {
        dir = Examine(lock, fib) && fib->fib_DirEntryType > 0;
        FreeDosObject(DOS_FIB, fib);
    }
    UnLock(lock);
    return dir;
}

bool exists(const Zstring& path)
{
    BPTR lock = Lock(reinterpret_cast<CONST_STRPTR>(path.c_str()), SHARED_LOCK);
    if (lock)
        UnLock(lock);
    return lock != 0;
}
}

void zen::moveToRecycleBin(const Zstring& itemPath) //throw FileError, RecycleBinUnavailable
{
    const Zstring volume = beforeFirst(itemPath, Zstr(':'), IfNotFoundReturn::none);
    if (volume.empty())
        throw RecycleBinUnavailable(replaceCpy(_("The recycle bin is not available for %x."), L"%x", fmtPath(itemPath)));

    Zstring trash;
    for (const char* name : {"Trashcan", ".Trashcan"})
        if (isDrawer(volume + ':' + name))
        {
            trash = volume + ':' + name;
            break;
        }
    if (trash.empty())
        throw RecycleBinUnavailable(replaceCpy(_("The recycle bin is not available for %x."), L"%x", fmtPath(itemPath)),
                                    L"No Trashcan drawer on " + utfTo<std::wstring>(volume) + L':');

    const Zstring itemName = afterLast(itemPath, FILE_NAME_SEPARATOR, IfNotFoundReturn::all);
    const Zstring leaf = afterLast(itemName, Zstr(':'), IfNotFoundReturn::all);
    Zstring target = appendPath(trash, leaf);
    for (int i = 1; exists(target); ++i)
        target = appendPath(trash, leaf + '.' + numberTo<Zstring>(i));

    if (!Rename(reinterpret_cast<CONST_STRPTR>(itemPath.c_str()), reinterpret_cast<CONST_STRPTR>(target.c_str())))
        throw FileError(replaceCpy(_("Unable to move %x to the recycle bin."), L"%x", fmtPath(itemPath)),
                        formatDosError("Rename", IoErr()));
}
