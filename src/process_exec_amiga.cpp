// process_exec_amiga.cpp - zen/process_exec.h for AmigaOS, in place of
// zen/process_exec.cpp (Linux: fork, exec, pipes).
//
// A command runs through dos.library's SystemTags in a Shell of its own.
// With a time-out (FreeFileSync's "run this after syncing" and "open
// with"), the command is started asynchronously and left to run: the
// caller takes SysErrorTimeOut as "started, probably fine", which is
// what it means on Linux too. Without one, it runs to the end with its
// output in a T: file, read back as the result.
//
// "Open with the default application" is Workbench's: OpenWorkbenchObject
// (workbench.library 44) does what a double-click on the icon does.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include <zen/process_exec.h>
#include <zen/file_access.h>
#include <zen/file_io.h>
#include <zen/guid.h>
#include "amiga_util.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dostags.h>
#include <utility/tagitem.h>

namespace
{
//workbench.library 44 OpenWorkbenchObjectA(name a0, tags a1), LVO -96; called
//directly, since the stove's NDK headers stop at workbench.library 39
LONG wbOpenObject(struct Library* wbBase, const char* name)
{
    register LONG d0 __asm("d0");
    register struct Library* a6 __asm("a6") = wbBase;
    register const char* a0 __asm("a0") = name;
    register const struct TagItem* a1 __asm("a1") = nullptr;
    __asm volatile ("jsr -96(%%a6)"
                    : "=r"(d0), "+r"(a0), "+r"(a1)
                    : "r"(a6)
                    : "d1", "cc", "memory");
    return d0;
}
}

using namespace zen;

Zstring zen::escapeCommandArg(const Zstring& arg)
{
    //AmigaDOS: quotes around anything with a space or a quote; *" is a quote inside
    if (!arg.empty() && std::none_of(arg.begin(), arg.end(), [](char c) { return c == ' ' || c == '"' || c == '*' || c == ';'; }))
        return arg;
    Zstring out = "\"";
    for (const char c : arg)
    {
        if (c == '"' || c == '*')
            out += '*';
        out += c;
    }
    return out + '"';
}

std::pair<int /*exit code*/, Zstring> zen::consoleExecute(const Zstring& cmdLine, std::optional<int> timeoutMs) //throw SysError, SysErrorTimeOut
{
    if (timeoutMs)
    {
        BPTR in  = Open(reinterpret_cast<CONST_STRPTR>("NIL:"), MODE_OLDFILE);
        BPTR out = Open(reinterpret_cast<CONST_STRPTR>("NIL:"), MODE_NEWFILE);
        if (!in || !out)
        {
            if (in) Close(in);
            if (out) Close(out);
            throw SysError(formatDosError("Open(NIL:)", IoErr()));
        }
        //SYS_Asynch: the new Shell closes in and out when the command ends
        const LONG rv = SystemTags(reinterpret_cast<CONST_STRPTR>(cmdLine.c_str()),
                                   SYS_Input, in, SYS_Output, out, SYS_Asynch, TRUE,
                                   NP_StackSize, 65536, TAG_END);
        if (rv == -1)
        {
            const LONG err = IoErr();
            Close(in);
            Close(out);
            throw SysError(formatDosError("SystemTags", err));
        }
        throw SysErrorTimeOut(_("Operation timed out."));
    }

    const Zstring tmpPath = "T:FreeFileSync-" + formatAsHexString(generateGUID()) + ".out";
    BPTR out = Open(reinterpret_cast<CONST_STRPTR>(tmpPath.c_str()), MODE_NEWFILE);
    if (!out)
        throw SysError(formatDosError("Open(" + tmpPath + ")", IoErr()));
    BPTR in = Open(reinterpret_cast<CONST_STRPTR>("NIL:"), MODE_OLDFILE);
    const LONG rv = SystemTags(reinterpret_cast<CONST_STRPTR>(cmdLine.c_str()),
                               SYS_Input, in, SYS_Output, out, NP_StackSize, 65536, TAG_END);
    const LONG err = IoErr();
    if (in) Close(in);
    Close(out);

    std::string output;
    try { output = getFileContent(tmpPath, nullptr /*notifyUnbufferedIO*/); } //throw FileError
    catch (FileError&) {}
    DeleteFile(reinterpret_cast<CONST_STRPTR>(tmpPath.c_str()));

    if (rv == -1)
        throw SysError(formatDosError("SystemTags", err));
    return {static_cast<int>(rv), Zstring(output)};
}

void zen::openWithDefaultApp(const Zstring& itemPath) //throw FileError
{
    struct Library* WorkbenchBase = OpenLibrary(reinterpret_cast<CONST_STRPTR>("workbench.library"), 44);
    if (WorkbenchBase)
    {
        const BOOL ok = wbOpenObject(WorkbenchBase, itemPath.c_str()) != 0;
        CloseLibrary(WorkbenchBase);
        if (ok)
            return;
    }
    throw FileError(replaceCpy(_("Cannot open file %x."), L"%x", fmtPath(itemPath)), L"OpenWorkbenchObject");
}
