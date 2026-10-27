"""Запуск: python glaze/tools/subset_icons.py "<Семейство>" <папка шрифтов> \\
           <JetBrainsMonoNerdFont-Regular.ttf> <JetBrainsMonoNerdFontPropo-Regular.ttf> <папка qml>...
Например, из корня Jot:
    python glaze/tools/subset_icons.py "Jot Icons" fonts NF.ttf NFPropo.ttf qml glaze/qml
(нужен fontTools: pip install fonttools)

Значки интерфейса — символы Nerd Font (\\u{f…} в QML). Чтобы приложение не зависело от
установленного шрифта, из JetBrainsMono Nerd Font вырезаются только те символы, что
встречаются в его QML и в контролах Glaze, и вшиваются под своим семейством
(«Jot Icons» → JotIcons.ttf и JotIconsPropo.ttf). Метрики и контуры те же, что у
Nerd Font: значки не сдвигаются ни на пиксель.
Обычный вариант — для стекла, Propo — для Material."""
import glob
import os
import re
import sys

from fontTools import subset
from fontTools.ttLib import TTFont

def used_codepoints(dirs):
    cps = set()
    for f in (f for d in dirs for f in glob.glob(os.path.join(d, "*.qml"))):
        for m in re.finditer(r"\\u\{([0-9a-fA-F]+)\}", open(f, encoding="utf-8").read()):
            cps.add(int(m.group(1), 16))
    return sorted(cps)


def rename(font, family):
    # 1/16 — семейство, 4 — полное имя, 6 — PostScript
    ps = family.replace(" ", "")
    for rec in font["name"].names:
        if rec.nameID in (1, 16):
            rec.string = family
        elif rec.nameID == 4:
            rec.string = family + " Regular"
        elif rec.nameID == 6:
            rec.string = ps + "-Regular"
        elif rec.nameID == 3:
            rec.string = ps + ";subset"


def build(src, family, out, cps):
    font = TTFont(src)
    opts = subset.Options()
    opts.layout_features = []
    opts.name_IDs = ["*"]
    opts.name_languages = ["*"]
    opts.notdef_outline = True
    # Хинтинг оставляем: без него значки растеризуются иначе, чем раньше
    opts.hinting = True
    sub = subset.Subsetter(opts)
    sub.populate(unicodes=cps)
    sub.subset(font)
    rename(font, family)
    font.save(out)
    missing = [c for c in cps if c not in font.getBestCmap()]
    print(f"{out}: {len(cps) - len(missing)} символов, {os.path.getsize(out)} байт")
    if missing:
        sys.exit("нет в шрифте: " + " ".join(f"{c:x}" for c in missing))


if len(sys.argv) < 6:
    sys.exit(__doc__)
family, out, regular, propo, *qml_dirs = sys.argv[1:]
cps = used_codepoints(qml_dirs)
name = family.replace(" ", "")
os.makedirs(out, exist_ok=True)
build(regular, family, os.path.join(out, name + ".ttf"), cps)
build(propo, family + " Propo", os.path.join(out, name + "Propo.ttf"), cps)
