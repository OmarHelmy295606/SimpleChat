#include <gtest/gtest.h>
#include "message.h"
#include <nlohmann/json.hpp>


TEST(MessageSerialize, ChatMessageProducesValidJson) {
    Message msg;
    msg.type      = "chat";
    msg.sender    = "alice";
    msg.recipient = "bob";
    msg.content   = "Hello Bob!";
    msg.timestamp = "2024-01-01 12:00:00";

    std::string raw = msg.serialize();
    auto j = nlohmann::json::parse(raw);

    EXPECT_EQ(j["type"],      "chat");
    EXPECT_EQ(j["sender"],    "alice");
    EXPECT_EQ(j["recipient"], "bob");
    EXPECT_EQ(j["content"],   "Hello Bob!");
    EXPECT_EQ(j["timestamp"], "2024-01-01 12:00:00");
}

TEST(MessageSerialize, LoginMessageSerializesCorrectly) {
    Message msg;
    msg.type      = "login";
    msg.sender    = "alice";
    msg.recipient = "";
    msg.content   = "";
    msg.timestamp = "2024-01-01 09:00:00";

    auto j = nlohmann::json::parse(msg.serialize());
    EXPECT_EQ(j["type"],   "login");
    EXPECT_EQ(j["sender"], "alice");
    EXPECT_EQ(j["recipient"], "");
    EXPECT_EQ(j["content"],   "");
}

TEST(MessageSerialize, SpecialCharactersInContent) {
    Message msg;
    msg.type    = "chat";
    msg.content = "Hello \"World\" & <test> \n newline";
    msg.sender  = "alice";

    std::string raw = msg.serialize();
    // Should round-trip cleanly through JSON
    auto j = nlohmann::json::parse(raw);
    EXPECT_EQ(j["content"], msg.content);
}

TEST(MessageDeserialize, MissingFieldsDefaultToEmptyString) {
    std::string raw = R"({"type":"chat"})";
    Message msg = Message::deserialize(raw);

    EXPECT_EQ(msg.type,      "chat");
    EXPECT_EQ(msg.sender,    "");
    EXPECT_EQ(msg.recipient, "");
    EXPECT_EQ(msg.content,   "");
    EXPECT_EQ(msg.timestamp, "");
}
