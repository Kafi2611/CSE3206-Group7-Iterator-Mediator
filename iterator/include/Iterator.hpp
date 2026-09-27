#pragma once
/**
 * @file   Iterator.hpp
 * @brief  <<Iterator>> role of the Iterator pattern.
 *
 * One small interface that every CampusConnect traversal implements
 * (friends list, "People You May Know", news feed). Client code walks
 * ANY collection with the same three calls and never sees how the data
 * is stored (graph, map, vector ...).
 *
 * CSE 3206 - Lab 3 - Group 7
 */

#include <stdexcept>
#include <string>

namespace campus::feed {

/// Thrown by an iterator when its collection was modified after the
/// iterator was created (fail-fast behaviour, like Java's
/// ConcurrentModificationException).
class ConcurrentModificationError : public std::runtime_error {
public:
    explicit ConcurrentModificationError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * Generic read-only forward iterator.
 * @tparam T element type returned by next() (Profile, Post, ...)
 */
template <typename T>
class Iterator {
public:
    virtual ~Iterator() = default;

    /// @return true if next() can be called at least once more.
    virtual bool hasNext() const = 0;

    /// Returns the current element and moves the cursor forward.
    /// @throws std::out_of_range            if no element is left.
    /// @throws ConcurrentModificationError  if the collection changed.
    virtual const T& next() = 0;

    /// Moves the cursor back to the first element (traverse again).
    virtual void reset() = 0;

protected:  // polymorphic base: no slicing copies from outside (C++ Core Guidelines C.67)
    Iterator()                           = default;
    Iterator(const Iterator&)            = default;
    Iterator& operator=(const Iterator&) = default;
    Iterator(Iterator&&)                 = default;
    Iterator& operator=(Iterator&&)      = default;
};

}  // namespace campus::feed
