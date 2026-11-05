// button_size_amiga.cpp - wxButton::GetDefaultSize(), which each wx port
// gives and the openlayout toolkit doesn't yet: FreeFileSync sizes its
// bitmap buttons from it. The same size as a button's own best size
// without a label: 60 wide, the GUI font's line plus 8 high.
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include <wx/button.h>
#include <wx/dcscreen.h>
#include <wx/settings.h>

wxSize wxButtonBase::GetDefaultSize(wxWindow* win)
{
    int h = 0;
    if (win)
        h = win->GetCharHeight();
    else
    {
        wxScreenDC dc;
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        h = dc.GetCharHeight();
    }
    return wxSize(60, h + 8);
}
