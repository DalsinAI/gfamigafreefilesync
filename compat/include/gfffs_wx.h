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
#include <wx/dcclient.h>
#include <wx/aui/framemanager.h>

/* wx 3.3 names: a DC only for measuring text, and one for a window */
using wxReadOnlyDC = wxDC;
class wxInfoDC : public wxClientDC
{
public:
    explicit wxInfoDC(wxWindow* win) : wxClientDC(win) {}
};

/* the openlayout port draws in pixels that are the logical ones */
#ifndef wxHAS_DPI_INDEPENDENT_PIXELS
#define wxHAS_DPI_INDEPENDENT_PIXELS
#endif

/* FreeFileSync's wx patch adds a grid line colour */
#ifndef wxSYS_COLOUR_GRIDLINES
#define wxSYS_COLOUR_GRIDLINES wxSYS_COLOUR_3DLIGHT
#endif

/* wx 3.3's wxAuiPaneInfoArray is a std::vector; 3.2's is an object array */
struct gfffsPaneRange
{
    wxAuiPaneInfoArray& a;
    struct It
    {
        wxAuiPaneInfoArray* a; size_t i;
        wxAuiPaneInfo& operator*() const { return a->Item(i); }
        It& operator++() { ++i; return *this; }
        bool operator!=(const It& o) const { return i != o.i; }
    };
    It begin() const { return { &a, 0 }; }
    It end() const { return { &a, a.GetCount() }; }
};
inline gfffsPaneRange gfffsPanes(wxAuiPaneInfoArray& a) { return { a }; }

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
