// amiga_util.h - dos.library errors in FreeFileSync's error format: the
// IoErr() number and Fault()'s text for it.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#ifndef GFFFS_AMIGA_UTIL_H
#define GFFFS_AMIGA_UTIL_H
#include <zen/sys_error.h>
#include <zen/string_tools.h>
#include <proto/dos.h>

namespace zen
{
inline std::wstring formatDosError(const std::string& functionName, LONG ioErr)
{
    char text[128] = {};
    Fault(ioErr, nullptr, reinterpret_cast<STRPTR>(text), sizeof text);
    return formatSystemError(functionName, L"IoErr " + numberTo<std::wstring>(ioErr), utfTo<std::wstring>(std::string_view(text)));
}
}
#endif
