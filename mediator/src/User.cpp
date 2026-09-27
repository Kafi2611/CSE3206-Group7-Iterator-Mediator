/**
 * @file   User.cpp
 * @brief  <<Colleague>> base implementation - pure delegation to the mediator.
 */

#include "User.hpp"

#include <stdexcept>
#include <utility>

namespace campus::chat {

User::User(std::string id, std::string name)
    : id_(std::move(id)), name_(std::move(name)) {
    if (id_.empty()) {
        throw std::invalid_argument("User id must not be empty");
    }
}

User::~User() { leave(); }

void User::join(ChatMediator& mediator) {
    if (mediator_ != nullptr) {
        throw std::logic_error(name_ + " is already connected to a server");
    }
    mediator.registerUser(*this);  // may throw (e.g. duplicate id)
    mediator_ = &mediator;         // only set once registration succeeded
}

void User::leave() noexcept {
    if (mediator_ != nullptr) {
        mediator_->unregisterUser(id_);
        mediator_ = nullptr;
    }
}

DeliveryStatus User::sendTo(const std::string& toId, const std::string& text) {
    return mediator().sendDirect(*this, toId, text);
}

std::size_t User::sendToGroup(const std::string& groupId, const std::string& text) {
    return mediator().sendToGroup(*this, groupId, text);
}

void User::block(const std::string& userId) { mediator().blockUser(*this, userId); }

void User::goOnline() { setPresence(true); }

void User::goOffline() { setPresence(false); }

void User::setPresence(bool online) {
    online_ = online;
    if (mediator_ != nullptr) {
        mediator_->presenceChanged(*this);  // server may flush queued messages
    }
}

ChatMediator& User::mediator() const {
    if (mediator_ == nullptr) {
        throw std::logic_error(name_ + " must join a server before chatting");
    }
    return *mediator_;
}

}  // namespace campus::chat
