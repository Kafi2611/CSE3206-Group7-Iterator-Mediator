/**
 * @file   FeedIterators.cpp
 * @brief  Traversal algorithms of the three <<ConcreteIterator>> classes.
 */

#include "FeedIterators.hpp"

#include <algorithm>
#include <map>
#include <set>

namespace campus::feed {

// ---- FriendsIterator -------------------------------------------------------

FriendsIterator::FriendsIterator(const CampusConnect& network,
                                 const std::string& profileId)
    : FailFastIterator(network, collect(network, profileId)) {}

std::vector<std::string> FriendsIterator::collect(const CampusConnect& network,
                                                  const std::string& profileId) {
    const std::set<std::string>& ids = network.friendIdsOf(profileId);
    return {ids.begin(), ids.end()};  // std::set is already sorted
}

const Profile& FriendsIterator::resolve(const std::string& id) const {
    return network().profile(id);
}

// ---- SuggestionsIterator ("People You May Know") ---------------------------

SuggestionsIterator::SuggestionsIterator(const CampusConnect& network,
                                         const std::string& profileId)
    : FailFastIterator(network, collect(network, profileId)) {}

std::vector<std::string>
SuggestionsIterator::collect(const CampusConnect& network,
                             const std::string& profileId) {
    const std::set<std::string>& myFriends = network.friendIdsOf(profileId);

    // Breadth-first search, depth 2: count how many of my friends know X.
    std::map<std::string, int> mutualCount;
    for (const std::string& friendId : myFriends) {
        for (const std::string& candidate : network.friendIdsOf(friendId)) {
            const bool isMe       = candidate == profileId;
            const bool isMyFriend = myFriends.count(candidate) > 0;
            if (!isMe && !isMyFriend) {
                ++mutualCount[candidate];
            }
        }
    }

    std::vector<std::string> order;
    order.reserve(mutualCount.size());
    for (const auto& entry : mutualCount) {
        order.push_back(entry.first);
    }
    std::stable_sort(order.begin(), order.end(),
                     [&mutualCount](const std::string& a, const std::string& b) {
                         return mutualCount.at(a) > mutualCount.at(b);
                     });
    return order;
}

const Profile& SuggestionsIterator::resolve(const std::string& id) const {
    return network().profile(id);
}

// ---- NewsFeedIterator ------------------------------------------------------

NewsFeedIterator::NewsFeedIterator(const CampusConnect& network,
                                   const std::string& profileId)
    : FailFastIterator(network, collect(network, profileId)) {}

std::vector<std::size_t> NewsFeedIterator::collect(const CampusConnect& network,
                                                   const std::string& profileId) {
    std::set<std::string> circle = network.friendIdsOf(profileId);
    circle.insert(profileId);  // my own posts appear in my feed too

    const std::vector<Post>& posts = network.posts();
    std::vector<std::size_t> order;
    for (std::size_t i = 0; i < posts.size(); ++i) {
        if (circle.count(posts[i].authorId) > 0) {
            order.push_back(i);
        }
    }
    std::sort(order.begin(), order.end(), [&posts](std::size_t a, std::size_t b) {
        return posts[a].timestamp > posts[b].timestamp;  // newest first
    });
    return order;
}

const Post& NewsFeedIterator::resolve(const std::size_t& index) const {
    return network().posts().at(index);
}

}  // namespace campus::feed
