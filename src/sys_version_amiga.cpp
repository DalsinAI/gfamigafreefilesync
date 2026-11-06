// sys_version_amiga.cpp - zen/sys_version.h for AmigaOS, in place of
// zen/sys_version.cpp (Linux: lsb_release, /etc/os-release): the version
// of version.library, which is the version of the Workbench installed.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include <zen/sys_version.h>
#include <zen/string_tools.h>
#include <proto/exec.h>

using namespace zen;

OsVersionDetail zen::impl::getOsVersionRaw() //throw SysError
{
    OsVersionDetail d;
    d.osName = L"AmigaOS";
    if (struct Library* v = OpenLibrary(reinterpret_cast<CONST_STRPTR>("version.library"), 0))
    {
        d.version.major = v->lib_Version;
        d.version.minor = v->lib_Revision;
        CloseLibrary(v);
    }
    else
    {
        d.version.major = SysBase->LibNode.lib_Version;
        d.version.minor = SysBase->LibNode.lib_Revision;
    }
    d.osVersionRaw = numberTo<std::wstring>(d.version.major) + L'.' + numberTo<std::wstring>(d.version.minor);
    return d;
}

OsVersionDetail zen::getOsVersion()
{
    static const OsVersionDetail d = impl::getOsVersionRaw();
    return d;
}
