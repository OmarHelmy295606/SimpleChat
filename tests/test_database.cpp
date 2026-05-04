#include <gtest/gtest.h>
#include "database.h"
#include "message.h"


class DatabaseTest : public ::testing::Test {
protected:
    DatabaseManager db{":memory:"};

    Message makeMsg(const std::string& sender,
                    const std::string& recipient,
                    const std::string& content,
                    const std::string& timestamp = "2024-01-01 10:00:00") {
        Message m;
        m.type      = "chat";
        m.sender    = sender;
        m.recipient = recipient;
        m.content   = content;
        m.timestamp = timestamp;
        return m;
    }
};
TEST_F(DatabaseTest, SaveMessageReturnsTrueOnSuccess) {
    Message msg = makeMsg("alice", "bob", "Hello!");
    EXPECT_TRUE(db.saveMessage(msg));
}

TEST_F(DatabaseTest, SaveMultipleMessagesDoesNotFail) {
    EXPECT_TRUE(db.saveMessage(makeMsg("alice", "bob", "Hi")));
    EXPECT_TRUE(db.saveMessage(makeMsg("bob",   "alice", "Hey")));
    EXPECT_TRUE(db.saveMessage(makeMsg("alice", "bob", "How are you?")));
}


TEST_F(DatabaseTest, GetConversationReturnsMessagesInBothDirections) {
    db.saveMessage(makeMsg("alice", "bob",   "Hello Bob",  "2024-01-01 10:00:00"));
    db.saveMessage(makeMsg("bob",   "alice", "Hello Alice","2024-01-01 10:01:00"));
    db.saveMessage(makeMsg("carol", "bob",   "Hi Bob",     "2024-01-01 10:02:00"));

    auto msgs = db.getConversation("alice", "bob");
    ASSERT_EQ(msgs.size(), 2u);
    EXPECT_EQ(msgs[0].sender, "alice");
    EXPECT_EQ(msgs[1].sender, "bob");
}

TEST_F(DatabaseTest, GetRecentMessagesReturnsAllWhenUnderLimit) {
    db.saveMessage(makeMsg("alice", "bob",   "first"));
    db.saveMessage(makeMsg("carol", "dave",  "second"));

    auto msgs = db.getRecentMessages(50);
    EXPECT_EQ(msgs.size(), 2u);
}
