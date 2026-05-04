#include <QApplication>
#include <QStackedWidget>
#include <iostream>
#include "chatclient.h"
#include "gui/LoginWindow.h"
#include "gui/MainWindow.h"
#include "gui/AboutWindow.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	ChatClient chatClient;

	LoginWindow loginWindow;
	AboutWindow  aboutWindow;
	MainWindow* mainWindow = nullptr;

	QStackedWidget stack;
	stack.addWidget(&loginWindow);
	stack.addWidget(&aboutWindow);
	stack.setCurrentIndex(0);
	stack.setWindowTitle("SimpleChat");
	stack.resize(800, 600);
	stack.show();

	QObject::connect(&loginWindow, &LoginWindow::connectRequested, [&](const QString& host, int port, const QString& username){
		chatClient.connectToServer(host, port, username);
	});


	QObject::connect(&chatClient, &ChatClient::errorOccurred, &loginWindow, &LoginWindow::ErrorOccurred);

	QObject::connect(&chatClient, &ChatClient::userListUpdated, &stack, [&](const QStringList& users){
		if (mainWindow == nullptr) {
			QString username = loginWindow.getUsername();
			mainWindow = new MainWindow(username);
			stack.addWidget(mainWindow);
			stack.setCurrentWidget(mainWindow);

			QObject::connect(&chatClient, &ChatClient::messageReceived, mainWindow, &MainWindow::onMessageReceived);

			QObject::connect(&chatClient, &ChatClient::userListUpdated, mainWindow, &MainWindow::onUserListUpdated);


			QObject::connect(&chatClient, &ChatClient::disconnected, mainWindow, &MainWindow::onDisconnected);


			QObject::connect(mainWindow, &MainWindow::messageSent, [&](const QString& recipient, const QString& content) {
                        chatClient.sendMessage(recipient, content);
                    	});


			QObject::connect(mainWindow, &MainWindow::aboutRequested, [&]() {
                        stack.setCurrentWidget(&aboutWindow);
                    	});

			QObject::connect(&aboutWindow, &AboutWindow::closeRequested, [&]() {
                        stack.setCurrentWidget(mainWindow);
                    	});
		}
		if(mainWindow)
			mainWindow->onUserListUpdated(users);
	}, Qt::QueuedConnection);
    	return app.exec();
}
