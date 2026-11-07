// rexx_amiga.cpp - FreeFileSync's ARexx port, FREEFILESYNC.1, on the
// GFOpenWX Toolkit's wxARexxPort and arexx.class (patch 0009 makes it with
// the main window):
//   OPEN FILE/A   load a configuration (.ffs_gui or .ffs_batch)
//   COMPARE       compare, as the Compare button; answers once done
//   SYNC          synchronise, as the Synchronize button; answers once done
//   BUSY          RESULT: 1 while comparing or synchronising, else 0
// and the toolkit's QUIT, ACTIVATE, DEACTIVATE, VERSION, HELP, FAULT.
// While a comparison or a synchronisation runs, OPEN, COMPARE and SYNC
// answer RC 10.
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include "ui/main_dlg.h"
#include "version/version.h"
#include <zen/utf.h>
#include <wx/filefn.h>
#include <wx/openlayout/arexx.h>

namespace fff
{
class AmigaRexx
{
public:
    static void start(MainDialog* dlg)
    {
        auto* rexx = new wxARexxPort("FREEFILESYNC", wxString("FreeFileSync ") + ffsVersion);
        rexx->Add("OPEN", "FILE/A", [dlg](wxARexxCommand& c)
        {
            const wxString file = c.Str(0);
            if (dlg->operationInProgress_)
                c.SetError(10);
            else if (!wxFileExists(file))
                c.SetError(10, 205); //object not found
            else if (!dlg->loadConfiguration({zen::utfTo<Zstring>(file)}))
                c.SetError(10);
        });
        rexx->Add("COMPARE", "", [dlg](wxARexxCommand& c)
        {
            if (dlg->operationInProgress_)
                return c.SetError(10);
            wxCommandEvent ev;
            dlg->onCompare(ev);
        });
        rexx->Add("SYNC", "", [dlg](wxARexxCommand& c)
        {
            if (dlg->operationInProgress_)
                return c.SetError(10);
            wxCommandEvent ev;
            dlg->onStartSync(ev);
        });
        rexx->Add("BUSY", "", [dlg](wxARexxCommand& c)
        {
            c.SetResult(dlg->operationInProgress_ ? "1" : "0");
        });
        rexx->Open();
        dlg->Bind(wxEVT_DESTROY, [dlg, rexx](wxWindowDestroyEvent& ev)
        {
            if (ev.GetEventObject() == dlg)
                delete rexx;
            ev.Skip();
        });
    }
};

void amigaRexxStart(MainDialog* dlg) { AmigaRexx::start(dlg); }
}
