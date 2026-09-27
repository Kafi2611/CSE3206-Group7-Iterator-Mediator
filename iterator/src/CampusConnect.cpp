/**
 * @file   CampusConnect.cpp
 * @brief  <<ConcreteAggregate>> implementation.
 */

#include "CampusConnect.hpp"

#include <memory>
#include <stdexcept>

#include "FeedIterators.hpp"

namespace campus::feed {

// ---- building the network --------------------------------------------------

void CampusConnect::addProfile(const Profile& profile) {
    if (profile.id.empty()) {
        throw std::invalid_argument("Profile id must not be empty");
    }
    if (hasProfile(profile.id)) {
        throw std::invalid_argument("Duplicate profile id: " + profile.id);
    }
    profiles_.emplace(profile.id, profile);
    friends_[profile.id];  // every profile gets an (empty) adjacency list
    ++version_;
}

void CampusConnect::addFriendship(const std::string& firstId,
                                  const std::string& secondId) {
    requireProfile(firstId);
    requireProfile(secondId);
    if (firstId == secondId) {
        throw std::invalid_argument("A profile cannot befriend itself: " + firstId);
    }
    friends_[firstId].insert(secondId);  // friendship is mutual (undirected)
    friends_[secondId].insert(firstId);
    ++version_;
}

int CampusConnect::publishPost(const std::string& authorId,
                               const std::string& content, int likes) {
    const Profile& author = profile(authorId);  // throws if unknown
    if (likes < 0) {
        throw std::invalid_argument("Likes cannot be negative");
    }
    Post post;
    post.id         = nextPostId_++;
    post.authorId   = author.id;
    post.authorName = author.name;
    post.content    = content;
    post.likes      = likes;
    post.timestamp  = ++clock_;
    posts_.push_back(post);
    ++version_;
    return post.id;
}

// ---- read-only helpers -----------------------------------------------------

bool CampusConnect::hasProfile(const std::string& id) const {
    return profiles_.find(id) != profiles_.end();
}

const Profile& CampusConnect::profile(const std::string& id) const {
    const auto it = profiles_.find(id);
    if (it == profiles_.end()) {
        throw std::invalid_argument("Unknown profile id: " + id);
    }
    return it->second;
}

const std::set<std::string>& CampusConnect::friendIdsOf(const std::string& id) const {
    requireProfile(id);
    return friends_.at(id);
}

void CampusConnect::requireProfile(const std::string& id) const {
    if (!hasProfile(id)) {
        throw std::invalid_argument("Unknown profile id: " + id);
    }
}

// ---- <<Aggregate>> factory methods ----------------------------------------

std::unique_ptr<ProfileIterator>
CampusConnect::createFriendsIterator(const std::string& profileId) const {
    return std::make_unique<FriendsIterator>(*this, profileId);
}

std::unique_ptr<ProfileIterator>
CampusConnect::createSuggestionsIterator(const std::string& profileId) const {
    return std::make_unique<SuggestionsIterator>(*this, profileId);
}

std::unique_ptr<PostIterator>
CampusConnect::createNewsFeedIterator(const std::string& profileId) const {
    return std::make_unique<NewsFeedIterator>(*this, profileId);
}

}  // namespace campus::feed
