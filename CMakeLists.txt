cmake_minimum_required(VERSION 3.16)

project(TelegramCloneQML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Находим Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Quick Qml Network Gui)

# Исходные файлы
file(GLOB_RECURSE SOURCES
        "source/*.cpp"
)

file(GLOB_RECURSE HEADERS
        "source/*.h"
)

# QML файлы
set(QML_FILES
        "source/Main.qml"
        "source/ChatsDelegate.qml"
        "source/DialogDelegate.qml"
        "Pages/AddUser.qml"
)

# Главное приложение
add_executable(TelegramCloneQML
        ${SOURCES}
        ${HEADERS}
        "source/main.cpp"
        "source/sources.qrc"
)

# Настройки для Windows
if(WIN32)
    set_target_properties(TelegramCloneQML PROPERTIES
            WIN32_EXECUTABLE TRUE
            SUFFIX ".exe"
    )

    target_compile_definitions(TelegramCloneQML PRIVATE
            -DUNICODE
            -D_UNICODE
    )
endif()

# Подключаем Qt модули
target_link_libraries(TelegramCloneQML PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Network
        Qt6::Gui
)

# Включаем директории
target_include_directories(TelegramCloneQML PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Опционально: Добавляем метаинформацию для QML модуля
qt_add_qml_module(TelegramCloneQML
        URI TelegramCloneQML
        VERSION 1.0
        QML_FILES ${QML_FILES}
)

# Устанавливаем приложение
install(TARGETS TelegramCloneQML
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)