#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "message.h"

using boost::asio::ip::tcp;

class Server; // to avoid circular dependency

class Session : public std::enable_shared_from_this<Session>{

public:
	Session(tcp::socket socket, Server& server);
	void start();
	void deliver(const std::string& message);

private:


	tcp::socket socket_;
	Server& server_;
	std::string username_;
	boost::asio::streambuf buffer_;
	std::vector<std::string> writingQueue_;

	void readMessages();
	void handleMessages(const std::string& incomingMessage);
	void writeNext();


};

class Server {

public:

	Server(boost::asio::io_context& io, int port);
	void registerUser(const std::string& username, std::shared_ptr<Session> session);
	void removeUser(const std::string& username);
	void sendToUser(const std::string& recipient, const std::string& message);
	void broadcastUserList();
	bool hasUser(const std::string& username) const;



private:

	tcp::acceptor acceptor_;
	std::map<std::string , std::shared_ptr<Session>> users_;

	void acceptNext();




};
