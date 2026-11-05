// taskbar_amiga.cpp - the tray icon events compat/include/wx/taskbar.h
// declares. No tray on AmigaOS, so nothing ever sends them.
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include <wx/taskbar.h>

wxDEFINE_EVENT(wxEVT_TASKBAR_MOVE, wxTaskBarIconEvent);
wxDEFINE_EVENT(wxEVT_TASKBAR_LEFT_DOWN, wxTaskBarIconEvent);
wxDEFINE_EVENT(wxEVT_TASKBAR_LEFT_UP, wxTaskBarIconEvent);
wxDEFINE_EVENT(wxEVT_TASKBAR_RIGHT_DOWN, wxTaskBarIconEvent);
wxDEFINE_EVENT(wxEVT_TASKBAR_RIGHT_UP, wxTaskBarIconEvent);
wxDEFINE_EVENT(wxEVT_TASKBAR_LEFT_DCLICK, wxTaskBarIconEvent);
wxDEFINE_EVENT(wxEVT_TASKBAR_RIGHT_DCLICK, wxTaskBarIconEvent);
