#!/bin/sh
# Builds FreeFileSync for AmigaOS 3 with a stove: 68040, soft float with
# the FPU taken when there is one (fpuauto), on the GFOpenWX Toolkit.
#   STOVE=.../stoves/os32-gcc16-open6 OUT=out ./build-amiga.sh
#   (WXT_SRC=... to use a toolkit tree already on disk)
# FreeFileSync, the toolkit and fpuauto are fetched at the commits
# ffs.lock gives; nothing third-party is kept here. The build tree is
# kept, so a run that stops at BUDGET_TOTAL seconds goes on next time.
# OUT gets the FreeFileSync drawer (the program and Resources/) when it links.
# Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$HERE/ffs.lock"
STOVE=${STOVE:-"$HOME/AmiCradle/stoves/os32-gcc16-open6"}
# the build tree lives outside the checkout (Cradle cleans its checkout
# before every job), so a long build goes on across runs
WORK="${WORK:-${XDG_CACHE_HOME:-$HOME/.cache}/gfamigafreefilesync/build}"; OUT="${OUT:-$WORK/out}"; JOBS=${JOBS:-12}
TC="$STOVE/prefix"; PATH="$TC/bin:$PATH"; export PATH
mkdir -p "$WORK/deps" "$OUT"
t0=$(date +%s)

# a repository at one commit, fetched once (SPARSE: only those folders)
fetch() {
    d="$WORK/deps/$1"
    if [ "$(git -C "$d" rev-parse HEAD 2>/dev/null)" != "$3" ]; then
        rm -rf "${d:?}"; mkdir -p "$d"
        git -C "$d" init -q
        if [ -n "${4:-}" ]; then
            git -C "$d" config core.sparseCheckout true
            echo "$4" > "$d/.git/info/sparse-checkout"
            git -C "$d" fetch -q --depth 1 --filter=blob:none "$2" "$3"
        else
            git -C "$d" fetch -q --depth 1 "$2" "$3"
        fi
        git -C "$d" checkout -q FETCH_HEAD
        [ "$(git -C "$d" rev-parse HEAD)" = "$3" ] || { echo "$1: commit mismatch"; exit 1; }
    fi
}

# --- the toolkit's SDK, again only when its tree changed ---------------------
if [ -z "${WXT_SRC:-}" ]; then fetch gfopenwxtoolkit "$WXT_REPO" "$WXT_COMMIT"; WXT_SRC="$WORK/deps/gfopenwxtoolkit"; fi
WXB="$WORK/gfopenwx"
# a toolkit SDK another job built (WXT_SDK_JOB in ffs.lock): the stove's
# 900 s don't cover a toolkit build and FreeFileSync in one run
SDKT="$OUT/../../${WXT_SDK_JOB:-none}/out/gfopenwx-sdk.tar.gz"
if [ -n "${WXT_SDK_JOB:-}" ] && [ -f "$SDKT" ]; then
    WXS="$WORK/gfopenwx-sdk-$WXT_SDK_JOB"
    if [ ! -f "$WXS/.done" ]; then
        rm -rf "$WXS"; mkdir -p "$WXS"; tar xzf "$SDKT" -C "$WXS"; touch "$WXS/.done"
    fi
    WXC=$(find "$WXS" -name wx-config -type f | head -1)
    ZLIBI=$(dirname "$(find "$WXS" -name zlib.h | head -1)")
    # wx 3.2.12's own zlib headers, from an earlier toolkit build here
    [ -f "$ZLIBI/zlib.h" ] || ZLIBI=$(ls -d "$WXB"/wxWidgets-*/src/zlib 2>/dev/null | head -1)
    echo "toolkit: SDK of $WXT_SDK_JOB ($WXC, zlib $ZLIBI)"
fi
if [ -z "${WXC:-}" ]; then
WXREV=$( (git -C "$WXT_SRC" rev-parse HEAD 2>/dev/null; git -C "$WXT_SRC" diff HEAD 2>/dev/null) | sha256sum | cut -c1-16)
if [ ! -x "$WXB/out/sdk/bin/wx-config" ] || [ "$(cat "$WXB/out/.rev" 2>/dev/null)" != "$WXREV" ]; then
    STOVE="$STOVE" WORK="$WXB" OUT="$WXB/out" JOBS=$JOBS sh "$WXT_SRC/build-amiga.sh" > "$WORK.wx.log" 2>&1 || { tail -40 "$WORK.wx.log"; exit 1; }
    [ -x "$WXB/out/sdk/bin/wx-config" ] || { tail -40 "$WORK.wx.log"; echo "no toolkit SDK"; exit 1; }
    echo "$WXREV" > "$WXB/out/.rev"
