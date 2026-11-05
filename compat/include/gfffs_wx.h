/* gfffs_wx.h - included first in FreeFileSync's C++ files, after the
 * toolkit's own amigacompat.h. FreeFileSync builds on a wxWidgets 3.3 with
 * its own patches (its Bugs.txt); the openlayout toolkit is wxWidgets 3.2.
 * What FreeFileSync takes from those patches is here.
 *
 * The colour hook: FreeFileSync's patch to wx/settings.h lets it change
 * wxSystemSettings' colours (for dark mode). Here the hook is held but wx
 * doesn't ask it: the colours are the Look GFOpenPrefs chose.
 *
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_WX_H
#define GFFFS_WX_H
#ifdef __cplusplus
#include <memory>
#include <wx/settings.h>

struct wxColorHook
{
    virtual ~wxColorHook() {}
    virtual wxColor getColor(wxSystemColour index) const = 0;
};

inline std::unique_ptr<wxColorHook>& refGlobalColorHook()
{
    static std::unique_ptr<wxColorHook> globalColorHook;
    return globalColorHook;
}
#endif
#endif
