/* glib.h - the few GLib types and calls FreeFileSync's own code uses, for
 * AmigaOS, where there is no GLib. src/amiga_glib.c has the calls.
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#ifndef GFFFS_GLIB_H
#define GFFFS_GLIB_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef char gchar;
typedef int gint;
typedef unsigned int guint;
typedef int gboolean;
typedef long gssize;
typedef uint32_t gunichar;
typedef uint32_t GQuark;
typedef struct _GError { GQuark domain; gint code; gchar* message; } GError;
typedef enum { G_NORMALIZE_DEFAULT, G_NORMALIZE_NFD = G_NORMALIZE_DEFAULT,
               G_NORMALIZE_DEFAULT_COMPOSE, G_NORMALIZE_NFC = G_NORMALIZE_DEFAULT_COMPOSE,
               G_NORMALIZE_ALL, G_NORMALIZE_NFKD = G_NORMALIZE_ALL,
               G_NORMALIZE_ALL_COMPOSE, G_NORMALIZE_NFKC = G_NORMALIZE_ALL_COMPOSE } GNormalizeMode;
#define GLIB_CHECK_VERSION(a, b, c) 1
#define G_FILE_ERROR ((GQuark)1)
#define G_IO_ERROR   ((GQuark)2)
/* UTF-8 in, a new UTF-8 string out (g_free it); NFC/NFD for the Latin
 * letters with accents, other text unchanged */
gchar* g_utf8_normalize(const gchar* str, gssize len, GNormalizeMode mode);
gunichar g_unichar_toupper(gunichar c);
gunichar g_unichar_tolower(gunichar c);
void g_free(void* p);
void g_error_free(GError* e);
const gchar* g_quark_to_string(GQuark q);
const gchar* g_strerror(gint errnum);
void g_object_unref(void* object);
#ifdef __cplusplus
}
#endif
#endif
