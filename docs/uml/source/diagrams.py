from seqsvg import Seq, tw, FONT
from html import escape
import math

GREEN = ("#E6F4EA", "#2E6B3F"); YELLOW = ("#FFF8E1", "#8A6A1F"); BLUE = ("#E3EEFA", "#2B5C8A"); PURPLE=("#EEE8F8","#5B3F8C")

# ---------------------------------------------------------------- Iterator sequence
s = Seq("Iterator Pattern - Sequence: rendering Kafi's news feed",
        [("main", "main()", "«Client code»", *GREEN),
         ("screen", "screen : FeedScreen", "«Client»", *GREEN),
         ("net", "network : CampusConnect", "«ConcreteAggregate»", *YELLOW),
         ("it", "feed : NewsFeedIterator", "«ConcreteIterator»", *YELLOW)],
        subtitle="CampusConnect (Facebook-like) - CSE 3206 Lab 3, Group 7")
s.call("main", "net", 'createNewsFeedIterator("kafi")')
s.create("net", "it", 'NewsFeedIterator(*this, "kafi")')
s.call("it", "net", 'friendIdsOf("kafi"), posts()')
s.self_call("it", "collect(): keep me + friends, sort newest first")
s.ret("net", "main", "unique_ptr<PostIterator>")
s.call("main", "screen", "showPosts(title, *feed)")
s.frame("loop", "[ while feed.hasNext() ]", "screen", "it")
s.call("screen", "it", "hasNext()")
s.ret("it", "screen", "true")
s.call("screen", "it", "next()")
s.self_call("it", "ensureNotModified()  (fail-fast check)")
s.call("it", "net", "posts().at(index)")
s.ret("it", "screen", "const Post&")
s.self_call("screen", 'print  "[t=6] Abdullah Al Kafi: ..."')
s.end()
s.call("screen", "it", "hasNext()")
s.ret("it", "screen", "false")
s.ret("screen", "main", "4  (posts shown)")
s.note("screen", "it", "FeedScreen talks only to the Iterator<Post> interface.\nIt never touches the vector, the map or the friendship graph.")
open("iterator_sequence.svg", "w").write(s.render())

# ---------------------------------------------------------------- Mediator sequence
m = Seq("Mediator Pattern - Sequence: offline delivery and a group message",
        [("kafi", "kafi : Member", "«ConcreteColleague»", *YELLOW),
         ("server", "server : MessengerServer", "«ConcreteMediator»", *BLUE),
         ("sakila", "sakila : Member", "«ConcreteColleague»", *YELLOW),
         ("sadaf", "sadaf : Member", "«ConcreteColleague»", *YELLOW)],
        subtitle="CampusConnect Messenger (Messenger-like) - CSE 3206 Lab 3, Group 7")
m.divider("Scenario A - direct message to an offline user")
m.self_call("sakila", "goOffline(): online_ = false")
m.call("sakila", "server", "presenceChanged(sakila)")
m.call("kafi", "server", 'sendDirect(kafi, "sakila", text)')
m.self_call("server", "not blocked -> makeMessage(), history_.push_back()")
m.self_call("server", 'deliver(): offline -> pending_["sakila"].push_back()')
m.ret("server", "kafi", "DeliveryStatus::Queued")
m.self_call("sakila", "goOnline(): online_ = true")
m.call("sakila", "server", "presenceChanged(sakila)")
m.call("server", "sakila", "receive(msg)   [flush queue]")
m.divider("Scenario B - group message to #Group-7")
m.call("sadaf", "server", 'sendToGroup(sadaf, "Group-7", text)')
m.self_call("server", "member? else throw PermissionDenied")
m.frame("loop", "[ each member != sender and not blocking sender ]", "kafi", "sakila")
m.call("server", "kafi", "receive(msg)")
m.call("server", "sakila", "receive(msg)")
m.end()
m.ret("server", "sadaf", "2  (members reached)")
m.note("kafi", "sadaf", "Kafi, Sakila and Sadaf never call each other:\nevery arrow starts or ends at the MessengerServer (the mediator).")
open("mediator_sequence.svg", "w").write(m.render())

