#pragma once
/**
 * @file   FeedScreen.hpp
 * @brief  <<Client>> of the Iterator pattern.
 *
 * The UI layer. It renders friends lists, suggestions and the news feed
 * using ONLY the Iterator<T> / SocialNetwork interfaces. It has no idea
 * whether the data is a graph, a map or a vector - and it does not care.
 */

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include "SocialNetwork.hpp"

namespace campus::feed {

class FeedScreen {
public:
    explicit FeedScreen(std::ostream& out) : out_(out) {}

    /// Prints every profile the iterator yields; returns how many.
    std::size_t showProfiles(const std::string& title, ProfileIterator& profiles);

    /// Prints every post the iterator yields; returns how many.
    std::size_t showPosts(const std::string& title, PostIterator& posts);

private:
    std::ostream& out_;
};

/**
 * Uses TWO iterators at the same time (nested traversal + reset()) to find
 * the names of the friends that @p firstId and @p secondId have in common.
 */
std::vector<std::string> findMutualFriends(const SocialNetwork& network,
                                           const std::string& firstId,
                                           const std::string& secondId);

}  // namespace campus::feed
