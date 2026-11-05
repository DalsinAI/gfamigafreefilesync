/* wx/taskbar.h - AmigaOS has no system tray. FreeFileSync's tray icon
 * (shown while a sync runs minimised) gets an icon that is never shown:
 * SetIcon says no, so FreeFileSync keeps its progress window instead.
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_WX_TASKBAR_H
#define GFFFS_WX_TASKBAR_H
#include <wx/event.h>
#include <wx/icon.h>
#include <wx/bmpbndl.h>
#include <wx/menu.h>

enum wxTaskBarIconType { wxTBI_DOCK, wxTBI_CUSTOM_STATUSITEM, wxTBI_DEFAULT_TYPE = wxTBI_CUSTOM_STATUSITEM };

class wxTaskBarIcon : public wxEvtHandler
{
public:
    explicit wxTaskBarIcon(wxTaskBarIconType = wxTBI_DEFAULT_TYPE) {}
    virtual ~wxTaskBarIcon() {}
    static bool IsAvailable() { return false; }
    bool IsOk() const { return false; }
    bool IsIconInstalled() const { return false; }
    bool SetIcon(const wxBitmapBundle&, const wxString& = wxString()) { return false; }
    bool RemoveIcon() { return true; }
    bool PopupMenu(wxMenu*) { return false; }
    void Destroy() { delete this; }
protected:
    virtual wxMenu* CreatePopupMenu() { return nullptr; }
};

class wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType t = wxEVT_NULL, wxTaskBarIcon* = nullptr) : wxEvent(wxID_ANY, t) {}
    wxEvent* Clone() const override { return new wxTaskBarIconEvent(*this); }
};
typedef void (wxEvtHandler::*wxTaskBarIconEventFunction)(wxTaskBarIconEvent&);
#define wxTaskBarIconEventHandler(func) wxEVENT_HANDLER_CAST(wxTaskBarIconEventFunction, func)

wxDECLARE_EVENT(wxEVT_TASKBAR_MOVE, wxTaskBarIconEvent);
wxDECLARE_EVENT(wxEVT_TASKBAR_LEFT_DOWN, wxTaskBarIconEvent);
wxDECLARE_EVENT(wxEVT_TASKBAR_LEFT_UP, wxTaskBarIconEvent);
wxDECLARE_EVENT(wxEVT_TASKBAR_RIGHT_DOWN, wxTaskBarIconEvent);
wxDECLARE_EVENT(wxEVT_TASKBAR_RIGHT_UP, wxTaskBarIconEvent);
wxDECLARE_EVENT(wxEVT_TASKBAR_LEFT_DCLICK, wxTaskBarIconEvent);
wxDECLARE_EVENT(wxEVT_TASKBAR_RIGHT_DCLICK, wxTaskBarIconEvent);
#endif
