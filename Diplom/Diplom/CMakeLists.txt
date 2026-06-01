cmake_minimum_required(VERSION 3.10)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED Core Sql Widgets)

# Database Manager (shared)
set(DATABASE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Database/DatabaseManager.cpp
)

set(DATABASE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Database/DatabaseManager.h
)

# Indexer executable
add_executable(Indexer
    ${CMAKE_CURRENT_SOURCE_DIR}/Indexer/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Indexer/Indexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Indexer/Indexer.h
    ${DATABASE_SOURCES}
    ${DATABASE_HEADERS}
)

target_link_libraries(Indexer Qt5::Core Qt5::Sql)

# Searcher executable
add_executable(Searcher
    ${CMAKE_CURRENT_SOURCE_DIR}/Searcher/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Searcher/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Searcher/MainWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Searcher/Searcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Searcher/Searcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Searcher/MainWindow.ui
    ${DATABASE_SOURCES}
    ${DATABASE_HEADERS}
)

target_link_libraries(Searcher Qt5::Core Qt5::Sql Qt5::Widgets)

set_target_properties(Indexer Searcher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)