cmake_minimum_required(VERSION 3.16)

project(TelegramCloneQML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Core Quick)

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
        Pages/AddUser.qml
    RESOURCES
        sources.qrc
)

set_target_properties(appTelegramCloneQML PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appTelegramCloneQML
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
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
