cmake_minimum_required(VERSION 3.5)

project(DataBase LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)
find_package(Qt5 REQUIRED COMPONENTS Core Network Sql)

add_executable(DataBase
  main.cpp
)
target_link_libraries(DataBase
    Qt5::Core
    Qt5::Core
    Qt5::Network
    Qt5::Sql)

