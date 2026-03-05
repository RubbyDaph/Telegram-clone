cmake_minimum_required(VERSION 3.16)
project(TelegramCloneQML VERSION 0.6 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt automation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

# Cross-compilation detection
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compiling detected - targeting Windows")
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Network Gui)

# Standard Qt project setup
qt_standard_project_setup(REQUIRES 6.8)

# Collect source files
file(GLOB_RECURSE SOURCES "source/*.cpp")
file(GLOB_RECURSE HEADERS "source/*.h")

# Define main executable
qt_add_executable(appTelegramCloneQML
        main.cpp
        ${SOURCES}
        ${HEADERS}
)

# Include directories
target_include_directories(appTelegramCloneQML PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Link Qt libraries
target_link_libraries(appTelegramCloneQML PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Network
        Qt6::Qml
        Qt6::Quick
)

# Add QML module
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

# Add resources if they exist
qt_add_resources(appTelegramCloneQML "resources"
        PREFIX "/"
        FILES
        sources.qrc
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS appTelegramCloneQML
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)