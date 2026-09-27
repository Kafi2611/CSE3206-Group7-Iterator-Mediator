/**
 * @file   main.cpp  (mediator demo)
 * @brief  Live demonstration of the Mediator pattern on CampusConnect
 *         Messenger, a Messenger-like chat service.
 *
 * CSE 3206 - Lab 3 - Group 7 (Iterator & Mediator)
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include "Member.hpp"
#include "MessengerServer.hpp"
#include "SupportBot.hpp"

using namespace campus::chat;

namespace {

void banner(const std::string& text) {
    std::cout << "\n==== " << text << " ====\n";
}

void report(const std::string& action, DeliveryStatus status) {
    std::cout << "  " << action << "  ->  " << toString(status) << '\n';
}

}  // namespace

int main() {
    std::cout << "CampusConnect Messenger - Mediator Pattern Demo (CSE 3206, Group 7)\n";

    // The server is declared FIRST so it outlives every user (users
    // unregister themselves in their destructor).
    MessengerServer server("CampusConnect Messenger");  // <<ConcreteMediator>>

    Member     kafi("kafi", "Kafi");                     // <<ConcreteColleague>>
    Member     sadaf("sadaf", "Sadaf");
    Member     sakila("sakila", "Sakila");
    Member     rafi("rafi", "Rafi");
    SupportBot helpDesk("helpdesk", "RUET HelpDesk Bot");  // another colleague type

    banner("1. Everyone joins the server (users know ONLY the server)");
    kafi.join(server);
    sadaf.join(server);
    sakila.join(server);
    rafi.join(server);
    helpDesk.join(server);
    std::cout << "  " << server.userCount() << " accounts registered on "
              << server.serverName() << '\n';

    banner("2. Direct message: both users online");
    report("Kafi -> Sadaf", kafi.sendTo("sadaf", "Did you finish the Mediator UML?"));
    report("Sadaf -> Kafi", sadaf.sendTo("kafi", "Yes! Pushing it to GitHub now."));

    banner("3. Offline delivery: the server queues the message");
    sakila.goOffline();
    report("Kafi -> Sakila", kafi.sendTo("sakila", "Presentation practice at 8 PM?"));
    std::cout << "  Messages waiting for Sakila: " << server.pendingCount("sakila") << '\n';
    std::cout << "  Sakila comes online...\n";
    sakila.goOnline();  // server flushes the queue
    std::cout << "  Messages waiting for Sakila: " << server.pendingCount("sakila") << '\n';

    banner("4. Group chat #Group-7 (sender is not echoed)");
    server.createGroup("Group-7", {"kafi", "sadaf", "sakila"});
    const std::size_t reached =
        sadaf.sendToGroup("Group-7", "Code review report is ready, please check.");
    std::cout << "  Sadaf's group message reached " << reached << " members\n";

    banner("5. Blocking: rule enforced centrally by the server");
    sakila.block("rafi");
    report("Rafi -> Sakila", rafi.sendTo("sakila", "Send me your lab report?"));

    banner("6. A different colleague type: SupportBot replies via the server");
    report("Kafi -> HelpDesk", kafi.sendTo("helpdesk", "My lab PC won't compile C++17"));

    banner("7. Exception handling: invalid requests are rejected");
    try {
        rafi.sendToGroup("Group-7", "Hi everyone!");
    } catch (const PermissionDenied& error) {
        std::cout << "  Caught PermissionDenied: " << error.what() << '\n';
    }
    try {
        kafi.sendTo("zara", "Hello?");
    } catch (const std::invalid_argument& error) {
        std::cout << "  Caught invalid_argument: " << error.what() << '\n';
    }
    try {
        Member guest("guest", "Guest");
        guest.sendTo("kafi", "Hi, I never joined the server");
    } catch (const std::logic_error& error) {
        std::cout << "  Caught logic_error: " << error.what() << '\n';
    }

    banner("8. Server statistics");
    std::cout << "  Messages stored in history : " << server.history().size() << '\n';
    std::cout << "  Kafi's inbox               : " << kafi.inbox().size() << " messages\n";
    std::cout << "  Tickets opened by the bot  : " << helpDesk.ticketsOpened() << '\n';

    std::cout << "\nDemo finished successfully.\n";
    return 0;
}
