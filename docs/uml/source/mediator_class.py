from umlgen import cls, HEADER
d = HEADER.replace("nodesep=0.55", "nodesep=0.7")
d += '  label=<<B>Mediator Pattern &#8212; CampusConnect Messenger (Messenger-like chat)</B><BR/><FONT POINT-SIZE="9">CSE 3206 Lab 3 &#183; Group 7</FONT>>; labelloc=t;\n'
d += cls("ChatMediator", "interface", "Mediator", [],
         ["+ registerUser(user : User&) : void",
          "+ unregisterUser(userId) : void",
          "+ sendDirect(from, toId, text) : DeliveryStatus",
          "+ sendToGroup(from, groupId, text) : size_t",
          "+ blockUser(blocker, blockedId) : void",
          "+ presenceChanged(user) : void"])
d += cls("User", "abstract", "Colleague",
         ["- id_ : string", "- name_ : string", "- online_ : bool",
          "- mediator_ : ChatMediator*"],
         ["+ join(mediator : ChatMediator&) : void",
          "+ leave() : void",
          "+ sendTo(toId, text) : DeliveryStatus",
          "+ sendToGroup(groupId, text) : size_t",
          "+ block(userId) : void",
          "+ goOnline() / goOffline() : void",
          "+ isAutomated() : bool  {virtual}",
          "+ receive(message : const Message&)  {abstract}"])
d += cls("MessengerServer", "concrete", "ConcreteMediator",
         ["- users_ : map<string, User*>",
          "- groups_ : map<string, vector<string>>",
          "- blocked_ : map<string, set<string>>",
          "- pending_ : map<string, vector<Message>>",
          "- history_ : vector<Message>"],
         ["+ registerUser(user) / unregisterUser(id)",
          "+ sendDirect(from, toId, text) : DeliveryStatus",
          "+ sendToGroup(from, groupId, text) : size_t",
          "+ blockUser(blocker, blockedId) : void",
          "+ presenceChanged(user) : void",
          "+ createGroup(groupId, memberIds) : void",
          "+ pendingCount(userId) : size_t",
          "+ history() : const vector<Message>&",
          "- deliver(recipient, message) : DeliveryStatus",
          "- isBlocked(recipientId, senderId) : bool"])
d += cls("Member", "concrete", "ConcreteColleague",
         ["- out_ : std::ostream&", "- inbox_ : vector<Message>"],
         ["+ receive(message) : void", "+ inbox() : const vector<Message>&"])
d += cls("SupportBot", "concrete", "ConcreteColleague",
         ["- out_ : std::ostream&", "- ticketsOpened_ : int"],
         ["+ receive(message) : void  (auto-reply)", "+ isAutomated() : bool  = true", "+ ticketsOpened() : int"])
d += cls("Message", "data", "",
         ["+ id : int", "+ fromId, fromName : string", "+ toId : string",
          "+ groupId : string  (empty = direct)", "+ text : string", "+ automated : bool"],
         ["+ isGroupMessage() : bool"])
enum = '''  "DeliveryStatus" [label=<<TABLE BORDER="1" CELLBORDER="0" CELLSPACING="0" CELLPADDING="3" BGCOLOR="#F2F2F2" COLOR="#666666"><TR><TD><FONT POINT-SIZE="9" COLOR="#666666">&laquo;enumeration&raquo;</FONT></TD></TR><TR><TD><B>DeliveryStatus</B></TD></TR><HR/><TR><TD ALIGN="LEFT">Delivered</TD></TR><TR><TD ALIGN="LEFT">Queued</TD></TR><TR><TD ALIGN="LEFT">Blocked</TD></TR></TABLE>>];
'''
d += enum
d += '''  "note" [shape=note, style=filled, fillcolor="#FFFDE7", color="#B59F3B", fontsize=9, label="Colleagues NEVER reference each other.\\nMember and SupportBot talk only to\\nChatMediator; MessengerServer owns all\\nrouting, group, block and offline rules."];

  { rank=same; "User"; "ChatMediator"; }
  { rank=same; "Member"; "SupportBot"; "MessengerServer"; }
  "Member" -> "SupportBot" [style=invis];

  // generalization / realization (arrow points to the parent)
  "User" -> "Member"     [dir=back, arrowtail=empty];
  "User" -> "SupportBot" [dir=back, arrowtail=empty];
  "ChatMediator" -> "MessengerServer" [dir=back, arrowtail=empty, style=dashed];

  // Colleague knows its Mediator (1)
  "User" -> "ChatMediator" [arrowhead=vee, label="  mediator_", headlabel="1", labeldistance=2.0];

  // ConcreteMediator knows all colleagues (aggregation, non-owning)
  "MessengerServer" -> "User" [dir=back, arrowtail=odiamond, arrowhead=vee, dir=both, label="users_", headlabel="0..*", labeldistance=2.0, constraint=false];

  // data used in the conversation
  "MessengerServer" -> "Message" [style=dashed, arrowhead=vee, label=" creates / stores"];
  "MessengerServer" -> "DeliveryStatus" [style=dashed, arrowhead=vee, label=" returns"];
  "Member" -> "note"     [style=dashed, arrowhead=none, color="#B59F3B"];
  "SupportBot" -> "note" [style=dashed, arrowhead=none, color="#B59F3B"];
  { rank=same; "note"; "DeliveryStatus"; "Message"; }
  "note" -> "DeliveryStatus" -> "Message" [style=invis];
}
'''
open("mediator_class.dot","w").write(d)
