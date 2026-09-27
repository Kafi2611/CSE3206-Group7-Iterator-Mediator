#pragma once
/**
 * @file   Message.hpp
 * @brief  Data carried between colleagues by the mediator.
 */

#include <cstdint>
#include <stdexcept>
#include <string>

namespace campus::chat {

/// What happened to a direct message.
enum class DeliveryStatus : std::uint8_t {
    Delivered,  ///< recipient was online and received it immediately
    Queued,     ///< recipient offline - stored until they come online
    Blocked     ///< recipient has blocked the sender - dropped silently
};

inline const char* toString(DeliveryStatus status) noexcept {
    switch (status) {
        case DeliveryStatus::Delivered: return "Delivered";
        case DeliveryStatus::Queued:    return "Queued (recipient offline)";
        case DeliveryStatus::Blocked:   return "Blocked by recipient";
    }
    return "Unknown";
}

/// A chat message. For a group message, groupId is set and toId is the
/// individual member it is being delivered to.
struct Message {
    int         id = 0;
    std::string fromId;
    std::string fromName;
    std::string toId;
    std::string groupId;    ///< empty for a direct (one-to-one) message
    std::string text;
    bool        automated = false;  ///< true if a bot sent it

    bool isGroupMessage() const noexcept { return !groupId.empty(); }
};

/// Thrown when a user tries to do something the server does not allow
/// (e.g. posting in a group they are not a member of).
class PermissionDenied : public std::runtime_error {
public:
    explicit PermissionDenied(const std::string& message)
        : std::runtime_error(message) {}
};

}  // namespace campus::chat