# ---------------------------------------------------------------- helpers for motivation diagrams
def box(x, y, w, h, title, sub=None, fill="#FFF8E1", stroke="#8A6A1F", bold=True, size=12):
    t = [f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="6" fill="{fill}" stroke="{stroke}" stroke-width="1.4"/>']
    if sub:
        t.append(f'<text x="{x+w/2}" y="{y+h/2-3}" text-anchor="middle" font-size="{size}" font-weight="{"bold" if bold else "normal"}">{escape(title)}</text>')
        t.append(f'<text x="{x+w/2}" y="{y+h/2+13}" text-anchor="middle" font-size="10" fill="#555">{escape(sub)}</text>')
    else:
        t.append(f'<text x="{x+w/2}" y="{y+h/2+4}" text-anchor="middle" font-size="{size}" font-weight="{"bold" if bold else "normal"}">{escape(title)}</text>')
    return "".join(t)

def line(x1, y1, x2, y2, color="#444", dash=None, arrow=True, width=1.4):
    d = f' stroke-dasharray="{dash}"' if dash else ""
    m = ' marker-end="url(#arr)"' if arrow else ""
    return f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" stroke="{color}" stroke-width="{width}"{d}{m}/>'

def text(x, y, s, size=11, anchor="middle", fill="#333", weight="normal"):
    return f'<text x="{x}" y="{y}" text-anchor="{anchor}" font-size="{size}" fill="{fill}" font-weight="{weight}">{escape(s)}</text>'

def svg(w, h, body):
    defs = ('<defs><marker id="arr" markerWidth="10" markerHeight="8" refX="9" refY="4" orient="auto">'
            '<path d="M0,0 L10,4 L0,8 z" fill="#444"/></marker>'
            '<marker id="tri" markerWidth="14" markerHeight="12" refX="13" refY="6" orient="auto">'
            '<path d="M0,0 L13,6 L0,12 z" fill="white" stroke="#444"/></marker></defs>')
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}" font-family="{FONT}">'
            f'<rect width="100%" height="100%" fill="white"/>{defs}{body}</svg>')

def panel(x, y, w, h, title, good):
    col = "#2E6B3F" if good else "#A33A3A"
    bg = "#F4FAF6" if good else "#FCF4F4"
    return (f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="10" fill="{bg}" stroke="{col}" stroke-width="1.5"/>'
            + text(x + w/2, y + 26, title, 14, fill=col, weight="bold"))

# ---------------------------------------------------------------- Mediator motivation
W, H = 1100, 520
b = [text(W/2, 30, "Why Mediator? Direct coupling vs. a central mediator", 16, weight="bold")]
b.append(panel(20, 50, 520, 450, "WITHOUT Mediator: every user knows every user", False))
b.append(panel(560, 50, 520, 450, "WITH Mediator: users know only the server", True))
names = ["Kafi", "Sadaf", "Sakila", "Rafi", "HelpBot"]
def ring(cx, cy, r):
    return [(cx + r*math.cos(-math.pi/2 + 2*math.pi*i/5), cy + r*math.sin(-math.pi/2 + 2*math.pi*i/5)) for i in range(5)]
pts = ring(280, 265, 140)
for i in range(5):
    for j in range(i+1, 5):
        b.append(line(*pts[i], *pts[j], color="#C06060", arrow=False, width=1.6))
for (px, py), n in zip(pts, names):
    b.append(box(px-45, py-18, 90, 36, n, fill="#FFF8E1", stroke="#8A6A1F"))
b.append(text(280, 445, "5 users = 10 links  (n(n-1)/2;  100 users = 4,950 links)", 12, fill="#A33A3A", weight="bold"))
b.append(text(280, 467, "Block lists, offline queues and group logic must be coded inside EVERY", 11))
b.append(text(280, 483, "user class; adding one user type means editing all the others.", 11))
pts2 = ring(820, 265, 150)
for (px, py) in pts2:
    b.append(line(px, py, 820, 265, color="#2E6B3F", arrow=False, width=2))
