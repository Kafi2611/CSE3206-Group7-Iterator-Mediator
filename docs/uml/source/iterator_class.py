from umlgen import cls, HEADER
d = HEADER
d += '  label=<<B>Iterator Pattern &#8212; CampusConnect (Facebook-like feed)</B><BR/><FONT POINT-SIZE="9">CSE 3206 Lab 3 &#183; Group 7</FONT>>; labelloc=t;\n'
d += cls("FeedScreen", "client", "Client", ["- out_ : std::ostream&"],
         ["+ showProfiles(title, ProfileIterator&) : size_t",
          "+ showPosts(title, PostIterator&) : size_t",
          "findMutualFriends(network, a, b) : vector<string>"])
d += cls("SocialNetwork", "interface", "Aggregate", [],
         ["+ createFriendsIterator(id) : unique_ptr<ProfileIterator>",
          "+ createSuggestionsIterator(id) : unique_ptr<ProfileIterator>",
          "+ createNewsFeedIterator(id) : unique_ptr<PostIterator>"])
d += cls("Iterator<T>", "interface", "Iterator", [],
         ["+ hasNext() : bool", "+ next() : const T&", "+ reset() : void"])
d += cls("CampusConnect", "concrete", "ConcreteAggregate",
         ["- profiles_ : map<string, Profile>",
          "- friends_ : map<string, set<string>>  (graph)",
          "- posts_ : vector<Post>",
          "- version_ : size_t"],
         ["+ addProfile(profile) : void",
          "+ addFriendship(a, b) : void",
          "+ publishPost(authorId, text, likes) : int",
          "+ profile(id) : const Profile&",
          "+ friendIdsOf(id) : const set<string>&",
          "+ posts() : const vector<Post>&",
          "+ version() : size_t",
          "+ createFriendsIterator(id)",
          "+ createSuggestionsIterator(id)",
          "+ createNewsFeedIterator(id)"])
d += cls("FailFastIterator<T, Key>", "abstract", "",
         ["- network_ : const CampusConnect&",
          "- order_ : vector<Key>",
          "- cursor_ : size_t",
          "- expectedVersion_ : size_t"],
         ["+ hasNext() : bool", "+ next() : const T&", "+ reset() : void",
          "# resolve(key) : const T&  {abstract}", "- ensureNotModified() : void"])
d += cls("FriendsIterator", "concrete", "ConcreteIterator", [],
         ["- collect(network, id) : vector<string>", "# resolve(id) : const Profile&"])
d += cls("SuggestionsIterator", "concrete", "ConcreteIterator", [],
         ["- collect(network, id) : vector<string>", "  (friends-of-friends, BFS depth 2)", "# resolve(id) : const Profile&"])
d += cls("NewsFeedIterator", "concrete", "ConcreteIterator", [],
         ["- collect(network, id) : vector<size_t>", "  (my + friends' posts, newest first)", "# resolve(index) : const Post&"])
d += cls("Profile", "data", "", ["+ id : string", "+ name : string", "+ department : string"], [])
d += cls("Post", "data", "", ["+ id : int", "+ authorId : string", "+ authorName : string", "+ content : string", "+ likes : int", "+ timestamp : long"], [])

d += '''
  { rank=same; "FeedScreen"; "SocialNetwork"; "Iterator<T>"; }
  { rank=same; "CampusConnect"; "FailFastIterator<T, Key>"; }
  { rank=same; "FriendsIterator"; "SuggestionsIterator"; "NewsFeedIterator"; "Profile"; "Post"; }
  "FriendsIterator" -> "SuggestionsIterator" -> "NewsFeedIterator" -> "Profile" -> "Post" [style=invis];

  "FeedScreen" -> "SocialNetwork" [style=dashed, arrowhead=vee, label=" uses"];
  "FeedScreen" -> "Iterator<T>"   [style=dashed, arrowhead=vee, label=" uses", constraint=false];

  "SocialNetwork" -> "CampusConnect" [dir=back, arrowtail=empty, style=dashed];
  "Iterator<T>"   -> "FailFastIterator<T, Key>" [dir=back, arrowtail=empty, style=dashed];
  "FailFastIterator<T, Key>" -> "FriendsIterator"     [dir=back, arrowtail=empty];
  "FailFastIterator<T, Key>" -> "SuggestionsIterator" [dir=back, arrowtail=empty];
  "FailFastIterator<T, Key>" -> "NewsFeedIterator"    [dir=back, arrowtail=empty];

  "FailFastIterator<T, Key>" -> "CampusConnect" [arrowhead=vee, label=" network_", constraint=false];

  "CampusConnect" -> "FriendsIterator"     [style=dashed, arrowhead=vee, label="«creates»", color="#8A6A1F", fontcolor="#8A6A1F"];
  "CampusConnect" -> "SuggestionsIterator" [style=dashed, arrowhead=vee, color="#8A6A1F"];
  "CampusConnect" -> "NewsFeedIterator"    [style=dashed, arrowhead=vee, color="#8A6A1F"];

  "CampusConnect" -> "Profile" [dir=back, arrowtail=diamond, headlabel="*", labeldistance=1.8];
  "CampusConnect" -> "Post"    [dir=back, arrowtail=diamond, headlabel="*", labeldistance=1.8];
}
'''
open("iterator_class.dot","w").write(d)
