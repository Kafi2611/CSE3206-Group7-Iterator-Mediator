/**
 * @file   test_mediator.cpp
 * @brief  Unit tests for the Mediator pattern (CampusConnect Messenger).
 */

#include <sstream>
#include <stdexcept>
#include <string>

#include "Member.hpp"
#include "MessengerServer.hpp"
#include "SupportBot.hpp"
#include "TestFramework.hpp"

using namespace campus::chat;

namespace {

/// A server with three members and a bot, all joined and online.
/// Notifications go to a private stream so test output stays clean.
struct Fixture {
    std::ostringstream out;
    MessengerServer    server{"TestServer"};
    Member             kafi{"kafi", "Kafi", out};
    Member             sadaf{"sadaf", "Sadaf", out};
    Member             sakila{"sakila", "Sakila", out};
    SupportBot         bot{"bot", "HelpBot", out};

    Fixture() {
        kafi.join(server);
        sadaf.join(server);
        sakila.join(server);
        bot.join(server);
    }
};

}  // namespace

// ---- direct messages -------------------------------------------------------------

TEST_CASE(direct_message_to_online_user_is_delivered) {
    Fixture f;
    CHECK(f.kafi.sendTo("sadaf", "hi") == DeliveryStatus::Delivered);
    CHECK_EQ(f.sadaf.inbox().size(), 1u);
    CHECK_EQ(f.sadaf.inbox()[0].fromId, std::string("kafi"));
    CHECK_EQ(f.sadaf.inbox()[0].text, std::string("hi"));
    CHECK(f.kafi.inbox().empty());  // sender does not receive its own message
}

TEST_CASE(message_to_offline_user_is_queued_then_flushed_in_order) {
    Fixture f;
    f.sakila.goOffline();
    CHECK(f.kafi.sendTo("sakila", "one") == DeliveryStatus::Queued);
    CHECK(f.sadaf.sendTo("sakila", "two") == DeliveryStatus::Queued);
    CHECK_EQ(f.server.pendingCount("sakila"), 2u);
    CHECK(f.sakila.inbox().empty());

    f.sakila.goOnline();
    CHECK_EQ(f.server.pendingCount("sakila"), 0u);
    CHECK_EQ(f.sakila.inbox().size(), 2u);
    CHECK_EQ(f.sakila.inbox()[0].text, std::string("one"));
    CHECK_EQ(f.sakila.inbox()[1].text, std::string("two"));
}

TEST_CASE(blocked_sender_cannot_reach_recipient) {
    Fixture f;
    f.sakila.block("kafi");
    CHECK(f.kafi.sendTo("sakila", "hello?") == DeliveryStatus::Blocked);
    CHECK(f.sakila.inbox().empty());
    CHECK(f.sadaf.sendTo("sakila", "hey") == DeliveryStatus::Delivered);  // others fine
}

TEST_CASE(invalid_direct_messages_are_rejected) {
    Fixture f;
    CHECK_THROWS_AS(f.kafi.sendTo("zara", "hi"), std::invalid_argument);
    CHECK_THROWS_AS(f.kafi.sendTo("sadaf", ""), std::invalid_argument);
    CHECK_THROWS_AS(f.kafi.block("kafi"), std::invalid_argument);
}

TEST_CASE(going_online_with_nothing_queued_is_harmless) {
    Fixture f;
    f.kafi.goOffline();
    f.kafi.goOnline();
    CHECK_EQ(f.server.pendingCount("kafi"), 0u);
    CHECK(f.kafi.inbox().empty());
}

// ---- group messages ----------------------------------------------------------------

TEST_CASE(group_message_reaches_all_members_except_sender) {
    Fixture f;
    f.server.createGroup("g7", {"kafi", "sadaf", "sakila"});
    CHECK_EQ(f.kafi.sendToGroup("g7", "meeting at 8"), 2u);
    CHECK_EQ(f.sadaf.inbox().size(), 1u);
    CHECK_EQ(f.sakila.inbox().size(), 1u);
    CHECK(f.kafi.inbox().empty());
    CHECK(f.sadaf.inbox()[0].isGroupMessage());
    CHECK_EQ(f.sadaf.inbox()[0].groupId, std::string("g7"));
}

TEST_CASE(group_message_respects_block_list_and_offline_queue) {
    Fixture f;
    f.server.createGroup("g7", {"kafi", "sadaf", "sakila"});
    f.sadaf.block("kafi");
    f.sakila.goOffline();
    CHECK_EQ(f.kafi.sendToGroup("g7", "hi"), 1u);  // only sakila, and queued
    CHECK(f.sadaf.inbox().empty());
    CHECK_EQ(f.server.pendingCount("sakila"), 1u);
}

