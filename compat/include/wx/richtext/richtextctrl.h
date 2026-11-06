/* wx/richtext/richtextctrl.h - FreeFileSync shows read-only messages with
 * links in a wxRichTextCtrl. The openlayout toolkit has no rich text
 * library, so here it is a read-only multi-line wxTextCtrl: the text and
 * its links show as plain text, which can be selected and copied.
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_WX_RICHTEXTCTRL_H
#define GFFFS_WX_RICHTEXTCTRL_H
#include <wx/textctrl.h>

class wxRichTextAttr : public wxTextAttr
{
public:
    using wxTextAttr::wxTextAttr;
};

class wxRichTextCtrl : public wxTextCtrl
{
public:
    wxRichTextCtrl() {}
    wxRichTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                   long style = 0) :
        wxTextCtrl(parent, id, value, pos, size, style | wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP) {}

    void BeginSuppressUndo() {}
    void EndSuppressUndo() {}
    void SetLineHeight(int) {}
    void BeginParagraphSpacing(int, int) {}
    void EndParagraphSpacing() {}
    void BeginStyle(const wxRichTextAttr&) {}
    void EndStyle() {}
    void BeginURL(const wxString&) {}
    void EndURL() {}
};
#endif