fi
echo "toolkit: $(( $(date +%s) - t0 )) s"
WXC="$WXB/out/sdk/bin/wx-config"
ZLIBI=$(ls -d "$WXB"/wxWidgets-*/src/zlib | head -1)
fi
[ -f "$ZLIBI/zlib.h" ] || { echo "no zlib.h ($ZLIBI)"; [ -n "${WXS:-}" ] && (cd "$WXS" && find . -maxdepth 3 | head -40); exit 1; }

# --- fpuauto: libfpuauto.a and its link specs ---------------------------------
fetch fpuauto "$FPA_REPO" "$FPA_COMMIT" "/common/fpuauto/"
FA="$WORK/deps/fpuauto/common/fpuauto"; FAL="$WORK/fpuauto"
if [ ! -f "$FAL/libfpuauto.a" ] || [ "$(cat "$FAL/.rev" 2>/dev/null)" != "$FPA_COMMIT" ]; then
    rm -rf "$FAL"; mkdir -p "$FAL/obj"
    m68k-amigaos-gcc -m68040 -DFPUAUTO_COMMON -c "$FA/fpuauto.S" -o "$FAL/obj/fa_common.o"
    for n in $(sed 's/^--wrap=//' "$FA/fpuauto.wraps"); do
        m68k-amigaos-gcc -m68040 -DR_$n -c "$FA/fpuauto.S" -o "$FAL/obj/fa_$n.o"
    done
    m68k-amigaos-ar rcs "$FAL/libfpuauto.a" "$FAL"/obj/fa_*.o
    echo "$FPA_COMMIT" > "$FAL/.rev"
fi

