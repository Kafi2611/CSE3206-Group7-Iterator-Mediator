#pragma once
/**
 * @file   SocialNetwork.hpp
 * @brief  <<Aggregate>> role of the Iterator pattern.
 *
 * Declares the factory methods that hand out iterators. Clients depend
 * only on this interface, so a different network implementation (e.g. a
 * database-backed one) can be plugged in without touching client code.
 */

#include <memory>
#include <string>

#include "Iterator.hpp"
#include "Models.hpp"

namespace campus::feed {

using ProfileIterator = Iterator<Profile>;
using PostIterator    = Iterator<Post>;

class SocialNetwork {
public:
    virtual ~SocialNetwork() = default;

    /// Friends of @p profileId in alphabetical order of their id.
    [[nodiscard]] virtual std::unique_ptr<ProfileIterator>
    createFriendsIterator(const std::string& profileId) const = 0;

    /// "People You May Know": friends-of-friends, most mutual friends first.
    [[nodiscard]] virtual std::unique_ptr<ProfileIterator>
    createSuggestionsIterator(const std::string& profileId) const = 0;

    /// News feed: posts by the user and their friends, newest first.
    [[nodiscard]] virtual std::unique_ptr<PostIterator>
    createNewsFeedIterator(const std::string& profileId) const = 0;

protected:  // polymorphic base: no slicing copies from outside (C.67)
    SocialNetwork()                                = default;
    SocialNetwork(const SocialNetwork&)            = default;
    SocialNetwork& operator=(const SocialNetwork&) = default;
    SocialNetwork(SocialNetwork&&)                 = default;
    SocialNetwork& operator=(SocialNetwork&&)      = default;
};

}  // namespace campus::feed
