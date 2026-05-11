cmake_minimum_required(VERSION 3.16)
project(BagInventory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

set(SOURCES
    main.cpp
    mainwindow.cpp
)

set(HEADERS
    mainwindow.h
    bagitem.h
)

set(UI_FILES
    mainwindow.ui
)

qt_add_executable(BagInventory
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

target_link_libraries(BagInventory PRIVATE Qt6::Widgets Qt6::Core)
