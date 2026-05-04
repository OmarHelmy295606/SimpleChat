cmake_minimum_required(VERSION 3.16)
project(SimpleChat)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED COMPONENTS system)

add_executable(server
	server/main.cpp
	server/server.cpp
)

target_include_directories(server PRIVATE   ${CMAKE_SOURCE_DIR}/shared)

target_link_libraries(server
	Boost::system
	pthread
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(client
	client/main.cpp
	client/chatclient.cpp
	client/gui/LoginWindow.cpp
	client/gui/MainWindow.cpp
	client/gui/AboutWindow.cpp
	client/gui/LoginWindow.h
	client/gui/AboutWindow.h
	client/gui/MainWindow.h
)

target_include_directories(client PRIVATE   ${CMAKE_SOURCE_DIR}/shared)

target_link_libraries(client
    Qt6::Widgets
    Boost::system
    pthread
)
