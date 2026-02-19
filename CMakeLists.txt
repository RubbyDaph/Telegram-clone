cmake_minimum_required(VERSION 3.16)

project(TelegramCloneQML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compiling detected")
    set(WIN32 TRUE)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Quick)




add_executable(TelegramCloneQML
        main.cpp
        ${SOURCES}
        ${HEADERS}
        ${app_resources}
)


qt_standard_project_setup(REQUIRES 6.8)

set(QT_QML_GENERATE_QMLLS_INI ON)

file(GLOB_RECURSE SOURCES
        "source/*.cpp"
)
file(GLOB_RECURSE HEADERS
        "source/*.h"
)

set(sources main.cpp)
qt_add_resources(sources sources.qrc)


target_link_libraries(TelegramCloneQML PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Network
        Qt6::Gui
)


qt_add_executable(appTelegramCloneQML
        main.cpp
        ${SOURCES}
        ${HEADERS}
        sources.qrc
)


target_include_directories(appTelegramCloneQML PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)


qt_add_qml_module(appTelegramCloneQML
        URI "TelegramCloneQML"
        VERSION 1.0
        QML_FILES
        Main.qml
        ChatsDelegate.qml
        DialogDelegate.qml
        Pages/login.qml
        RESOURCES
        sources.qrc
)


target_link_libraries(appTelegramCloneQML
        PRIVATE Qt6::Core Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appTelegramCloneQML
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
include_directories(${Qt6Core_INCLUDE_DIRS})
include_directories(${Qt6Quick_INCLUDE_DIRS})