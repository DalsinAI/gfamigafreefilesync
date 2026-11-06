/* amiga_glib.c - the GLib calls FreeFileSync's own code makes (glib.h),
 * written for AmigaOS, where there is no GLib.
 *
 * Case: FreeFileSync compares names without case through
 * g_unichar_toupper. Here it maps ASCII, Latin-1, Latin Extended-A, Greek
 * and Cyrillic, which covers the names an Amiga's Latin-1 file systems
 * can hold, and more.
 *
 * Normal forms: a name typed on one machine can arrive decomposed (e + a
 * combining accent) from another. g_utf8_normalize composes (NFC) or
 * decomposes (NFD) the Latin-1 letters with accents; other text is copied
 * as it is.
 *
 * Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync. */
#include <glib.h>
#include <stdlib.h>
#include <string.h>

gunichar g_unichar_toupper(gunichar c)
{
    if (c < 0x80) return (c >= 'a' && c <= 'z') ? c - 0x20 : c;
    if (c < 0x100) {
        if (c == 0xb5) return 0x39c;
        if (c == 0xff) return 0x178;
        if (c >= 0xe0 && c <= 0xfe && c != 0xf7) return c - 0x20;
        return c;
    }
    if (c < 0x180) {
        if (c == 0x131) return 'I';
        if (c == 0x17f) return 'S';
        if ((c >= 0x139 && c <= 0x148) || (c >= 0x179 && c <= 0x17e))
            return (c & 1) ? c : c - 1;   /* odd upper, even lower */
        if (c == 0x138 || c == 0x149 || c == 0x178) return c;
        return (c & 1) ? c - 1 : c;       /* even upper, odd lower */
    }
    if (c >= 0x3b1 && c <= 0x3c9) return c == 0x3c2 ? 0x3a3 : c - 0x20;
    if (c >= 0x3ac && c <= 0x3af) {
        static const unsigned short g[] = { 0x386, 0x388, 0x389, 0x38a };
        return g[c - 0x3ac];
    }
    if (c == 0x3cc) return 0x38c;
    if (c == 0x3cd) return 0x38e;
    if (c == 0x3ce) return 0x38f;
    if (c >= 0x430 && c <= 0x44f) return c - 0x20;
    if (c >= 0x450 && c <= 0x45f) return c - 0x50;
    if (c >= 0xff41 && c <= 0xff5a) return c - 0x20;
    return c;
}

gunichar g_unichar_tolower(gunichar c)
{
    if (c < 0x80) return (c >= 'A' && c <= 'Z') ? c + 0x20 : c;
    if (c < 0x100) return (c >= 0xc0 && c <= 0xde && c != 0xd7) ? c + 0x20 : c;
    if (c < 0x180) {
        if (c == 0x130) return 'i';
        if (c == 0x178) return 0xff;
        if ((c >= 0x139 && c <= 0x148) || (c >= 0x179 && c <= 0x17e))
            return (c & 1) ? c + 1 : c;
        if (c == 0x131 || c == 0x138 || c == 0x149 || c == 0x17f) return c;
        return (c & 1) ? c : c + 1;
    }
    if (c >= 0x391 && c <= 0x3a9 && c != 0x3a2) return c + 0x20;
    if (c == 0x386) return 0x3ac;
    if (c >= 0x388 && c <= 0x38a) return c + 0x25;
    if (c == 0x38c) return 0x3cc;
    if (c == 0x38e || c == 0x38f) return c + 0x3f;
    if (c >= 0x410 && c <= 0x42f) return c + 0x20;
    if (c >= 0x400 && c <= 0x40f) return c + 0x50;
    if (c >= 0xff21 && c <= 0xff3a) return c + 0x20;
    return c;
}

/* Latin-1 letters with accents: the letter and its combining mark.
 * Index c - 0xc0; 0 where there is no decomposition. */
