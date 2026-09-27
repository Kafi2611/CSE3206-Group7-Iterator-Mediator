#pragma once
/**
 * @file   Models.hpp
 * @brief  Plain data objects stored inside the CampusConnect network.
 */

#include <string>

namespace campus::feed {

/// A user profile (one node of the friendship graph).
struct Profile {
    std::string id;          ///< unique handle, e.g. "kafi"
    std::string name;        ///< display name, e.g. "Abdullah Al Kafi"
    std::string department;  ///< e.g. "CSE"
};

/// A status update shown in the news feed.
struct Post {
    int         id = 0;
    std::string authorId;
    std::string authorName;
    std::string content;
    int         likes = 0;
    long        timestamp = 0;  ///< logical clock: bigger value = newer post
};

}  // namespace campus::feed
