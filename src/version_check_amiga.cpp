// version_check_amiga.cpp - FreeFileSync's ui/version_check.h for this
// port, in place of ui/version_check.cpp. The update check asks
// freefilesync.org for the newest Windows/macOS/Linux release, which says
// nothing about this Amiga build, so there is no automatic check; "Check
// for updates now" says where the Amiga builds come from instead.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include "ui/version_check.h"
#include <wx+/popup_dlg.h>

using namespace zen;
using namespace fff;

struct fff::UpdateCheckResultPrep {};
struct fff::UpdateCheckResult {};

bool fff::haveNewerVersionOnline(const std::string& onlineVersion) { return false; }

bool fff::automaticUpdateCheckDue(time_t lastUpdateCheck) { return false; }

SharedRef<const UpdateCheckResultPrep> fff::automaticUpdateCheckPrepare(wxWindow& parent)
{
    return makeSharedRef<const UpdateCheckResultPrep>();
}

SharedRef<const UpdateCheckResult> fff::automaticUpdateCheckRunAsync(const UpdateCheckResultPrep& resultPrep)
{
    return makeSharedRef<const UpdateCheckResult>();
}

void fff::automaticUpdateCheckEval(wxWindow& parent, time_t& lastUpdateCheck, std::string& lastOnlineVersion, const UpdateCheckResult& result) {}

void fff::checkForUpdateNow(wxWindow& parent, std::string& lastOnlineVersion)
{
    showNotificationDialog(&parent, DialogInfoType::info,
                           PopupDialogCfg().setTitle(_("Check for Program Updates")).
                           setMainInstructions(L"This is the Amiga build of FreeFileSync. New Amiga builds come with AmiCradle's updates."));
}
