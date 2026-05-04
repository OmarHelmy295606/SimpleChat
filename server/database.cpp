#include "database.h"
#include <iostream>
#include <stdexcept>

DatabaseManager::DatabaseManager(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "[DB] Cannot open database: " << sqlite3_errmsg(db_) << "\n";
        sqlite3_close(db_);
        db_   = nullptr;
        open_ = false;
        return;
    }
    open_ = true;
    createSchema();
    std::cout << "[DB] Opened database: " << dbPath << "\n";
}

DatabaseManager::~DatabaseManager() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool DatabaseManager::isOpen() const { return open_; }


void DatabaseManager::createSchema() {
    const char* sql = R"SQL(
        CREATE TABLE IF NOT EXISTS messages (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            type        TEXT    NOT NULL DEFAULT 'chat',
            sender      TEXT    NOT NULL,
            recipient   TEXT    NOT NULL,
            content     TEXT    NOT NULL,
            timestamp   TEXT    NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_sender    ON messages(sender);
        CREATE INDEX IF NOT EXISTS idx_recipient ON messages(recipient);
        CREATE INDEX IF NOT EXISTS idx_timestamp ON messages(timestamp);
    )SQL";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "[DB] Schema creation failed: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
}


bool DatabaseManager::saveMessage(const Message& msg) {
    if (!open_) return false;

    const char* sql =
        "INSERT INTO messages (type, sender, recipient, content, timestamp) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[DB] Prepare failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, msg.type.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, msg.sender.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, msg.recipient.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, msg.content.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, msg.timestamp.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "[DB] Insert failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    return true;
}

std::vector<Message> DatabaseManager::collectRows(sqlite3_stmt* stmt) {
    std::vector<Message> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Message m;
        auto col = [&](int i) -> std::string {
            const unsigned char* txt = sqlite3_column_text(stmt, i);
            return txt ? reinterpret_cast<const char*>(txt) : "";
        };
        m.type      = col(0);
        m.sender    = col(1);
        m.recipient = col(2);
        m.content   = col(3);
        m.timestamp = col(4);
        results.push_back(m);
    }
    sqlite3_finalize(stmt);
    return results;
}

std::vector<Message> DatabaseManager::getConversation(
        const std::string& userA, const std::string& userB, int limit) const {

    if (!open_) return {};

    const char* sql =
        "SELECT type, sender, recipient, content, timestamp FROM messages "
        "WHERE (sender = ? AND recipient = ?) OR (sender = ? AND recipient = ?) "
        "ORDER BY timestamp ASC "
        "LIMIT ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {};

    sqlite3_bind_text(stmt, 1, userA.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, userB.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, userB.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, userA.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 5, limit);

    return collectRows(stmt);
}

std::vector<Message> DatabaseManager::getRecentMessages(int limit) const {
    if (!open_) return {};

    const char* sql =
        "SELECT type, sender, recipient, content, timestamp FROM messages "
        "ORDER BY timestamp DESC LIMIT ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {};

    sqlite3_bind_int(stmt, 1, limit);
    return collectRows(stmt);
}

std::vector<Message> DatabaseManager::getMessagesBySender(
        const std::string& sender, int limit) const {

    if (!open_) return {};

    const char* sql =
        "SELECT type, sender, recipient, content, timestamp FROM messages "
        "WHERE sender = ? ORDER BY timestamp ASC LIMIT ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {};

    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, limit);
    return collectRows(stmt);
}
