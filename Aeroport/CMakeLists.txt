cmake_minimum_required(VERSION 3.10)
project(AirportInspector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql PrintSupport)

add_executable(AirportInspector
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    statsdialog.h
    statsdialog.cpp
    statsdialog.ui
    dbmanager.cpp
    dbmanager.h
    qcustomplot.cpp
    qcustomplot.h
)

target_link_libraries(AirportInspector
    Qt5::Core
    Qt5::Widgets
    Qt5::Sql
    Qt5::PrintSupport
)
