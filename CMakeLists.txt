cmake_minimum_required(VERSION 3.16)
project(SimpleChat)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6    REQUIRED COMPONENTS Widgets)
find_package(Boost  REQUIRED COMPONENTS system)
find_package(SQLite3 REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(server
    server/main.cpp
    server/server.cpp
    server/database.cpp
)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_link_libraries(server Boost::system SQLite::SQLite3 pthread)

add_library(server_lib STATIC
    server/server.cpp
    server/database.cpp
)
target_include_directories(server_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/server
)
target_link_libraries(server_lib Boost::system SQLite::SQLite3 pthread)

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
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_link_libraries(client Qt6::Widgets Boost::system pthread)

add_library(client_lib STATIC client/chatclient.cpp)
target_include_directories(client_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/client
)
target_link_libraries(client_lib Qt6::Widgets Boost::system pthread)

enable_testing()

add_executable(simplechat_tests
    tests/test_message.cpp
    tests/test_server.cpp
    tests/test_client.cpp
    tests/test_database.cpp
    server/database.cpp
)
target_include_directories(simplechat_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/server
    ${CMAKE_SOURCE_DIR}/client
)
target_link_libraries(simplechat_tests
    server_lib
    client_lib
    GTest::gtest_main
    GTest::gmock_main
    Qt6::Widgets
    Boost::system
    SQLite::SQLite3
    pthread
)

include(GoogleTest)
gtest_discover_tests(simplechat_tests)