b.append(box(750, 240, 140, 50, "MessengerServer", "«ConcreteMediator»", fill="#E3EEFA", stroke="#2B5C8A"))
for (px, py), n in zip(pts2, names):
    b.append(box(px-45, py-18, 90, 36, n, fill="#FFF8E1", stroke="#8A6A1F"))
b.append(text(820, 445, "5 users = 5 links  (n links;  100 users = 100 links)", 12, fill="#2E6B3F", weight="bold"))
b.append(text(820, 467, "All routing / group / block / offline rules live in ONE class.", 11))
b.append(text(820, 483, "New user types (e.g. a bot) plug in without touching anyone else.", 11))
open("mediator_motivation.svg", "w").write(svg(W, H, "".join(b)))

# ---------------------------------------------------------------- Iterator motivation
W, H = 1100, 500
b = [text(W/2, 30, "Why Iterator? Client coupled to storage vs. one traversal interface", 16, weight="bold")]
b.append(panel(20, 50, 520, 430, "WITHOUT Iterator: the UI walks raw containers", False))
b.append(panel(560, 50, 520, 430, "WITH Iterator: the UI walks one interface", True))
# left
b.append(box(180, 95, 200, 46, "FeedScreen (UI)", "contains 3 different loops", fill="#E6F4EA", stroke="#2E6B3F"))
store = [("std::map<id, Profile>", "for (auto& [id, p] : profiles)"),
         ("friendship graph", "BFS for 'People You May Know'"),
         ("std::vector<Post>", "filter + sort by time, by hand")]
for k, (t1, t2) in enumerate(store):
    x = 45 + k*165
    b.append(line(280, 141, x+75, 262, color="#A33A3A"))
    b.append(box(x, 265, 150, 50, t1, None, fill="#F2F2F2", stroke="#666", size=11))
    b.append(text(x+75, 335, t2, 10, fill="#7a2a2a"))
b.append(text(280, 385, "UI depends on every internal data structure.", 11.5, fill="#A33A3A", weight="bold"))
b.append(text(280, 405, "Change vector -> database, or add a new traversal", 11))
b.append(text(280, 421, "(e.g. 'Top posts'), and the UI code must be edited.", 11))
b.append(text(280, 441, "Two scrolls at once? Each loop keeps its own messy index.", 11))
# right
b.append(box(720, 95, 200, 46, "FeedScreen (UI)", "one loop: while (it.hasNext())", fill="#E6F4EA", stroke="#2E6B3F"))
b.append(line(820, 141, 820, 176))
b.append(box(725, 178, 190, 46, "Iterator<T>", "hasNext()  next()  reset()", fill="#E3EEFA", stroke="#2B5C8A"))
its = ["FriendsIterator", "SuggestionsIterator", "NewsFeedIterator"]
for k, n in enumerate(its):
    x = 585 + k*165
    b.append(f'<line x1="{x+75}" y1="268" x2="{820 + (k-1)*40}" y2="226" stroke="#444" stroke-width="1.3" stroke-dasharray="5,3" marker-end="url(#tri)"/>')
    b.append(box(x, 270, 150, 40, n, None, fill="#FFF8E1", stroke="#8A6A1F", size=11))
    b.append(line(x+75, 310, 820 + (k-1)*30, 338, color="#8A6A1F", dash="4,3"))
b.append(box(700, 340, 240, 36, "CampusConnect (storage hidden)", None, fill="#FFF8E1", stroke="#8A6A1F", size=11))
b.append(text(820, 403, "UI depends only on Iterator<T>.", 11.5, fill="#2E6B3F", weight="bold"))
b.append(text(820, 423, "New traversal = new class; storage can change freely.", 11))
b.append(text(820, 441, "Many independent iterators can run at the same time.", 11))
open("iterator_motivation.svg", "w").write(svg(W, H, "".join(b)))
print("svgs written")
