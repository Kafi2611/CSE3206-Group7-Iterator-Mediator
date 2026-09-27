#pragma once
/**
 * @file   User.hpp
 * @brief  <<Colleague>> role of the Mediator pattern (abstract).
 *
 * A User can send, block and change presence, but EVERY action is
 * forwarded to the ChatMediator. A User never references another User.
 */

#include <cstddef>
#include <string>

#include "ChatMediator.hpp"
#include "Message.hpp"

namespace campus::chat {

class User {
public:
    User(std::string id, std::string name);
    virtual ~User();  // automatically leaves the server (RAII)

    // A User is an identity registered by address in the server:
    // copying or moving it would leave a dangling registration.
    User(const User&)            = delete;
    User& operator=(const User&) = delete;
    User(User&&)                 = delete;
    User& operator=(User&&)      = delete;

    const std::string& id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
    bool isOnline() const noexcept { return online_; }
    bool isConnected() const noexcept { return mediator_ != nullptr; }
    virtual bool isAutomated() const noexcept { return false; }

    // ---- connection ---------------------------------------------------------
    void join(ChatMediator& mediator);  ///< log in to a server
    void leave() noexcept;              ///< log out (safe to call twice)

    // ---- actions: all forwarded to the mediator -----------------------------
    DeliveryStatus sendTo(const std::string& toId, const std::string& text);
    std::size_t    sendToGroup(const std::string& groupId, const std::string& text);
    void           block(const std::string& userId);
    void           goOnline();
    void           goOffline();

    /// Called BY THE MEDIATOR when a message arrives for this user.
    virtual void receive(const Message& message) = 0;

protected:
    /// @throws std::logic_error if the user has not joined a server.
    ChatMediator& mediator() const;

private:
    void setPresence(bool online);

    std::string   id_;
    std::string   name_;
    bool          online_   = true;
    ChatMediator* mediator_ = nullptr;  ///< non-owning; server outlives users
};

}  // namespace campus::chat
