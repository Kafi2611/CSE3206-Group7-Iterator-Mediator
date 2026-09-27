/**
 * @file   Member.cpp
 * @brief  <<ConcreteColleague>> implementation.
 */

#include "Member.hpp"

#include <utility>

namespace campus::chat {

Member::Member(std::string id, std::string name, std::ostream& out)
    : User(std::move(id), std::move(name)), out_(out) {}

void Member::receive(const Message& message) {
    out_ << "    [" << name() << "'s phone] ";
    if (message.isGroupMessage()) {
        out_ << '#' << message.groupId << " | ";
    }
    out_ << message.fromName << ": " << message.text << '\n';
    inbox_.push_back(message);
}

}  // namespace campus::chat
