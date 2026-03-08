cmake_minimum_required(VERSION 3.16)
project(TelegramCloneQML VERSION 0.6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.10.2/mingw_64" CACHE STRING "Qt installation path") # unlucky i'm a noob and i
                                                                       # don't want to do thing for different dirs

    find_package(Qt6 REQUIRED COMPONENTS Core Quick Network Gui Qml)

else()
    find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Network Gui)
endif()


qt_standard_project_setup(REQUIRES 6.8)

file(GLOB_RECURSE SOURCES "source/*.cpp")
file(GLOB_RECURSE HEADERS "source/*.h")

qt_add_executable(appTelegramCloneQML
        main.cpp
        ${SOURCES}
        ${HEADERS}
)

target_include_directories(appTelegramCloneQML PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(appTelegramCloneQML PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Network
        Qt6::Qml
        Qt6::Quick
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

qt_add_resources(appTelegramCloneQML "resources"
        PREFIX "/"
        FILES
        sources.qrc
)


if(WIN32)
    target_compile_definitions(appTelegramCloneQML PRIVATE
            _WIN32_WINNT=0x0A00
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )

    set_target_properties(appTelegramCloneQML PROPERTIES
            WIN32_EXECUTABLE FALSE
    )
endif()

# Copy dll files for windows, AND FOR LINUX i don't need to do it, because linux is better
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${CMAKE_PREFIX_PATH}/bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET appTelegramCloneQML POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_CURRENT_SOURCE_DIR} $<TARGET_FILE:appTelegramCloneQML>
                COMMENT "Deploying Qt libraries..."
        )
    endif()
endif()


include(GNUInstallDirs)
install(TARGETS appTelegramCloneQML
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)