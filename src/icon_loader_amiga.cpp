// icon_loader_amiga.cpp - FreeFileSync's base/icon_loader.h for AmigaOS,
// in place of base/icon_loader.cpp (Linux: GTK's icon theme, GdkPixbuf).
//
// File and drawer icons are the toolkit's own art (wxArtProvider), the
// same picture for every file of a kind: a file, a drawer, the trashcan,
// the file manager. A GIcon here is one of four fixed kinds, never freed.
// Thumbnails: none yet (on AmigaOS they will come through datatypes).
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include "base/icon_loader.h"
#include <zen/thread.h>
#include <wx/artprov.h>
#include <cstring>

using namespace zen;
using namespace fff;

struct _GIcon { const char* artId; };

namespace
{
GIcon iconFile   {wxART_NORMAL_FILE};
GIcon iconDir    {wxART_FOLDER};
GIcon iconTrash  {wxART_DELETE};
GIcon iconFileMgr{wxART_HARDDISK};

FileIconHolder holder(GIcon& icon, int maxSize) { return FileIconHolder(&icon, maxSize); }
}

FileIconHolder fff::getIconByTemplatePath(const Zstring& templatePath, int maxSize) { return holder(iconFile, maxSize); }
FileIconHolder fff::genericFileIcon(int maxSize)    { return holder(iconFile, maxSize); }
FileIconHolder fff::genericDirIcon(int maxSize)     { return holder(iconDir, maxSize); }
FileIconHolder fff::getTrashIcon(int maxSize)       { return holder(iconTrash, maxSize); }
FileIconHolder fff::getFileManagerIcon(int maxSize) { return holder(iconFileMgr, maxSize); }
FileIconHolder fff::getFileIcon(const Zstring& filePath, int maxSize) { return holder(iconFile, maxSize); }

ImageHolder fff::getThumbnailImage(const Zstring& filePath, int maxSize) //throw SysError
{
    return {};
}

wxImage fff::extractWxImage(ImageHolder&& ih)
{
    assert(runningOnMainThread());
    if (!ih.getRgb())
        return wxNullImage;

    wxImage img(ih.getWidth(), ih.getHeight(), ih.releaseRgb(), false /*static_data*/); //pass ownership
    if (ih.getAlpha())
        img.SetAlpha(ih.releaseAlpha(), false /*static_data*/);
    else
    {
        img.SetAlpha();
        std::memset(img.GetAlpha(), wxIMAGE_ALPHA_OPAQUE, ih.getWidth() * ih.getHeight());
    }
    return img;
}

wxImage fff::extractWxImage(zen::FileIconHolder&& fih)
{
    assert(runningOnMainThread());
    wxImage img;
    if (GIcon* icon = fih.gicon.release()) //fixed icons: not freed
    {
        const int sz = fih.maxSize > 0 ? fih.maxSize : 16;
        img = wxArtProvider::GetBitmap(icon->artId, wxART_OTHER, wxSize(sz, sz)).ConvertToImage();
        if (img.IsOk() && (img.GetWidth() > sz || img.GetHeight() > sz))
            img.Rescale(sz, sz, wxIMAGE_QUALITY_HIGH);
        if (img.IsOk() && !img.HasAlpha())
            img.InitAlpha();
    }
    return img;
}
