#!/usr/bin/env python3
"""Unit test of tools/lha_archive.py's "auto" method: members whose names end
in an already-compressed format (PNG, Ogg, ...) are stored (-lh0-) without an
lh5 try, even when lh5 would shrink them; others are lh5 when it helps; every
member reads back unchanged. Copyright (c) 2026 AmiCradle Team."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
import lha_archive as L

members = [
    L.Member("D/title.png", bytes(200000)),       # zeros: lh5 would shrink it
    L.Member("D/music/Theme.OGG", bytes(300000)),
    L.Member("D/level.map", b"tile " * 20000),
    L.Member("D/empty.png", b""),
]
archive = L.pack(members)
back = L.read(archive)
assert [(m.path, m.data) for m in back] == [(m.path, m.data) for m in members], "round trip"
want = {"D/title.png": b"-lh0-", "D/music/Theme.OGG": b"-lh0-", "D/level.map": b"-lh5-", "D/empty.png": b"-lh0-"}
for m in members:
    i = archive.index(m.path.encode())
    meth = archive[archive.rfind(b"-lh", 0, i):][:5]
    assert meth == want[m.path], f"{m.path}: {meth} (want {want[m.path]})"
print(f"lha: PNG and Ogg stored, the rest lh5, {len(members)} members read back ({len(archive)} bytes)")
