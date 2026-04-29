cmake_minimum_required(VERSION 3.5)

project(08_PreDataBase VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для Qt 5.14 используем Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets Sql Concurrent)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        database.h
        database.cpp
        dbdata.h
        dbdata.cpp
        dbdata.ui
)

add_executable(08_PreDataBase
    ${PROJECT_SOURCES}
)

# Для Windows: копируем DLL в папку с исполняемым файлом
if(WIN32)
    add_custom_command(TARGET 08_PreDataBase POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/PG_Libs/libpq.dll
            ${CMAKE_CURRENT_SOURCE_DIR}/PG_Libs/libcrypto-3-x64.dll
            ${CMAKE_CURRENT_SOURCE_DIR}/PG_Libs/libssl-3-x64.dll
            ${CMAKE_CURRENT_SOURCE_DIR}/PG_Libs/libiconv-2.dll
            ${CMAKE_CURRENT_SOURCE_DIR}/PG_Libs/libintl-9.dll
            $<TARGET_FILE_DIR:08_PreDataBase>)
endif()

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/PG_Libs)
target_link_libraries(08_PreDataBase PRIVATE Qt5::Widgets Qt5::Sql Qt5::Concurrent
                                             libcrypto-3-x64
                                             libiconv-2
                                             libintl-9
                                             libpq
                                             libssl-3-x64)
