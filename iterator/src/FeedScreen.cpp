/**
 * @file   FeedScreen.cpp
 * @brief  <<Client>> implementation - notice both loops look identical.
 */

#include "FeedScreen.hpp"

namespace campus::feed {

std::size_t FeedScreen::showProfiles(const std::string& title,
                                     ProfileIterator& profiles) {
    out_ << title << '\n';
    std::size_t count = 0;
    while (profiles.hasNext()) {                 // same loop for ANY collection
        const Profile& profile = profiles.next();
        out_ << "  " << ++count << ". " << profile.name
             << " (@" << profile.id << ", " << profile.department << ")\n";
    }
    if (count == 0) {
        out_ << "  (nobody)\n";
    }
    return count;
}

std::size_t FeedScreen::showPosts(const std::string& title, PostIterator& posts) {
    out_ << title << '\n';
    std::size_t count = 0;
    while (posts.hasNext()) {                    // same loop for ANY collection
        const Post& post = posts.next();
        out_ << "  [t=" << post.timestamp << "] " << post.authorName << ": \""
             << post.content << "\"  (" << post.likes << " likes)\n";
        ++count;
    }
    if (count == 0) {
        out_ << "  (no posts)\n";
    }
    return count;
}

std::vector<std::string> findMutualFriends(const SocialNetwork& network,
                                           const std::string& firstId,
                                           const std::string& secondId) {
    auto outer = network.createFriendsIterator(firstId);
    auto inner = network.createFriendsIterator(secondId);  // 2nd, independent cursor

    std::vector<std::string> mutual;
    while (outer->hasNext()) {
        const Profile& candidate = outer->next();
        inner->reset();                          // restart the inner traversal
        while (inner->hasNext()) {
            if (inner->next().id == candidate.id) {
                mutual.push_back(candidate.name);
                break;
            }
        }
    }
    return mutual;
}

}  // namespace campus::feed
