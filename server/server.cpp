#include "server.h"
#include <iostream>
#include <nlohmann/json.hpp>

bool Server::hasUser(const std::string& username) const {
	return users_.count(username) > 0;
}

Session::Session(tcp::socket socket, Server& server)
    : socket_(std::move(socket)), server_(server) {}

void Session::start() {
    readMessages();
}

void Session::readMessages() {
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec,
                     [[maybe_unused]] std::size_t len) {
            if (ec) {
                std::cout << username_ << " disconnected: " << ec.message() << "\n";
                server_.removeUser(username_);
                return;
            }
            std::istream stream(&buffer_);
            std::string  line;
            std::getline(stream, line);
            if (!line.empty()) handleMessages(line);
            readMessages();
        });
}

void Session::handleMessages(const std::string& incomingMessage){
try{
        Message msg = Message::deserialize(incomingMessage);

        if(msg.type == "login"){
		if(!username_.empty())
			return;

		if(server_.hasUser(msg.sender)){
			std::cout << "Duplicate username rejected: " << msg.sender << std::endl;
			nlohmann::json j;
			j["type"] = "error";
			j["message"] = "Username already taken";
			deliver(j.dump());
			return;
		}

                username_ = msg.sender;
                server_.registerUser(username_, shared_from_this());
                server_.broadcastUserList();
                std::cout << username_ << " logged in\n";

        }
        else if (msg.type == "chat") {
            server_.persistMessage(msg);

            server_.sendToUser(msg.recipient, incomingMessage);
            deliver(incomingMessage);
        }
        else {
            std::cout << "Unknown message type: " << msg.type << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cout << "Failed to handle message: " << e.what() << "\n";
    }
}

void Session::deliver(const std::string& message) {
    writingQueue_.push_back(message + "\n");
    if (writingQueue_.size() == 1) writeNext();
}

void Session::writeNext() {
    auto self = shared_from_this();
    boost::asio::async_write(socket_,
        boost::asio::buffer(writingQueue_.front()),
        [this, self](boost::system::error_code ec,
                     [[maybe_unused]] std::size_t) {
            if (ec) {
                std::cout << "Write error for " << username_ << ": "
                          << ec.message() << "\n";
                server_.removeUser(username_);
                return;
            }
            writingQueue_.erase(writingQueue_.begin());
            if (!writingQueue_.empty()) writeNext();
        });
}


Server::Server(boost::asio::io_context& io, int port, const std::string& dbPath)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port))
    , db_(dbPath)
{
    std::cout << "SimpleChat server started on port " << port << "\n";
    if (!db_.isOpen())
        std::cerr << "[DB] Warning: running without persistent storage\n";
    acceptNext();
}

void Server::acceptNext() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "New connection\n";
                auto session = std::make_shared<Session>(std::move(socket), *this);
                session->start();
            } else {
                std::cout << "Accept error: " << ec.message() << "\n";
            }
            acceptNext();
        });
}

void Server::registerUser(const std::string& username, std::shared_ptr<Session> session){
        if(users_.count(username)){
        	std::cout << "Duplicate username rejected: " << username << std::endl;
        	nlohmann::json j;
		j["type"] = "error";
		session->deliver(j.dump());
		return;
    	}
	users_[username] = session;
        std::cout << "Registerd: " << username << std::endl;
}

void Server::removeUser(const std::string& username){
        if(users_.erase(username)){
                std::cout << "Removed: "<< username << std::endl;
		broadcastUserList();
        }

}

void Server::sendToUser(const std::string& recipient,
                        const std::string& message) {
    auto it = users_.find(recipient);
    if (it != users_.end())
        it->second->deliver(message);
    else
        std::cerr << "User not found: " << recipient << "\n";
}

void Server::broadcastUserList() {
    nlohmann::json j;
    j["type"]  = "user_list";
    j["users"] = nlohmann::json::array();
    for (auto& [name, _] : users_)
        j["users"].push_back(name);

    std::string msg = j.dump();
    for (auto& [name, session] : users_)
        session->deliver(msg);
}

void Server::persistMessage(const Message& msg) {
    if (!db_.saveMessage(msg))
        std::cerr << "[DB] Failed to save message from " << msg.sender << "\n";
}
