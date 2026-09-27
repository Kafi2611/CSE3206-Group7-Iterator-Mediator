/**
 * @file   test_iterator.cpp
 * @brief  Unit tests for the Iterator pattern (CampusConnect feed).
 */

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "CampusConnect.hpp"
#include "FeedScreen.hpp"
#include "TestFramework.hpp"

using namespace campus::feed;

namespace {

/// Same small network as the demo: kafi-sadaf-sakila triangle, rafi is a
/// friend of sadaf and sakila, nusrat is a friend of sakila.
CampusConnect makeNetwork() {
    CampusConnect network;
    network.addProfile({"kafi", "Kafi", "CSE"});
    network.addProfile({"sadaf", "Sadaf", "CSE"});
    network.addProfile({"sakila", "Sakila", "CSE"});
    network.addProfile({"rafi", "Rafi", "EEE"});
    network.addProfile({"nusrat", "Nusrat", "ME"});
    network.addProfile({"loner", "Loner", "BME"});
    network.addFriendship("kafi", "sadaf");
    network.addFriendship("kafi", "sakila");
    network.addFriendship("sadaf", "sakila");
    network.addFriendship("sadaf", "rafi");
    network.addFriendship("sakila", "rafi");
    network.addFriendship("sakila", "nusrat");
    return network;
}

/// Drains a profile iterator into a list of ids.
std::vector<std::string> idsOf(ProfileIterator& it) {
    std::vector<std::string> ids;
    while (it.hasNext()) {
        ids.push_back(it.next().id);
    }
    return ids;
}

std::string join(const std::vector<std::string>& items) {
    std::string out;
    for (const std::string& item : items) {
        out += (out.empty() ? "" : ",") + item;
    }
    return out;
}

}  // namespace

// ---- FriendsIterator -----------------------------------------------------------

TEST_CASE(friends_iterator_returns_direct_friends_in_id_order) {
    const CampusConnect network = makeNetwork();
    auto it = network.createFriendsIterator("sakila");
    CHECK_EQ(join(idsOf(*it)), std::string("kafi,nusrat,rafi,sadaf"));
}

TEST_CASE(friends_iterator_on_profile_without_friends_is_empty) {
    const CampusConnect network = makeNetwork();
    auto it = network.createFriendsIterator("loner");
    CHECK(!it->hasNext());
}

TEST_CASE(friendship_is_mutual) {
    const CampusConnect network = makeNetwork();
    auto it = network.createFriendsIterator("rafi");
    CHECK_EQ(join(idsOf(*it)), std::string("sadaf,sakila"));
}

// ---- SuggestionsIterator -------------------------------------------------------

TEST_CASE(suggestions_rank_by_mutual_friends_and_exclude_existing_friends) {
    const CampusConnect network = makeNetwork();
    auto it = network.createSuggestionsIterator("kafi");
    // rafi knows sadaf + sakila (2 mutual), nusrat knows sakila (1 mutual)
    CHECK_EQ(join(idsOf(*it)), std::string("rafi,nusrat"));
}

TEST_CASE(suggestions_never_include_self) {
    const CampusConnect network = makeNetwork();
    auto it = network.createSuggestionsIterator("rafi");
    const std::vector<std::string> ids = idsOf(*it);
    for (const std::string& id : ids) {
        CHECK(id != "rafi");
    }
    CHECK_EQ(join(ids), std::string("kafi,nusrat"));
}

// ---- NewsFeedIterator ----------------------------------------------------------

TEST_CASE(news_feed_is_newest_first_and_only_from_friend_circle) {
    CampusConnect network = makeNetwork();
    network.publishPost("sadaf", "first");
    network.publishPost("nusrat", "not a friend of kafi");
    network.publishPost("kafi", "second");
    network.publishPost("sakila", "third");

    auto it = network.createNewsFeedIterator("kafi");
    std::vector<std::string> contents;
    while (it->hasNext()) {
        contents.push_back(it->next().content);
    }
    CHECK_EQ(join(contents), std::string("third,second,first"));
}

TEST_CASE(publish_post_validates_input) {
    CampusConnect network = makeNetwork();
    CHECK_THROWS_AS(network.publishPost("ghost", "hello"), std::invalid_argument);
    CHECK_THROWS_AS(network.publishPost("kafi", "hello", -1), std::invalid_argument);
}