TEST_CASE(non_member_cannot_post_in_group) {
    Fixture f;
    f.server.createGroup("g7", {"kafi", "sadaf"});
    CHECK_THROWS_AS(f.sakila.sendToGroup("g7", "let me in"), PermissionDenied);
    CHECK_THROWS_AS(f.kafi.sendToGroup("nope", "hi"), std::invalid_argument);
}

TEST_CASE(create_group_validates_members) {
    Fixture f;
    CHECK_THROWS_AS(f.server.createGroup("g", {"kafi"}), std::invalid_argument);
    CHECK_THROWS_AS(f.server.createGroup("g", {"kafi", "kafi"}), std::invalid_argument);
    CHECK_THROWS_AS(f.server.createGroup("g", {"kafi", "zara"}), std::invalid_argument);
    f.server.createGroup("g", {"kafi", "sadaf"});
    CHECK_THROWS_AS(f.server.createGroup("g", {"kafi", "sadaf"}), std::invalid_argument);
}

TEST_CASE(group_and_user_input_is_validated) {
    Fixture f;
    f.server.createGroup("g7", {"kafi", "sadaf"});
    CHECK_THROWS_AS(f.kafi.sendToGroup("g7", ""), std::invalid_argument);
    CHECK_THROWS_AS(f.server.createGroup("", {"kafi", "sadaf"}), std::invalid_argument);
    CHECK_THROWS_AS(Member("", "No Id", f.out), std::invalid_argument);
}

// ---- different colleague types ---------------------------------------------------

TEST_CASE(support_bot_replies_through_the_mediator) {
    Fixture f;
    CHECK(f.kafi.sendTo("bot", "help!") == DeliveryStatus::Delivered);
    CHECK_EQ(f.bot.ticketsOpened(), 1);
    CHECK_EQ(f.kafi.inbox().size(), 1u);
    CHECK_EQ(f.kafi.inbox()[0].fromId, std::string("bot"));
    CHECK(f.kafi.inbox()[0].automated);
}

TEST_CASE(support_bot_ignores_group_messages_and_other_bots) {
    Fixture f;
    SupportBot otherBot("bot2", "OtherBot", f.out);
    otherBot.join(f.server);
    f.server.createGroup("g", {"kafi", "bot"});
    f.kafi.sendToGroup("g", "hello group");
    otherBot.sendTo("bot", "ping");  // must NOT start an endless bot-to-bot loop
    CHECK_EQ(f.bot.ticketsOpened(), 0);
}

// ---- membership & lifetime ---------------------------------------------------------

TEST_CASE(user_must_join_before_chatting) {
    std::ostringstream out;
    Member guest("guest", "Guest", out);
    CHECK_THROWS_AS(guest.sendTo("kafi", "hi"), std::logic_error);
}

TEST_CASE(duplicate_ids_and_double_join_are_rejected) {
    Fixture f;
    Member impostor("kafi", "Fake Kafi", f.out);
    CHECK_THROWS_AS(impostor.join(f.server), std::invalid_argument);
    CHECK(!impostor.isConnected());
    CHECK_THROWS_AS(f.kafi.join(f.server), std::logic_error);
}

TEST_CASE(destroyed_user_unregisters_automatically) {
    Fixture f;
    const std::size_t before = f.server.userCount();
    {
        Member temp("temp", "Temp", f.out);
        temp.join(f.server);
        CHECK_EQ(f.server.userCount(), before + 1);
    }  // temp's destructor runs here
    CHECK_EQ(f.server.userCount(), before);
    CHECK_THROWS_AS(f.kafi.sendTo("temp", "still there?"), std::invalid_argument);
}

TEST_CASE(server_rejects_users_registered_elsewhere) {
    Fixture f;
    MessengerServer otherServer("OtherServer");
    // kafi joined f.server, not otherServer: the mediator must refuse him.
    CHECK_THROWS_AS(otherServer.sendDirect(f.kafi, "sadaf", "hi"), std::logic_error);
}

TEST_CASE(history_records_every_accepted_message_once) {
    Fixture f;
    f.server.createGroup("g7", {"kafi", "sadaf", "sakila"});
    f.kafi.sendTo("sadaf", "a");
    f.kafi.sendToGroup("g7", "b");
    f.sakila.block("kafi");
    f.kafi.sendTo("sakila", "blocked");  // not stored
    CHECK_EQ(f.server.history().size(), 2u);
    CHECK(f.server.history()[0].id < f.server.history()[1].id);
}

int main() { return campus::test::runAll(); }
