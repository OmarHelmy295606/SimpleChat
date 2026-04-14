#include <QApplication>
#include <iostream>
#include "chatclient.h"
#include "gui/LoginWindow.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	std::cout << "This is the client" << std::endl;
	LoginWindow loginWindow;
	loginWindow.show();

	ChatClient client;

	QObject::connect(&loginWindow, &LoginWindow::connectRequested, &client, &ChatClient::connectToServer);

	QObject::connect(&client, &ChatClient::connected, &loginWindow, &LoginWindow::onConnected);

	QObject::connect(&client, &ChatClient::errorOccurred, &loginWindow, &LoginWindow::ErrorOccurred);

    	return app.exec();
}