// ---- Iterator contract ---------------------------------------------------------

TEST_CASE(reset_allows_a_second_traversal) {
    const CampusConnect network = makeNetwork();
    auto it = network.createFriendsIterator("kafi");
    const std::string first = join(idsOf(*it));
    it->reset();
    CHECK_EQ(join(idsOf(*it)), first);
}

TEST_CASE(next_after_end_throws_out_of_range) {
    const CampusConnect network = makeNetwork();
    auto it = network.createFriendsIterator("loner");
    CHECK_THROWS_AS(it->next(), std::out_of_range);
}

TEST_CASE(two_iterators_on_same_collection_are_independent) {
    const CampusConnect network = makeNetwork();
    auto a = network.createFriendsIterator("kafi");
    auto b = network.createFriendsIterator("kafi");
    CHECK_EQ(a->next().id, std::string("sadaf"));
    CHECK_EQ(a->next().id, std::string("sakila"));
    CHECK_EQ(b->next().id, std::string("sadaf"));  // b unaffected by a
}

TEST_CASE(modifying_network_makes_live_iterator_fail_fast) {
    CampusConnect network = makeNetwork();
    network.publishPost("kafi", "hello");
    auto it = network.createNewsFeedIterator("kafi");
    network.addFriendship("kafi", "nusrat");
    CHECK_THROWS_AS(it->next(), ConcurrentModificationError);
    CHECK_THROWS_AS(it->reset(), ConcurrentModificationError);
}

TEST_CASE(unknown_profile_is_rejected) {
    const CampusConnect network = makeNetwork();
    CHECK_THROWS_AS((void)network.createFriendsIterator("zara"), std::invalid_argument);
    CHECK_THROWS_AS((void)network.createSuggestionsIterator("zara"), std::invalid_argument);
    CHECK_THROWS_AS((void)network.createNewsFeedIterator("zara"), std::invalid_argument);
}

TEST_CASE(network_rejects_bad_profiles_and_friendships) {
    CampusConnect network = makeNetwork();
    CHECK_THROWS_AS(network.addProfile({"kafi", "Dup", "CSE"}), std::invalid_argument);
    CHECK_THROWS_AS(network.addProfile({"", "NoId", "CSE"}), std::invalid_argument);
    CHECK_THROWS_AS(network.addFriendship("kafi", "kafi"), std::invalid_argument);
    CHECK_THROWS_AS(network.addFriendship("kafi", "zara"), std::invalid_argument);
}

// ---- Client ----------------------------------------------------------------------

TEST_CASE(client_uses_the_same_loop_for_every_iterator) {
    CampusConnect network = makeNetwork();
    network.publishPost("sadaf", "hi");
    std::ostringstream out;
    FeedScreen screen(out);

    auto friends     = network.createFriendsIterator("kafi");
    auto suggestions = network.createSuggestionsIterator("kafi");
    auto feed        = network.createNewsFeedIterator("kafi");
    CHECK_EQ(screen.showProfiles("Friends", *friends), 2u);
    CHECK_EQ(screen.showProfiles("Suggestions", *suggestions), 2u);
    CHECK_EQ(screen.showPosts("Feed", *feed), 1u);
    CHECK(out.str().find("Sadaf") != std::string::npos);
}

TEST_CASE(client_prints_placeholder_for_empty_collections) {
    const CampusConnect network = makeNetwork();
    std::ostringstream out;
    FeedScreen screen(out);
    auto friends = network.createFriendsIterator("loner");
    auto feed    = network.createNewsFeedIterator("loner");
    CHECK_EQ(screen.showProfiles("Friends", *friends), 0u);
    CHECK_EQ(screen.showPosts("Feed", *feed), 0u);
    CHECK(out.str().find("(nobody)") != std::string::npos);
    CHECK(out.str().find("(no posts)") != std::string::npos);
}

TEST_CASE(mutual_friends_uses_two_iterators) {
    const CampusConnect network = makeNetwork();
    CHECK_EQ(join(findMutualFriends(network, "kafi", "rafi")), std::string("Sadaf,Sakila"));
    CHECK(findMutualFriends(network, "kafi", "loner").empty());
}

int main() { return campus::test::runAll(); }
