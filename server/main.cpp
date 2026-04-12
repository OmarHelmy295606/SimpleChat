#include <boost/asio.hpp>
#include <iostream>
#include "server.h"

int main() {
    try {
        boost::asio::io_context io;
        Server server(io, 12345);
        std::cout << "Press Ctrl+C to stop the server\n";
        io.run();
    } catch (const std::exception& e) {
        std::cerr << "Server crashed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
