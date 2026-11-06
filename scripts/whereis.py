#!/usr/bin/env python3
# whereis.py HUNKFILE NMFILE PATTERN... - for a lab hang: where in the
# program a run of code bytes (hex, as the runtime's profile prints a
# guest block) is, as "symbol+offset". The program's code is its first
# hunk; nm gives the symbols' offsets in it.
import bisect, struct, sys

data = open(sys.argv[1], 'rb').read()
def L(o): return struct.unpack('>I', data[o:o+4])[0]
assert L(0) == 0x3F3, 'not a hunk file'
o = 4
while L(o): o += 4 + 4 * L(o)
o += 4
n, first, last = L(o), L(o+4), L(o+8)
o += 12 + 4 * (last - first + 1)
while L(o) & 0x3FFFFFFF not in (0x3E9,): o += 4
size = L(o+4) * 4
code0 = o + 8
syms = []
for line in open(sys.argv[2]):
    p = line.split(None, 2)
    if len(p) == 3 and p[1] in 'tTwW':
        syms.append((int(p[0], 16), p[2].strip()))
syms.sort()
keys = [s[0] for s in syms]
for pat in sys.argv[3:]:
    b = bytes.fromhex(pat)
    i, hits = code0, 0
    while hits < 8:
        i = data.find(b, i, code0 + size)
        if i < 0:
            break
        off = i - code0
        j = bisect.bisect_right(keys, off) - 1
        name = syms[j][1] if j >= 0 else '?'
        print(f'{pat[:16]}: code+0x{off:x} = {name}+0x{off - (syms[j][0] if j >= 0 else 0):x}')
        hits += 1
        i += 2
    if not hits:
        print(f'{pat[:16]}: not in the code')
