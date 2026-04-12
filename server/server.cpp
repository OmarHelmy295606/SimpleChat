#include "server.h"
#include <iostream>
#include <nlohmann/json.hpp>

// Session class functions
Session::Session(tcp::socket socket, Server& server)
        :socket_(std::move(socket)), server_(server){}
void Session::start(){
        readMessages();
}
void Session::readMessages(){
        auto self = shared_from_this();
        boost::asio::async_read_until(socket_, buffer_, '\n',
                [this, self](boost::system::error_code ec, [[maybe_unused]] std::size_t unusedLength){
                        if(ec){
                                std::cout << username_ << " disconnected: "<< ec.message() << std::endl;
                                server_.removeUser(username_);
                                return;
                        }std::istream stream(&buffer_);
                        std::string line;
                        std::getline(stream, line);

                        if(!line.empty()) handleMessages(line);

                        readMessages();
                        }
                );
}

void Session::handleMessages(const std::string& incomingMessage){
try{
        Message msg = Message::deserialize(incomingMessage);

        if(msg.type == "login"){
                username_ = msg.sender;
                server_.registerUser(username_, shared_from_this());
                server_.broadcastUserList();
                std::cout << username_ << " logged in\n";
        }
        else if(msg.type == "chat"){
                server_.sendToUser(msg.recipient, incomingMessage);
                deliver(incomingMessage);
        }
        else{
                std::cout<< "Unknown message type: " << msg.type << std::endl;
        }
}
catch(const std::exception& e){
        std::cout << "failed to handle the message: " << e.what() << std::endl;
}
}

void Session::deliver(const std::string& message){
        writingQueue_.push_back(message + "\n");
        if(writingQueue_.size() == 1) writeNext();
}

void Session::writeNext(){
        auto self = shared_from_this();

        boost::asio::async_write(socket_,
                boost::asio::buffer(writingQueue_.front()),
                [this, self](boost::system::error_code ec, [[maybe_unused]]std::size_t unused){
                        if(ec){
                                std::cout << "Writing error for " << username_ << ": " << ec.message() << std::endl;
                                server_.removeUser(username_);
                                return;
                        }
                        writingQueue_.erase(writingQueue_.begin());
                        if(!writingQueue_.empty()) writeNext();

        });
}







// Server class functions
Server::Server(boost::asio::io_context& io, int port)
:acceptor_(io, tcp::endpoint(tcp::v4(), port)){
        std:: cout << "SimpleChat server started working on port" << port << std::endl;
        acceptNext();
}
void Server::acceptNext(){
        acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket){
                        if(!ec){
                                std::cout << "New connection is here" << std::endl;
                                auto session = std::make_shared<Session>(std::move(socket), *this);
                                session->start();
                        }else{
                                std::cout << "Errors: " << ec.message() << std::endl;
                        }
                acceptNext();

        });
}

void Server::registerUser(const std::string& username, std::shared_ptr<Session> session){
        users_[username] = session;
        std::cout << "Registerd: " << username << std::endl;
}

void Server::removeUser(const std::string& username){
        if(users_.erase(username)){
                std::cout << "Removed: "<< username << std::endl;
        }
}

void Server::sendToUser(const std::string& recipient, const std::string& message){
        auto it = users_.find(recipient);
        if (it != users_.end()) {
                it->second->deliver(message);
        }
        else{
                std::cerr << "User not found: " << recipient << "\n";
        }
}

void Server::broadcastUserList(){
        nlohmann::json j;
        j["type"]  = "user_list";
        j["users"] = nlohmann::json::array();
        for (auto& [name, session] : users_)
        j["users"].push_back(name);

    std::string msg = j.dump();
    for (auto& [name, session] : users_)
        session->deliver(msg);

}