# --- FreeFileSync at its pinned commit, with patches/ -------------------------
SRC="$WORK/FreeFileSync"
PSUM=$(cat "$HERE"/patches/*.patch | sha256sum | cut -c1-16)
if [ "$(cat "$SRC/.patched" 2>/dev/null)" != "$FFS_COMMIT-$PSUM" ]; then
    rm -rf "$SRC"; mkdir -p "$SRC"
    git -C "$SRC" init -q
    git -C "$SRC" fetch -q --depth 1 "$FFS_GIT" "$FFS_COMMIT"
    git -C "$SRC" checkout -q FETCH_HEAD
    [ "$(git -C "$SRC" rev-parse HEAD)" = "$FFS_COMMIT" ] || { echo "commit mismatch"; exit 1; }
    for p in "$HERE"/patches/*.patch; do
        patch -s -d "$SRC" -p1 < "$p" || { echo "patch failed: $p"; exit 1; }
    done
    echo "$FFS_COMMIT-$PSUM" > "$SRC/.patched"
    rm -rf "$WORK/obj"
fi

# --- one Makefile: FreeFileSync's sources (its own Makefile's list, less the
# network and Linux-only files src/ replaces) and ours ------------------------
FS="$SRC/FreeFileSync/Source"
# from FreeFileSync/Source/Makefile, as given there
UP=$(sed -n 's/^cppFiles+=//p' "$FS/Makefile")
# not built: network (src/net_none.cpp says why) and Linux-only (src/*_amiga.cpp)
SKIP="afs/ftp.cpp afs/gdrive.cpp afs/sftp.cpp afs/init_curl_libssh2.cpp ../../libcurl/curl_wrap.cpp
      ../../zen/http.cpp ../../zen/open_ssl.cpp ui/version_check.cpp base/icon_loader.cpp
      ../../zen/sys_info.cpp ../../zen/sys_version.cpp ../../zen/process_exec.cpp
      ../../zen/recycler.cpp ../../zen/process_priority.cpp"
OURS="net_none sys_info_amiga sys_version_amiga process_exec_amiga recycler_amiga
      process_priority_amiga icon_loader_amiga version_check_amiga taskbar_amiga"
LATE=""
if m68k-amigaos-gcc -fno-late-combine-instructions -S -o /dev/null -x c /dev/null 2>/dev/null; then LATE="-fno-late-combine-instructions"; fi
CF="-noixemul -m68040 -O2 -fno-delete-null-pointer-checks $LATE -DNDEBUG"
O="$WORK/obj"; mkdir -p "$O"
M="$O/Makefile"
{
    echo "CF = $CF -I$HERE/compat/include"
    echo "CXXF = $CF -std=gnu++23 -I$HERE/compat/include $("$WXC" --cxxflags) -I$SRC -I$SRC/zenXml -I$ZLIBI -include gfffs_amiga.h -include gfffs_wx.h -DWXINTL_NO_GETTEXT_MACRO -include zen/i18n.h -fmax-errors=30 -Wno-deprecated-declarations"
    A=""
    for f in $UP; do
        case " $(echo $SKIP) " in *" $f "*) continue ;; esac
        o="ffs_$(echo "$f" | sed 's|^\.\./\.\./||; s|/|_|g; s|\.cpp$||').o"; A="$A $o"
        printf '%s: %s\n\tm68k-amigaos-g++ $(CXXF) -MMD -c $< -o $@\n' "$o" "$FS/$f"
    done
    for f in $OURS; do
        A="$A our_$f.o"
        printf 'our_%s.o: %s\n\tm68k-amigaos-g++ $(CXXF) -I%s -MMD -c $< -o $@\n' "$f" "$HERE/src/$f.cpp" "$FS"
    done
    for f in amiga_glib amiga_posix amiga_stack; do
        A="$A c_$f.o"
        printf 'c_%s.o: %s\n\tm68k-amigaos-gcc $(CF) -std=gnu17 -MMD -c $< -o $@\n' "$f" "$HERE/src/$f.c"
    done
    echo "APPOBJS =$A"
    echo "all: \$(APPOBJS)"
    echo "-include *.d"
} > "$M"
left=$(( ${BUDGET_TOTAL:-840} - ($(date +%s) - t0) ))
[ "$left" -gt 60 ] || { echo "no time left for FreeFileSync this run"; exit 0; }
set +e
(cd "$O" && timeout "$left" nice -n 19 make -k -j"$JOBS" all > make.log 2>&1)
rc=$?
set -e
echo "objects: $(ls "$O"/*.o 2>/dev/null | wc -l) of $(sed -n 's/^APPOBJS =//p' "$M" | wc -w)"
grep -E ': (fatal )?error|Error [0-9]' "$O/make.log" | sort | uniq | head -150
if [ "$rc" != 0 ]; then
    for f in $(sed -n 's/^APPOBJS =//p' "$M"); do [ -f "$O/$f" ] || echo "missing: $f"; done
    echo "--- make.log tail"; tail -n 20 "$O/make.log"
fi
[ "$rc" = 0 ] || { [ "$rc" = 124 ] && echo "out of time: run again to go on"; echo "compile: $(( $(date +%s) - t0 )) s"; exit "$rc"; }

# --- the link: soft-float specs, then fpuauto's ------------------------------
# shellcheck disable=SC2046
m68k-amigaos-g++ -noixemul -m68040 -specs="$HERE/scripts/m68k-soft-m68040.specs" -specs="$FA/fpuauto.specs" -L"$FAL" \
    -o "$O/FreeFileSync.debug" $(cd "$O" && for f in $(sed -n 's/^APPOBJS =//p' Makefile); do echo "$O/$f"; done) \
    $("$WXC" --libs std,aui) > "$O/link.log" 2>&1 || { grep -E 'undefined reference|error' "$O/link.log" | sed 's/.*undefined reference to/undefined:/' | sort | uniq -c | sort -rn | head -120; exit 1; }

# --- the drawer: the program and its Resources/ ------------------------------
D="$OUT/FreeFileSync"; rm -rf "$D"; mkdir -p "$D/Resources"
m68k-amigaos-strip -o "$D/FreeFileSync" "$O/FreeFileSync.debug"
for r in Icons.zip Languages.zip bell.wav bell2.wav ding.wav fail.wav fail2.wav gong.wav harp.wav notify.wav notify2.wav remind.wav; do
    [ -f "$SRC/FreeFileSync/Build/Resources/$r" ] && cp "$SRC/FreeFileSync/Build/Resources/$r" "$D/Resources/"
done
ls -l "$D" "$D/Resources"
echo "fpuauto: $(m68k-amigaos-nm "$O/FreeFileSync.debug" | grep -c ' T ___wrap_') wrapped routines"
# a lab hang: debug/find.txt holds code bytes from the runtime's profile
if [ -s "$HERE/debug/find.txt" ]; then
    m68k-amigaos-nm -n -C "$O/FreeFileSync.debug" > "$O/nm.txt"
    python3 "$HERE/scripts/whereis.py" "$O/FreeFileSync.debug" "$O/nm.txt" $(grep -v '^#' "$HERE/debug/find.txt")
fi
echo "total: $(( $(date +%s) - t0 )) s"