static const unsigned short marks[] = {
    /* grave, acute, circumflex, tilde, diaeresis, ring */
    0x300, 0x301, 0x302, 0x303, 0x308, 0x30a,
};
static int decompose(gunichar c, gunichar* base, gunichar* mark)
{
    static const char tbl[64][2] = {
        /* C0 */ {'A',0},{'A',1},{'A',2},{'A',3},{'A',4},{'A',5},{0,0},{'C',6},
        /* C8 */ {'E',0},{'E',1},{'E',2},{'E',4},{'I',0},{'I',1},{'I',2},{'I',4},
        /* D0 */ {0,0},{'N',3},{'O',0},{'O',1},{'O',2},{'O',3},{'O',4},{0,0},
        /* D8 */ {0,0},{'U',0},{'U',1},{'U',2},{'U',4},{'Y',1},{0,0},{0,0},
        /* E0 */ {'a',0},{'a',1},{'a',2},{'a',3},{'a',4},{'a',5},{0,0},{'c',6},
        /* E8 */ {'e',0},{'e',1},{'e',2},{'e',4},{'i',0},{'i',1},{'i',2},{'i',4},
        /* F0 */ {0,0},{'n',3},{'o',0},{'o',1},{'o',2},{'o',3},{'o',4},{0,0},
        /* F8 */ {0,0},{'u',0},{'u',1},{'u',2},{'u',4},{'y',1},{0,0},{'y',4},
    };
    if (c < 0xc0 || c > 0xff || !tbl[c - 0xc0][0]) return 0;
    *base = (unsigned char)tbl[c - 0xc0][0];
    *mark = tbl[c - 0xc0][1] == 6 ? 0x327 /* cedilla */ : marks[(int)tbl[c - 0xc0][1]];
    return 1;
}
static gunichar compose(gunichar base, gunichar mark)
{
    gunichar c, b, m;
    for (c = 0xc0; c <= 0xff; c++)
        if (decompose(c, &b, &m) && b == base && m == mark) return c;
    return 0;
}

static size_t utf8_get(const unsigned char* s, size_t n, gunichar* out)
{
    if (s[0] < 0x80 || n < 2) { *out = s[0]; return 1; }
    if ((s[0] & 0xe0) == 0xc0) { *out = ((s[0] & 0x1f) << 6) | (s[1] & 0x3f); return 2; }
    if ((s[0] & 0xf0) == 0xe0 && n >= 3) { *out = ((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f); return 3; }
    if ((s[0] & 0xf8) == 0xf0 && n >= 4) { *out = ((gunichar)(s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) | ((s[2] & 0x3f) << 6) | (s[3] & 0x3f); return 4; }
    *out = s[0]; return 1;
}
static size_t utf8_put(gunichar c, char* d)
{
    if (c < 0x80) { d[0] = (char)c; return 1; }
    if (c < 0x800) { d[0] = (char)(0xc0 | (c >> 6)); d[1] = (char)(0x80 | (c & 0x3f)); return 2; }
    if (c < 0x10000) { d[0] = (char)(0xe0 | (c >> 12)); d[1] = (char)(0x80 | ((c >> 6) & 0x3f)); d[2] = (char)(0x80 | (c & 0x3f)); return 3; }
    d[0] = (char)(0xf0 | (c >> 18)); d[1] = (char)(0x80 | ((c >> 12) & 0x3f));
    d[2] = (char)(0x80 | ((c >> 6) & 0x3f)); d[3] = (char)(0x80 | (c & 0x3f)); return 4;
}

gchar* g_utf8_normalize(const gchar* str, gssize len, GNormalizeMode mode)
{
    const unsigned char* s = (const unsigned char*)str;
    size_t n = len < 0 ? strlen(str) : (size_t)len, i = 0, o = 0;
    /* decomposing at most doubles a character's bytes (2 -> 1 + 2) */
    char* d = (char*)malloc(n * 2 + 1);
    const int nfc = mode == G_NORMALIZE_NFC || mode == G_NORMALIZE_NFKC;
    gunichar prev = 0; size_t prevAt = 0; int havePrev = 0;
    if (!d) return NULL;
    while (i < n) {
        gunichar c, b, m;
        i += utf8_get(s + i, n - i, &c);
        if (nfc) {
            gunichar k;
            if (havePrev && c >= 0x300 && c <= 0x36f && (k = compose(prev, c)) != 0) {
                o = prevAt; o += utf8_put(k, d + o); prev = k; havePrev = 0;
                continue;
            }
            prevAt = o; prev = c; havePrev = 1;
            o += utf8_put(c, d + o);
        } else if (decompose(c, &b, &m)) {
            o += utf8_put(b, d + o); o += utf8_put(m, d + o);
        } else
            o += utf8_put(c, d + o);
    }
    d[o] = 0;
    return d;
}

void g_free(void* p) { free(p); }
void g_error_free(GError* e) { if (e) { free(e->message); free(e); } }
const gchar* g_quark_to_string(GQuark q)
{
    return q == G_FILE_ERROR ? "g-file-error-quark" : q == G_IO_ERROR ? "g-io-error-quark" : "unknown-quark";
}
void g_object_unref(void* object) { (void)object; }
const gchar* g_strerror(gint errnum) { return strerror(errnum); }
