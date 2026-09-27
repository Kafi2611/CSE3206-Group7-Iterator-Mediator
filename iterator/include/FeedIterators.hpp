#pragma once
/**
 * @file   FeedIterators.hpp
 * @brief  <<ConcreteIterator>> classes of CampusConnect.
 *
 * Every concrete iterator = ONE traversal algorithm:
 *   FriendsIterator      - direct friends (graph neighbours)
 *   SuggestionsIterator  - friends-of-friends ranked by mutual friends
 *   NewsFeedIterator     - posts of me + my friends, newest first
 *
 * The common cursor / fail-fast plumbing lives in FailFastIterator so
 * each concrete class only decides the ORDER of the elements.
 */

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "CampusConnect.hpp"

namespace campus::feed {

/**
 * Reusable base: walks a pre-computed list of keys and resolves each key
 * to an element of the network. Fails fast if the network is modified.
 * @tparam T   element type returned to the client
 * @tparam Key what we store in the traversal order (id, index, ...)
 */
template <typename T, typename Key>
class FailFastIterator : public Iterator<T> {
public:
    bool hasNext() const override { return cursor_ < order_.size(); }

    const T& next() override {
        ensureNotModified();
        if (!hasNext()) {
            throw std::out_of_range("Iterator exhausted: no more elements");
        }
        return resolve(order_[cursor_++]);
    }

    void reset() override {
        ensureNotModified();
        cursor_ = 0;
    }

    /// Number of elements this traversal will visit in total.
    std::size_t size() const noexcept { return order_.size(); }

protected:
    FailFastIterator(const CampusConnect& network, std::vector<Key> order)
        : network_(network),
          order_(std::move(order)),
          expectedVersion_(network.version()) {}

    /// Converts a stored key into the element handed to the client.
    virtual const T& resolve(const Key& key) const = 0;

    const CampusConnect& network() const noexcept { return network_; }

private:
    void ensureNotModified() const {
        if (network_.version() != expectedVersion_) {
            throw ConcurrentModificationError(
                "CampusConnect was modified while it was being iterated");
        }
    }

    const CampusConnect& network_;
    std::vector<Key>     order_;
    std::size_t          cursor_ = 0;
    std::size_t          expectedVersion_;
};

// ---------------------------------------------------------------------------

/// Direct friends of one profile (alphabetical by id).
class FriendsIterator final : public FailFastIterator<Profile, std::string> {
public:
    FriendsIterator(const CampusConnect& network, const std::string& profileId);

private:
    static std::vector<std::string> collect(const CampusConnect& network,
                                            const std::string& profileId);
    const Profile& resolve(const std::string& id) const override;
};

/// "People You May Know": friends-of-friends who are not yet friends,
/// ordered by number of mutual friends (desc), then by id.
class SuggestionsIterator final
    : public FailFastIterator<Profile, std::string> {
public:
    SuggestionsIterator(const CampusConnect& network, const std::string& profileId);

private:
    static std::vector<std::string> collect(const CampusConnect& network,
                                            const std::string& profileId);
    const Profile& resolve(const std::string& id) const override;
};

/// News feed: posts written by the profile or any friend, newest first.
class NewsFeedIterator final : public FailFastIterator<Post, std::size_t> {
public:
    NewsFeedIterator(const CampusConnect& network, const std::string& profileId);

private:
    static std::vector<std::size_t> collect(const CampusConnect& network,
                                            const std::string& profileId);
    const Post& resolve(const std::size_t& index) const override;
};

}  // namespace campus::feed
