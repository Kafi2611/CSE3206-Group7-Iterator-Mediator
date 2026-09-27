/**
 * @file   MessengerServer.cpp
 * @brief  <<ConcreteMediator>> implementation - the single home of all
 *         "who may talk to whom, and how" rules.
 */

#include "MessengerServer.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "User.hpp"

namespace campus::chat {

MessengerServer::MessengerServer(std::string serverName)
    : serverName_(std::move(serverName)) {}

// ---- membership --------------------------------------------------------------

void MessengerServer::registerUser(User& user) {
    if (users_.count(user.id()) > 0) {
        throw std::invalid_argument("User id already taken: " + user.id());
    }
    users_[user.id()] = &user;
}

void MessengerServer::unregisterUser(const std::string& userId) noexcept {
    users_.erase(userId);
    pending_.erase(userId);  // group membership is kept for when they return
}

// ---- interactions --------------------------------------------------------------

DeliveryStatus MessengerServer::sendDirect(const User& from, const std::string& toId,
                                           const std::string& text) {
    requireRegistered(from);
    User& recipient = requireUser(toId);
    if (text.empty()) {
        throw std::invalid_argument("Cannot send an empty message");
    }
    if (isBlocked(toId, from.id())) {
        return DeliveryStatus::Blocked;
    }
    const Message message = makeMessage(from, toId, "", text);
    history_.push_back(message);
    return deliver(recipient, message);
}

std::size_t MessengerServer::sendToGroup(const User& from, const std::string& groupId,
                                         const std::string& text) {
    requireRegistered(from);
    const auto group = groups_.find(groupId);
    if (group == groups_.end()) {
        throw std::invalid_argument("Unknown group: " + groupId);
    }
    // Copy: a receive() callback must not be able to invalidate our loop.
    const std::vector<std::string> members = group->second;
    if (std::find(members.begin(), members.end(), from.id()) == members.end()) {
        throw PermissionDenied(from.name() + " is not a member of #" + groupId);
    }
    if (text.empty()) {
        throw std::invalid_argument("Cannot send an empty message");
    }

    const Message record = makeMessage(from, groupId, groupId, text);
    history_.push_back(record);

    std::size_t reached = 0;
    for (const std::string& memberId : members) {
        const auto member = users_.find(memberId);
        const bool isSender   = memberId == from.id();
        const bool isOnServer = member != users_.end();
        if (isSender || !isOnServer || isBlocked(memberId, from.id())) {
            continue;
        }
        Message copy = record;
        copy.toId = memberId;
        deliver(*member->second, copy);
        ++reached;
    }
    return reached;
}

void MessengerServer::blockUser(const User& blocker, const std::string& blockedId) {
    requireRegistered(blocker);
    requireUser(blockedId);
    if (blocker.id() == blockedId) {
        throw std::invalid_argument("A user cannot block themselves");
    }
    blocked_[blocker.id()].insert(blockedId);
}

void MessengerServer::presenceChanged(const User& user) {
    requireRegistered(user);
    if (!user.isOnline()) {
        return;
    }
    const auto queue = pending_.find(user.id());
    if (queue == pending_.end()) {
        return;
    }
    // Move the queue out first so deliveries can safely enqueue new messages.
    const std::vector<Message> waiting = std::move(queue->second);
    pending_.erase(queue);
    User& recipient = *users_.at(user.id());
    for (const Message& message : waiting) {
        recipient.receive(message);
    }
}

// ---- administration ------------------------------------------------------------

void MessengerServer::createGroup(const std::string& groupId,
                                  const std::vector<std::string>& memberIds) {
    if (groupId.empty()) {
        throw std::invalid_argument("Group id must not be empty");
    }
    if (groups_.count(groupId) > 0) {
        throw std::invalid_argument("Group already exists: " + groupId);
    }
    const std::set<std::string> unique(memberIds.begin(), memberIds.end());
    if (unique.size() != memberIds.size()) {
        throw std::invalid_argument("Duplicate member in group: " + groupId);
    }
    if (unique.size() < 2) {
        throw std::invalid_argument("A group needs at least 2 members");
    }
    for (const std::string& memberId : memberIds) {
        requireUser(memberId);
    }
    groups_[groupId] = memberIds;
}

std::size_t MessengerServer::pendingCount(const std::string& userId) const {
    const auto queue = pending_.find(userId);
    return queue == pending_.end() ? 0 : queue->second.size();
}

// ---- helpers -------------------------------------------------------------------

User& MessengerServer::requireUser(const std::string& userId) const {
    const auto user = users_.find(userId);
    if (user == users_.end()) {
        throw std::invalid_argument("Unknown user: " + userId);
    }
    return *user->second;
}

void MessengerServer::requireRegistered(const User& user) const {
    const auto entry = users_.find(user.id());
    if (entry == users_.end() || entry->second != &user) {
        throw std::logic_error(user.name() + " is not registered on " + serverName_);
    }
}

bool MessengerServer::isBlocked(const std::string& recipientId,
                                const std::string& senderId) const {
    const auto list = blocked_.find(recipientId);
    return list != blocked_.end() && list->second.count(senderId) > 0;
}

DeliveryStatus MessengerServer::deliver(User& recipient, const Message& message) {
    if (recipient.isOnline()) {
        recipient.receive(message);
        return DeliveryStatus::Delivered;
    }
    pending_[recipient.id()].push_back(message);
    return DeliveryStatus::Queued;
}

Message MessengerServer::makeMessage(const User& from, const std::string& toId,
                                     const std::string& groupId,
                                     const std::string& text) {
    Message message;
    message.id        = nextMessageId_++;
    message.fromId    = from.id();
    message.fromName  = from.name();
    message.toId      = toId;
    message.groupId   = groupId;
    message.text      = text;
    message.automated = from.isAutomated();
    return message;
}

}  // namespace campus::chat
