#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include "message.h"

class DatabaseManager {
public:
    explicit DatabaseManager(const std::string& dbPath = "chat_history.db");
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&)            = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool isOpen() const;

    bool saveMessage(const Message& msg);

    std::vector<Message> getConversation(const std::string& userA,
                                         const std::string& userB,
                                         int limit = 100) const;

    std::vector<Message> getRecentMessages(int limit = 50) const;

    std::vector<Message> getMessagesBySender(const std::string& sender,
                                              int limit = 100) const;

private:
    sqlite3*    db_     = nullptr;
    bool        open_   = false;

    void createSchema();
    static std::vector<Message> collectRows(sqlite3_stmt* stmt);
};
