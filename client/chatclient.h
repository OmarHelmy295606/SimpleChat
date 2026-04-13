#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <boost/asio.hpp>
#include <thread>
#include <string>
#include "message.h"

using boost::asio::ip::tcp;

class ChatClient : public QObject{
	Q_OBJECT
public:

	explicit ChatClient(QObject* parent = nullptr);
	~ChatClient();
	void connectToServer(const QString& host, int port, const QString& username);
	void sendMessage(const QString& recipient, const QString& content);
	void disconnect();

signals:
	void messageReceived(const Message& message);
	void userListUpdated(const QStringList& users);
	void disconnected();
	void errorOccurred(const QString& error);


private:

	boost::asio::io_context io_;
	tcp::socket socket_;
	boost::asio::streambuf buffer_;
	std::thread networkThread_;
	std::string username_;

	void sendLogin();
	void readMessage();
	void handleMessage(const std::string& raw);
	void sendRaw(const std::string& data);



};
