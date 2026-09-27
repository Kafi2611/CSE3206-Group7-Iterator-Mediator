"""Tiny helper that writes UML class diagrams as Graphviz DOT (HTML labels)."""
import html

STYLE = {
    "interface": ("#E3EEFA", "#2B5C8A"),
    "abstract":  ("#EEE8F8", "#5B3F8C"),
    "concrete":  ("#FFF8E1", "#8A6A1F"),
    "client":    ("#E6F4EA", "#2E6B3F"),
    "data":      ("#F2F2F2", "#666666"),
}

def cls(name, kind, role, attrs, methods, italic=False, width=None):
    fill, line = STYLE[kind]
    esc = html.escape
    rows = []
    stereo = []
    if kind == "interface": stereo.append("interface")
    if kind == "abstract": stereo.append("abstract")
    if kind == "data": stereo.append("struct")
    if role: stereo.append(role)
    st = " ".join(f"&laquo;{s}&raquo;" for s in stereo)
    title = f"<I>{esc(name)}</I>" if (italic or kind in ("interface","abstract")) else esc(name)
    rows.append(f'<TR><TD ALIGN="CENTER"><FONT POINT-SIZE="9" COLOR="{line}">{st}</FONT></TD></TR>' if st else "")
    rows.append(f'<TR><TD ALIGN="CENTER"><B>{title}</B></TD></TR>')
    rows.append("<HR/>")
    if attrs:
        for a in attrs:
            rows.append(f'<TR><TD ALIGN="LEFT">{fmt(a)}</TD></TR>')
    else:
        rows.append('<TR><TD> </TD></TR>')
    rows.append("<HR/>")
    if methods:
        for m in methods:
            rows.append(f'<TR><TD ALIGN="LEFT">{fmt(m)}</TD></TR>')
    else:
        rows.append('<TR><TD> </TD></TR>')
    w = f' WIDTH="{width}"' if width else ""
    table = (f'<TABLE BORDER="1" CELLBORDER="0" CELLSPACING="0" CELLPADDING="3" '
             f'BGCOLOR="{fill}" COLOR="{line}"{w}>' + "".join(rows) + "</TABLE>")
    return f'  "{name}" [label=<{table}>];\n'

def fmt(s):
    """'*text*' -> italic (abstract member)."""
    s = html.escape(s)
    if s.startswith("*") and s.endswith("*"):
        return f"<I>{s[1:-1]}</I>"
    return s

HEADER = '''digraph G {
  graph [rankdir=TB, splines=true, nodesep=0.55, ranksep=0.75, fontname="DejaVu Sans", fontsize=11, pad=0.3, bgcolor="white"];
  node  [shape=plaintext, fontname="DejaVu Sans", fontsize=10];
  edge  [fontname="DejaVu Sans", fontsize=9, color="#444444"];
'''
