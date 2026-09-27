#pragma once
/**
 * @file   Member.hpp
 * @brief  <<ConcreteColleague>> - a human student using the messenger.
 */

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "User.hpp"

namespace campus::chat {

class Member final : public User {
public:
    /// @param out where "phone notifications" are printed (std::cout or a
    ///            std::ostringstream in unit tests).
    Member(std::string id, std::string name, std::ostream& out = std::cout);

    /// Shows a notification and stores the message in the inbox.
    void receive(const Message& message) override;

    const std::vector<Message>& inbox() const noexcept { return inbox_; }

private:
    std::ostream&        out_;
    std::vector<Message> inbox_;
};

}  // namespace campus::chat
