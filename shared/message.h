#pragma once
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

inline std::string currentTimeStamp(){
	auto now = std::time(nullptr);
	char buf[20];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return std::string(buf);

}


struct Message{
	std::string type;
	std::string sender;
	std::string recipient;
	std::string content;
	std::string timestamp;

    std::string serialize() const {
        nlohmann::json j;
        j["type"]      = type;
        j["sender"]    = sender;
        j["recipient"] = recipient;
        j["content"]   = content;
        j["timestamp"] = timestamp;
        return j.dump();
    }
    static Message deserialize(const std::string& raw) {
        auto j = nlohmann::json::parse(raw);
        Message m;
        m.type      = j.value("type",      "");
        m.sender    = j.value("sender",    "");
	m.recipient = j.value("recipient", "");
        m.content   = j.value("content",   "");
        m.timestamp = j.value("timestamp", "");
        return m;
    }

};
