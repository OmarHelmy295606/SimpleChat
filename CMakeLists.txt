cmake_minimum_required(VERSION 3.16)
project(SimpleChat)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED COMPONENETS system)

add_executable(server
	server/main.cpp
	server/server.cpp
)

target_include_directories(server PRIVATE shared)

target_link_libraries(server
	Boost::system
	pthread
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(client
	client/main.cpp
	client/ChatClient.cpp
	client/gui/LoginWindow.cpp
	client/gui/MainWindow.cpp
	client/gui/AboutWindow.cpp
)

target_include_directories(client PRIVATE shared)

target_link_libraries(client
    Qt6::Widgets
    Boost::system
    pthread
)
