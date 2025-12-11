cmake_minimum_required(VERSION 3.16)

project(Telegram-clone-QML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# ДОБАВЛЯЕМ Core компонент - это обязательно!
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
qt_add_resources(sources example.qrc)

qt_add_executable(appTelegram-clone-QML
        main.cpp
        ${SOURCES}
        ${HEADERS}
)

target_include_directories(appTelegram-clone-QML PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)


qt_add_qml_module(appTelegram-clone-QML
    URI "TelegramCloneQML"
    VERSION 1.0
    QML_FILES
        Main.qml
        ChatsDelegate.qml
        DialogDelegate.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appTelegram-clone-QML PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appTelegram-clone-QML
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ДОБАВЛЯЕМ Core в линковку
target_link_libraries(appTelegram-clone-QML
    PRIVATE Qt6::Core Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appTelegram-clone-QML
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
include_directories(${Qt6Core_INCLUDE_DIRS})
include_directories(${Qt6Quick_INCLUDE_DIRS})
