cmake_minimum_required(VERSION 3.16)

project(TelegramCloneQML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Поиск Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Quick Qml Network)

# Настройка кросскомпиляции для Windows
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Cross-compiling for Windows")
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

# Список исходных файлов
set(SOURCES
        source/main.cpp
        source/ChatList.cpp
        source/DialogClass.cpp
        source/MessageClass.cpp
        source/NetworkManager.cpp
        source/UserController.cpp
)

set(HEADERS
        source/ChatList.h
        source/DialogClass.h
        source/MessageClass.h
        source/NetworkManager.h
        source/UserController.h
)

# Создание исполняемого файла
add_executable(TelegramCloneQML ${SOURCES} ${HEADERS})

# Ресурсы
qt_add_resources(TelegramCloneQML "resources"
        PREFIX "/"
        FILES
        source/sources.qrc
)

# Для Windows настройки
if(WIN32)
    set_target_properties(TelegramCloneQML PROPERTIES
            WIN32_EXECUTABLE TRUE
            SUFFIX ".exe"
    )
endif()

# Подключение Qt библиотек
target_link_libraries(TelegramCloneQML PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Network
        Qt6::Gui
)

# Включение директорий
target_include_directories(TelegramCloneQML PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Установка QML файлов для сборки
file(GLOB_RECURSE QML_FILES
        "source/*.qml"
        "Pages/*.qml"
)

# Добавление QML модуля
qt_add_qml_module(TelegramCloneQML
        URI TelegramCloneQML
        VERSION 1.0
        QML_FILES ${QML_FILES}
)

# Установка
install(TARGETS TelegramCloneQML
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .
        LIBRARY DESTINATION lib
)