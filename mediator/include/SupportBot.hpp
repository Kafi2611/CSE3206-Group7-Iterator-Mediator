#pragma once
/**
 * @file   SupportBot.hpp
 * @brief  <<ConcreteColleague>> - an automated help-desk account.
 *
 * Shows that colleagues of DIFFERENT types plug into the same mediator.
 * The bot replies through the mediator, exactly like a human would.
 */

#include <iostream>
#include <ostream>
#include <string>

#include "User.hpp"

namespace campus::chat {

class SupportBot final : public User {
public:
    SupportBot(std::string id, std::string name, std::ostream& out = std::cout);

    bool isAutomated() const noexcept override { return true; }

    /// Opens a ticket and auto-replies to direct messages from humans.
    /// Ignores group chatter and messages from other bots (no reply loops).
    void receive(const Message& message) override;

    int ticketsOpened() const noexcept { return ticketsOpened_; }

private:
    std::ostream& out_;
    int           ticketsOpened_ = 0;
    int           nextTicket_    = 1001;
};

}  // namespace campus::chat
