#include "chatclient.h"
#include <iostream>
#include <nlohmann/json.hpp>

ChatClient::ChatClient(QObject* parent)
: QObject(parent), socket_(io_){}

ChatClient::~ChatClient(){disconnect();}


void ChatClient::connectToServer(const QString& host, int port, const QString& username){
	username_ = username.toStdString();

	try{
		tcp::resolver resolver(io_);
		auto endpoints = resolver.resolve(host.toStdString() , std::to_string(port));
		boost::asio::connect(socket_, endpoints);

		sendLogin();
		readMessage();

		networkThread_ = std::thread([this](){
				try{
					io_.run();
				}catch(const std::exception& e){
					emit errorOccurred(QString::fromStdString(e.what() ) );
				}
		});

	}catch(const std::exception& e) {
		emit errorOccurred(QString::fromStdString(e.what()));
	}

}

void ChatClient::sendLogin(){
	Message msg;
	msg.type = "login";
	msg.sender = username_;
	msg.recipient = "";
	msg.content = "";
	msg.timestamp = currentTimestamp();

	sendRaw(msg.serialize());
}


void ChatClient::sendMessage(const QString& recipient, const QString& content){
        Message msg;
        msg.type = "chat";
        msg.sender = username_;
        msg.recipient = recipient.toStdString();
        msg.content = content.toStdString();
        msg.timestamp = currentTimestamp();

        sendRaw(msg.serialize());
}


void ChatClient::sendRaw(const std::string& data){
	auto message = std::make_shared<std::string>(data + "\n");
	boost::asio::post(io_, [this, message](){
		boost::asio::async_write(socket_, boost::asio::buffer(*message), [message](boost::system::error_code ec, std::size_t){
			if(ec){
				std::cout<< "Send error: " << ec.message() << std::endl;
			}
		});

	});
}

void ChatClient::readMessage(){
	boost::asio::async_read_until(socket_, buffer_, '\n',
        	[this](boost::system::error_code ec, std::size_t /*length*/) {
            		if (ec) {
                		std::cout << "Disconnected from server: " << ec.message() << std::endl;
                		emit disconnected();
                		return;
            		}
			std::istream stream(&buffer_);
			std::string line;
			std::getline(stream,line);

			if(!line.empty()) handleMessage(line);
		readMessage();

	});
}

void ChatClient::handleMessage(const std::string& raw){
	try{
		auto j = nlohmann::json::parse(raw);
		std::string type = j.value("type", "");

		if(type == "chat"){
			Message msg = Message::deserialize(raw);
			emit messageReceived(msg);
		}
		else if(type == "user_list"){
			QStringList users;
			for(auto& name :j["users"])
				users << QString::fromStdString(name.get<std::string>());
			emit userListUpdated(users);
		}
		else{
			std:: cout << "Unknown message type: " << type << std::endl;
		}

	}

	catch(const std::exception& e){
		std::cout << "Failed to handle message: " << e.what() << std::endl;
	}
}

void ChatClient::disconnect(){
	io_.stop();
	boost::system::error_code ec;
	socket_.close(ec);
	if(networkThread_.joinable())
		networkThread_.join();
}
