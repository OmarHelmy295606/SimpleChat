#include <QApplication>
#include <iostream>
#include "chatclient.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
	std::cout << "This is the client" << std::endl;
    return app.exec();
}
