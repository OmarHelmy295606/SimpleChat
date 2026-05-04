#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "message.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <stdexcept>

using ::testing::_;
using ::testing::ElementsAre;

class INetworkSender {
public:
    virtual ~INetworkSender() = default;
    virtual void sendRaw(const std::string& data) = 0;
};

class MockNetworkSender : public INetworkSender {
public:
    MOCK_METHOD(void, sendRaw, (const std::string& data), (override));
};

class ClientMessageHandler {
public:
    explicit ClientMessageHandler(INetworkSender& sender, const std::string& username)
        : sender_(sender), username_(username) {}


    void sendLogin() {
        Message msg;
        msg.type      = "login";
        msg.sender    = username_;
        msg.recipient = "";
        msg.content   = "";
        msg.timestamp = currentTimestamp();
        sender_.sendRaw(msg.serialize());
    }


    void sendMessage(const std::string& recipient, const std::string& content) {
        Message msg;
        msg.type      = "chat";
        msg.sender    = username_;
        msg.recipient = recipient;
        msg.content   = content;
        msg.timestamp = currentTimestamp();
        sender_.sendRaw(msg.serialize());
    }

    enum class EventType { Chat, UserList, Unknown, ParseError };

    struct HandleResult {
        EventType   event;
        Message     message;        
        std::vector<std::string> users;
    };

    HandleResult handleMessage(const std::string& raw) {
        HandleResult result;
        try {
            auto j = nlohmann::json::parse(raw);
            std::string type = j.value("type", "");

            if (type == "chat") {
                result.event   = EventType::Chat;
                result.message = Message::deserialize(raw);
            } else if (type == "user_list") {
                result.event = EventType::UserList;
                for (auto& name : j["users"])
                    result.users.push_back(name.get<std::string>());
            } else {
                result.event = EventType::Unknown;
            }
        } catch (...) {
            result.event = EventType::ParseError;
        }
        return result;
    }

private:
    INetworkSender& sender_;
    std::string     username_;
};


class ClientHandlerTest : public ::testing::Test {
protected:
    MockNetworkSender   mockSender;
    ClientMessageHandler handler{mockSender, "alice"};
};

TEST_F(ClientHandlerTest, SendLoginSendsLoginTypeMessage) {
    std::string captured;
    EXPECT_CALL(mockSender, sendRaw(_))
        .WillOnce([&](const std::string& data) { captured = data; });

    handler.sendLogin();

    auto j = nlohmann::json::parse(captured);
    EXPECT_EQ(j["type"],   "login");
    EXPECT_EQ(j["sender"], "alice");
    EXPECT_EQ(j["recipient"], "");
    EXPECT_EQ(j["content"],   "");
    EXPECT_TRUE(j.contains("timestamp"));
}


TEST_F(ClientHandlerTest, SendMessageBuildsChatPayload) {
    std::string captured;
    EXPECT_CALL(mockSender, sendRaw(_))
        .WillOnce([&](const std::string& data) { captured = data; });

    handler.sendMessage("bob", "Hey Bob!");

    auto j = nlohmann::json::parse(captured);
    EXPECT_EQ(j["type"],      "chat");
    EXPECT_EQ(j["sender"],    "alice");
    EXPECT_EQ(j["recipient"], "bob");
    EXPECT_EQ(j["content"],   "Hey Bob!");
}


TEST_F(ClientHandlerTest, HandleMessageParsesChat) {
    std::string raw = R"({"type":"chat","sender":"bob","recipient":"alice","content":"Hello!","timestamp":"2024-01-01 10:00:00"})";
    auto result = handler.handleMessage(raw);

    EXPECT_EQ(result.event, ClientMessageHandler::EventType::Chat);
    EXPECT_EQ(result.message.sender,  "bob");
    EXPECT_EQ(result.message.content, "Hello!");
}
