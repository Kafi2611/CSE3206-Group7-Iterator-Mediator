#pragma once
/**
 * @file   CampusConnect.hpp
 * @brief  <<ConcreteAggregate>> - a Facebook-like campus social network.
 *
 * Internally the data lives in THREE different structures:
 *   - profiles  : std::map  (id -> Profile)
 *   - friends   : adjacency list (id -> set of friend ids)  = a graph
 *   - posts     : std::vector<Post> in publishing order
 * None of these are exposed to clients as containers; clients only get
 * iterators from the create...Iterator() factory methods.
 */

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "SocialNetwork.hpp"

namespace campus::feed {

class CampusConnect final : public SocialNetwork {
public:
    // ---- building the network (each call changes version()) -------------
    void addProfile(const Profile& profile);
    void addFriendship(const std::string& firstId, const std::string& secondId);
    int  publishPost(const std::string& authorId, const std::string& content,
                     int likes = 0);

    // ---- read-only helpers used by the concrete iterators --------------
    bool                         hasProfile(const std::string& id) const;
    const Profile&               profile(const std::string& id) const;
    const std::set<std::string>& friendIdsOf(const std::string& id) const;
    const std::vector<Post>&     posts() const noexcept { return posts_; }
    std::size_t                  version() const noexcept { return version_; }

    // ---- <<Aggregate>> factory methods ----------------------------------
    [[nodiscard]] std::unique_ptr<ProfileIterator>
    createFriendsIterator(const std::string& profileId) const override;

    [[nodiscard]] std::unique_ptr<ProfileIterator>
    createSuggestionsIterator(const std::string& profileId) const override;

    [[nodiscard]] std::unique_ptr<PostIterator>
    createNewsFeedIterator(const std::string& profileId) const override;

private:
    void requireProfile(const std::string& id) const;

    std::map<std::string, Profile>               profiles_;
    std::map<std::string, std::set<std::string>> friends_;
    std::vector<Post>                            posts_;

    long        clock_      = 0;  ///< logical clock for post timestamps
    int         nextPostId_ = 1;
    std::size_t version_    = 0;  ///< bumped on every modification
};

}  // namespace campus::feed
