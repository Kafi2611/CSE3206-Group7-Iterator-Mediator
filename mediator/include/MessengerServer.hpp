#pragma once
/**
 * @file   MessengerServer.hpp
 * @brief  <<ConcreteMediator>> - the CampusConnect Messenger server.
 *
 * ALL interaction rules live here and nowhere else:
 *   - routing direct messages
 *   - fan-out of group messages (sender excluded)
 *   - block lists
 *   - offline queue, flushed when the user comes back online
 *   - message history (audit log)
 * Change a rule -> change only this class. Users stay untouched.
 */

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "ChatMediator.hpp"
#include "Message.hpp"

namespace campus::chat {

class MessengerServer final : public ChatMediator {
public:
    explicit MessengerServer(std::string serverName);

    // ---- <<Mediator>> interface --------------------------------------------
    void registerUser(User& user) override;
    void unregisterUser(const std::string& userId) noexcept override;

    DeliveryStatus sendDirect(const User& from, const std::string& toId,
                              const std::string& text) override;
    std::size_t    sendToGroup(const User& from, const std::string& groupId,
                               const std::string& text) override;
    void           blockUser(const User& blocker, const std::string& blockedId) override;
    void           presenceChanged(const User& user) override;

    // ---- administration ----------------------------------------------------
    void createGroup(const std::string& groupId,
                     const std::vector<std::string>& memberIds);

    // ---- queries -----------------------------------------------------------
    const std::string&          serverName() const noexcept { return serverName_; }
    std::size_t                 userCount() const noexcept { return users_.size(); }
    std::size_t                 pendingCount(const std::string& userId) const;
    const std::vector<Message>& history() const noexcept { return history_; }

private:
    User& requireUser(const std::string& userId) const;
    void  requireRegistered(const User& user) const;
    bool  isBlocked(const std::string& recipientId, const std::string& senderId) const;
    DeliveryStatus deliver(User& recipient, const Message& message);
    Message makeMessage(const User& from, const std::string& toId,
                        const std::string& groupId, const std::string& text);

    std::string                                  serverName_;
    std::map<std::string, User*>                 users_;    ///< non-owning
    std::map<std::string, std::vector<std::string>> groups_;
    std::map<std::string, std::set<std::string>> blocked_;  ///< blocker -> ids
    std::map<std::string, std::vector<Message>>  pending_;  ///< offline inbox
    std::vector<Message>                         history_;
    int                                          nextMessageId_ = 1;
};

}  // namespace campus::chat
