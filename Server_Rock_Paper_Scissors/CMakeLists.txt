cmake_minimum_required(VERSION 3.16)

project(Server_Rock_Paper_Scissors VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        serverwindow.cpp
        serverwindow.h
        serverwindow.ui
        clienthandler.h
        clienthandler.cpp
        servermanager.h
        servermanager.cpp
        gamedata.h
        protocol.h
        servergamecontroller.h
        servergamecontroller.cpp
        gamesession.h
        gamesession.cpp
        stylemanager.h
        stylemanager.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Server_Rock_Paper_Scissors
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Server_Rock_Paper_Scissors APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Server_Rock_Paper_Scissors SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Server_Rock_Paper_Scissors
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Server_Rock_Paper_Scissors PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Server_Rock_Paper_Scissors)
endif()
set_target_properties(Server_Rock_Paper_Scissors PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Server_Rock_Paper_Scissors
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Copy Qt DLLs to build directory automatically
add_custom_command(TARGET Server_Rock_Paper_Scissors POST_BUILD
    COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
        $<$<CONFIG:Debug>:--debug>
        $<$<CONFIG:Release>:--release>
        --no-translations
        --no-opengl-sw
        --compiler-runtime
        "$<TARGET_FILE:Server_Rock_Paper_Scissors>"
    COMMENT "Running windeployqt..."
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Server_Rock_Paper_Scissors)
endif()
