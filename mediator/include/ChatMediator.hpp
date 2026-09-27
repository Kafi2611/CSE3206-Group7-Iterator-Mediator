#pragma once
/**
 * @file   ChatMediator.hpp
 * @brief  <<Mediator>> role of the Mediator pattern.
 *
 * The ONLY thing a User knows about the rest of the system. Users never
 * hold pointers to each other; every interaction goes through here.
 */

#include <cstddef>
#include <string>

#include "Message.hpp"

namespace campus::chat {

class User;  // <<Colleague>> (forward declaration breaks the include cycle)

class ChatMediator {
public:
    virtual ~ChatMediator() = default;

    // ---- membership -------------------------------------------------------
    virtual void registerUser(User& user) = 0;
    virtual void unregisterUser(const std::string& userId) noexcept = 0;

    // ---- interactions requested by colleagues -----------------------------
    virtual DeliveryStatus sendDirect(const User& from, const std::string& toId,
                                      const std::string& text) = 0;

    /// @return number of group members the message reached (now or queued).
    virtual std::size_t sendToGroup(const User& from, const std::string& groupId,
                                    const std::string& text) = 0;

    virtual void blockUser(const User& blocker, const std::string& blockedId) = 0;

    /// A colleague tells the mediator it went online/offline.
    virtual void presenceChanged(const User& user) = 0;

protected:  // polymorphic base: no slicing copies from outside (C.67)
    ChatMediator()                               = default;
    ChatMediator(const ChatMediator&)            = default;
    ChatMediator& operator=(const ChatMediator&) = default;
    ChatMediator(ChatMediator&&)                 = default;
    ChatMediator& operator=(ChatMediator&&)      = default;
};

}  // namespace campus::chat
