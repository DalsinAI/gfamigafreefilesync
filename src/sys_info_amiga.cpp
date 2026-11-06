// sys_info_amiga.cpp - zen/sys_info.h for AmigaOS, in place of
// zen/sys_info.cpp (Linux: /proc, passwd, XDG).
//
// The user is $USER (or "Amiga"), the host name is the TCP/IP stack's (or
// "amiga"), the program's own path comes from dos.library's program
// directory and name. FreeFileSync keeps its settings in its own drawer,
// Settings/, as Amiga programs do; "home" is $HOME, else SYS:.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include <zen/sys_info.h>
#include <zen/file_path.h>
#include <zen/sys_version.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <unistd.h>

using namespace zen;

namespace
{
Zstring lockName(BPTR lock)
{
    std::vector<char> buf(1024);
    if (lock && NameFromLock(lock, reinterpret_cast<STRPTR>(buf.data()), buf.size()))
        return buf.data();
    return {};
}
}

Zstring zen::getLoginUser() //throw FileError
{
    if (const std::optional<Zstring> user = getEnvironmentVar("USER"))
        if (!user->empty())
            return *user;
    return "Amiga";
}

Zstring zen::getHostName() //throw FileError
{
    char buf[256] = {};
    if (::gethostname(buf, sizeof buf - 1) == 0 && buf[0])
        return buf;
    return "amiga";
}

ComputerModel zen::getComputerModel() //throw FileError
{
    return {L"Amiga", L""};
}

std::wstring zen::getOsDescription()
{
    const OsVersionDetail verDetail = getOsVersion();
    return verDetail.osName + L" (" + verDetail.osVersionRaw + L')'; //e.g. "AmigaOS (47.7)"
}

Zstring zen::getProcessPath() //throw FileError
{
    const Zstring dir = lockName(GetProgramDir());
    char name[256] = {};
    if (dir.empty() || !GetProgramName(reinterpret_cast<STRPTR>(name), sizeof name))
        throw FileError(_("Cannot get process information."), L"GetProgramDir/GetProgramName");
    return appendPath(dir, Zstring(FilePart(reinterpret_cast<STRPTR>(name))));
}

Zstring zen::getUserHome() //throw FileError
{
    if (const std::optional<Zstring> home = getEnvironmentVar("HOME"))
        if (!home->empty())
            return *home;
    return "SYS:";
}

Zstring zen::getUserDataPath() //throw FileError
{
    //FreeFileSync appends "FreeFileSync": PROGDIR:Settings/FreeFileSync
    const Zstring dir = lockName(GetProgramDir());
    if (dir.empty())
        throw FileError(_("Cannot get process information."), L"GetProgramDir");
    return appendPath(dir, Zstr("Settings"));
}

Zstring zen::getUserDownloadsPath() //throw FileError
{
    return "RAM:";
}

bool zen::runningElevated() //throw FileError
{
    return false; //no users, no elevation
}
