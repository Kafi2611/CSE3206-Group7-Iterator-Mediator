/**
 * @file   SupportBot.cpp
 * @brief  <<ConcreteColleague>> implementation.
 */

#include "SupportBot.hpp"

#include <utility>

namespace campus::chat {

SupportBot::SupportBot(std::string id, std::string name, std::ostream& out)
    : User(std::move(id), std::move(name)), out_(out) {}

void SupportBot::receive(const Message& message) {
    if (message.automated || message.isGroupMessage()) {
        return;  // never answer bots or group chatter -> no infinite loops
    }
    const int ticket = nextTicket_++;
    ++ticketsOpened_;
    out_ << "    [" << name() << "] opened ticket #" << ticket << " for "
         << message.fromName << '\n';

    // The reply goes back THROUGH the mediator, never directly to the sender.
    sendTo(message.fromId, "Hi " + message.fromName + ", ticket #" +
                               std::to_string(ticket) +
                               " created. A moderator will reply soon.");
}

}  // namespace campus::chat
