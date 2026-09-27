/**
 * @file   main.cpp  (iterator demo)
 * @brief  Live demonstration of the Iterator pattern on CampusConnect,
 *         a Facebook-like campus social network.
 *
 * CSE 3206 - Lab 3 - Group 7 (Iterator & Mediator)
 */

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "CampusConnect.hpp"
#include "FeedScreen.hpp"

using namespace campus::feed;

namespace {

void banner(const std::string& text) {
    std::cout << "\n==== " << text << " ====\n";
}

/// Sample data: 7 students, 8 friendships, 6 posts.
void buildSampleNetwork(CampusConnect& network) {
    network.addProfile({"kafi",   "Abdullah Al Kafi", "CSE"});
    network.addProfile({"sadaf",  "Sadaf Rahman",     "CSE"});
    network.addProfile({"sakila", "Sakila Akter",     "CSE"});
    network.addProfile({"rafi",   "Rafi Hasan",       "EEE"});
    network.addProfile({"nusrat", "Nusrat Jahan",     "ME"});
    network.addProfile({"tanvir", "Tanvir Ahmed",     "CE"});
    network.addProfile({"mim",    "Mim Chowdhury",    "URP"});

    network.addFriendship("kafi",   "sadaf");
    network.addFriendship("kafi",   "sakila");
    network.addFriendship("sadaf",  "sakila");
    network.addFriendship("sadaf",  "rafi");
    network.addFriendship("sakila", "rafi");
    network.addFriendship("sakila", "nusrat");
    network.addFriendship("rafi",   "tanvir");
    network.addFriendship("nusrat", "mim");

    network.publishPost("sadaf",  "Lab 3 group formed: we got Iterator + Mediator!", 12);
    network.publishPost("rafi",   "EEE fest tickets available at the main gate", 30);
    network.publishPost("kafi",   "Iterator pattern = one loop for every collection", 25);
    network.publishPost("nusrat", "Robotics club meeting at 5 PM", 8);
    network.publishPost("sakila", "UML diagrams done, pushing to GitHub tonight", 17);
    network.publishPost("kafi",   "Live demo tomorrow in the CSE 3206 lab. Wish us luck!", 40);
}

}  // namespace

int main() {
    std::cout << "CampusConnect - Iterator Pattern Demo (CSE 3206, Group 7)\n";

    CampusConnect network;          // <<ConcreteAggregate>>
    buildSampleNetwork(network);
    FeedScreen screen(std::cout);   // <<Client>> - knows only Iterator<T>

    // 1-3. One client, three different traversal algorithms --------------
    banner("1. FriendsIterator: Kafi's friends");
    auto friends = network.createFriendsIterator("kafi");
    screen.showProfiles("Friends of Kafi:", *friends);

    banner("2. SuggestionsIterator: People You May Know");
    auto suggestions = network.createSuggestionsIterator("kafi");
    screen.showProfiles("Suggested for Kafi (most mutual friends first):", *suggestions);

    banner("3. NewsFeedIterator: Kafi's news feed");
    auto feed = network.createNewsFeedIterator("kafi");
    screen.showPosts("Newest first, only Kafi + friends:", *feed);

    // 4. Two independent iterators at the same time + reset() -----------
    banner("4. Two iterators at once: mutual friends of Kafi and Rafi");
    for (const std::string& name : findMutualFriends(network, "kafi", "rafi")) {
        std::cout << "  - " << name << '\n';
    }

    // 5. reset(): the same iterator object can traverse again -----------
    banner("5. reset(): walk Kafi's friends again with the SAME iterator");
    friends->reset();
    screen.showProfiles("Friends of Kafi (second pass):", *friends);

    // 6. Fail-fast: collection changes during iteration ------------------
    banner("6. Fail-fast: a new post arrives while scrolling");
    auto scrolling = network.createNewsFeedIterator("kafi");
    std::cout << "  First post on screen: \"" << scrolling->next().content << "\"\n";
    network.publishPost("sakila", "Breaking: the demo actually works!", 5);
    try {
        scrolling->next();
    } catch (const ConcurrentModificationError& error) {
        std::cout << "  Caught ConcurrentModificationError: " << error.what() << '\n';
    }
    auto refreshed = network.createNewsFeedIterator("kafi");
    std::cout << "  After refresh, top post: \"" << refreshed->next().content << "\"\n";

    // 7. Exception handling -------------------------------------------------
    banner("7. Exception handling");
    try {
        auto ghost = network.createFriendsIterator("zara");  // throws first
    } catch (const std::invalid_argument& error) {
        std::cout << "  Caught invalid_argument: " << error.what() << '\n';
    }
    try {
        auto it = network.createSuggestionsIterator("mim");
        while (it->hasNext()) {
            it->next();
        }
        it->next();  // one step too far
    } catch (const std::out_of_range& error) {
        std::cout << "  Caught out_of_range: " << error.what() << '\n';
    }

    std::cout << "\nDemo finished successfully.\n";
    return 0;
}
