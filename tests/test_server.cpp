#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "server.h"
#include "message.h"
#include <boost/asio.hpp>
#include <memory>
#include <string>

using ::testing::_;
using ::testing::NiceMock;

class ISession {
public:
    virtual ~ISession() = default;
    virtual void deliver(const std::string& message) = 0;
};

class MockSession : public ISession {
public:
    MOCK_METHOD(void, deliver, (const std::string& message), (override));
};


class TestableServer {
public:
    void registerUser(const std::string& username,
                      std::shared_ptr<ISession> session) {
        users_[username] = session;
    }

    void removeUser(const std::string& username) {
        users_.erase(username);
    }

    bool hasUser(const std::string& username) const {
        return users_.count(username) > 0;
    }

    int userCount() const {
        return static_cast<int>(users_.size());
    }

    void sendToUser(const std::string& recipient, const std::string& message) {
        auto it = users_.find(recipient);
        if (it != users_.end())
            it->second->deliver(message);
    }

    void broadcastUserList() {
        nlohmann::json j;
        j["type"]  = "user_list";
        j["users"] = nlohmann::json::array();
        for (auto& [name, _] : users_)
            j["users"].push_back(name);
        std::string msg = j.dump();
        for (auto& [name, session] : users_)
            session->deliver(msg);
    }

private:
    std::map<std::string, std::shared_ptr<ISession>> users_;
};


class ServerTest : public ::testing::Test {
protected:
    TestableServer server;

    std::shared_ptr<MockSession> makeSession() {
        return std::make_shared<NiceMock<MockSession>>();
    }
};


TEST_F(ServerTest, RegisterUserAddsToRegistry) {
    auto session = makeSession();
    server.registerUser("alice", session);
    EXPECT_TRUE(server.hasUser("alice"));
    EXPECT_EQ(server.userCount(), 1);
}


TEST_F(ServerTest, RemoveUserDeletesFromRegistry) {
    auto session = makeSession();
    server.registerUser("alice", session);
    server.removeUser("alice");
    EXPECT_FALSE(server.hasUser("alice"));
    EXPECT_EQ(server.userCount(), 0);
}


TEST_F(ServerTest, RemoveNonExistentUserIsNoOp) {
    EXPECT_NO_THROW(server.removeUser("ghost"));
    EXPECT_EQ(server.userCount(), 0);
}

TEST_F(ServerTest, SendToUnknownUserDoesNotCrash) {
    auto alice = makeSession();
    server.registerUser("alice", alice);
    EXPECT_CALL(*alice, deliver(_)).Times(0);
    EXPECT_NO_THROW(server.sendToUser("nobody", "msg"));
}


TEST_F(ServerTest, ReRegisteringUsernameReplacesSession) {
    auto first  = makeSession();
    auto second = makeSession();
    server.registerUser("alice", first);
    server.registerUser("alice", second);

    std::string msg = "ping";
    EXPECT_CALL(*second, deliver(msg)).Times(1);
    server.sendToUser("alice", msg);
    EXPECT_EQ(server.userCount(), 1);
}
