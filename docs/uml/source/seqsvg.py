"""Minimal UML sequence-diagram -> SVG generator (no external deps)."""
from html import escape

FONT = "DejaVu Sans, Arial, sans-serif"
CHAR_W = 6.6   # approx width of one char at 11.5px


def tw(text, size=11.5):
    return len(text) * CHAR_W * size / 11.5


class Seq:
    def __init__(self, title, participants, col_gap=60, subtitle=None):
        """participants: list of (key, name, stereotype, fill, stroke)"""
        self.title = title
        self.subtitle = subtitle
        self.parts = participants
        self.items = []
        self.col_gap = col_gap

    # ---- API ---------------------------------------------------------------
    def call(self, a, b, text):   self.items.append(("call", a, b, text))
    def ret(self, a, b, text):    self.items.append(("ret", a, b, text))
    def create(self, a, b, text): self.items.append(("create", a, b, text))
    def self_call(self, a, text): self.items.append(("self", a, None, text))
    def note(self, a, b, text):   self.items.append(("note", a, b, text))
    def divider(self, text):      self.items.append(("div", None, None, text))
    def frame(self, kind, label, a, b): self.items.append(("fstart", a, b, (kind, label)))
    def end(self):                self.items.append(("fend", None, None, None))

    # ---- layout + render ---------------------------------------------------
    def render(self):
        # column x positions: wide enough for header boxes and message labels
        widths = [max(tw(p[1], 12.5), tw(p[2], 10)) + 30 for p in self.parts]
        idx = {p[0]: i for i, p in enumerate(self.parts)}
        gaps = [0.0] * (len(self.parts) - 1)
        for kind, a, b, text in self.items:
            if kind in ("call", "ret", "create") and a != b:
                i, j = sorted((idx[a], idx[b]))
                need = tw(text) + 30
                span = j - i
                for k in range(i, j):
                    gaps[k] = max(gaps[k], need / span)
        xs = []
        x = 30 + widths[0] / 2
        for i, w in enumerate(widths):
            if i == 0:
                xs.append(x)
                continue
            dist = max((widths[i - 1] + w) / 2 + self.col_gap, gaps[i - 1])
            x += dist
            xs.append(x)
        # extra width for self-call labels at the rightmost columns
        right_extra = 0
        for kind, a, b, text in self.items:
            if kind == "self":
                right_extra = max(right_extra, xs[idx[a]] + 40 + tw(text) - xs[-1])
            if kind == "note":
                pass
        width = xs[-1] + max(widths[-1] / 2, right_extra) + 40

        out = []
        y = 28
        out.append(f'<text x="{width/2:.0f}" y="{y}" text-anchor="middle" font-size="15" font-weight="bold">{escape(self.title)}</text>')
        if self.subtitle:
            y += 18
            out.append(f'<text x="{width/2:.0f}" y="{y}" text-anchor="middle" font-size="11" fill="#555">{escape(self.subtitle)}</text>')
        y += 22
        head_top = y
        head_h = 44
        for (key, name, st, fill, stroke), x0, w in zip(self.parts, xs, widths):
            out.append(f'<rect x="{x0-w/2:.1f}" y="{head_top}" width="{w:.1f}" height="{head_h}" rx="4" fill="{fill}" stroke="{stroke}" stroke-width="1.3"/>')
            out.append(f'<text x="{x0:.1f}" y="{head_top+16}" text-anchor="middle" font-size="10" fill="{stroke}">{escape(st)}</text>')
            out.append(f'<text x="{x0:.1f}" y="{head_top+33}" text-anchor="middle" font-size="12.5" font-weight="bold" text-decoration="underline">{escape(name)}</text>')
        y = head_top + head_h + 26
        body = []
        frames = []
        for kind, a, b, text in self.items:
            if kind in ("call", "ret", "create"):
                x1, x2 = xs[idx[a]], xs[idx[b]]
                dash = ' stroke-dasharray="6,4"' if kind in ("ret", "create") else ""
                marker = "open" if kind in ("ret", "create") else "filled"
                d = 1 if x2 > x1 else -1
                body.append(f'<line x1="{x1:.1f}" y1="{y}" x2="{x2 - d*2:.1f}" y2="{y}" stroke="#222" stroke-width="1.2"{dash} marker-end="url(#{marker})"/>')
                label = text if kind != "create" else "«create» " + text
                body.append(f'<text x="{(x1+x2)/2:.1f}" y="{y-6}" text-anchor="middle" font-size="11.5" stroke="white" stroke-width="5" stroke-linejoin="round" paint-order="stroke">{escape(label)}</text>')
                y += 34
            elif kind == "self":
                x1 = xs[idx[a]]
                body.append(f'<path d="M{x1:.1f},{y-8} h34 v18 h-32" fill="none" stroke="#222" stroke-width="1.2" marker-end="url(#filled)"/>')
                body.append(f'<text x="{x1+40:.1f}" y="{y+5}" font-size="11.5" stroke="white" stroke-width="5" stroke-linejoin="round" paint-order="stroke">{escape(text)}</text>')
                y += 36
            elif kind == "note":
                i, j = sorted((idx[a], idx[b if b else a]))
                lines = text.split("\n")
                h = 10 + 15 * len(lines)
                x_left = xs[i] - 70 if i == j else xs[i] - 40
                x_right = xs[j] + 70 if i == j else xs[j] + 40
                x_left = max(x_left, 10)
                need = max(tw(line, 11) for line in lines) + 24
                if x_right - x_left < need:
                    mid = (x_left + x_right) / 2
                    x_left, x_right = mid - need / 2, mid + need / 2
                body.append(f'<path d="M{x_left:.1f},{y-12} h{x_right-x_left-10:.1f} l10,10 v{h-10} h-{x_right-x_left:.1f} z" fill="#FFFBE6" stroke="#B59F3B" stroke-width="1"/>')
                for n, line in enumerate(lines):
                    body.append(f'<text x="{(x_left+x_right)/2:.1f}" y="{y+4+15*n}" text-anchor="middle" font-size="11" fill="#5a4a00">{escape(line)}</text>')
                y += h + 16
            elif kind == "div":
                body.append(f'<line x1="14" y1="{y-6}" x2="{width-14:.0f}" y2="{y-6}" stroke="#8a8a8a" stroke-width="1" stroke-dasharray="2,3"/>')
                w = tw(text, 11.5) + 24
                body.append(f'<rect x="{width/2-w/2:.1f}" y="{y-17}" width="{w:.1f}" height="22" rx="3" fill="#EEF2F7" stroke="#8a8a8a"/>')
                body.append(f'<text x="{width/2:.1f}" y="{y-2}" text-anchor="middle" font-size="11.5" font-weight="bold" fill="#2B3A4A">{escape(text)}</text>')
                y += 30
            elif kind == "fstart":
                frames.append((a, b, text, y - 20))
                y += 28
            elif kind == "fend":
                a, b, (fkind, label), top = frames.pop()
                i, j = sorted((idx[a], idx[b]))
                x_left = xs[i] - 60
                x_right = min(xs[j] + 170, width - 12)
                bottom = y - 14
                body.insert(0, f'<rect x="{x_left:.1f}" y="{top}" width="{x_right-x_left:.1f}" height="{bottom-top}" fill="#F7F9FC" stroke="#5B6B7F" stroke-width="1"/>')
                tag = f"{fkind} {label}"
                tag_w = tw(fkind, 11) + 16
                body.append(f'<path d="M{x_left:.1f},{top} h{tag_w:.1f} v14 l-6,6 h-{tag_w-6:.1f} z" fill="#DDE5EF" stroke="#5B6B7F"/>')
                body.append(f'<text x="{x_left+6:.1f}" y="{top+14}" font-size="11" font-weight="bold">{escape(fkind)}</text>')
                body.append(f'<text x="{x_left+tag_w+8:.1f}" y="{top+14}" font-size="11" fill="#333">{escape(label)}</text>')
                y += 6
        end_y = y + 4
        for x0 in xs:
            out.append(f'<line x1="{x0:.1f}" y1="{head_top+head_h}" x2="{x0:.1f}" y2="{end_y}" stroke="#9aa5b1" stroke-width="1.2" stroke-dasharray="5,4"/>')
        out.extend(body)
        height = end_y + 24
        defs = ('<defs>'
                '<marker id="filled" markerWidth="10" markerHeight="8" refX="9" refY="4" orient="auto"><path d="M0,0 L10,4 L0,8 z" fill="#222"/></marker>'
                '<marker id="open" markerWidth="10" markerHeight="8" refX="9" refY="4" orient="auto"><path d="M0,0 L10,4 L0,8" fill="none" stroke="#222" stroke-width="1.2"/></marker>'
                '</defs>')
        return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{width:.0f}" height="{height:.0f}" '
                f'viewBox="0 0 {width:.0f} {height:.0f}" font-family="{FONT}">'
                f'<rect width="100%" height="100%" fill="white"/>{defs}' + "".join(out) + "</svg>")
