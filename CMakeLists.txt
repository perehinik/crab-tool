cmake_minimum_required(VERSION 3.16)

project(crab-tool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent)

# GLOB sources in subdirectories
file(GLOB_RECURSE CORE_SRC "core/*.cpp" "core/*.h")
file(GLOB_RECURSE WIDGETS_SRC "widgets/*.cpp" "widgets/*.h")

# Manually list root sources
set(ROOT_SRC
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

qt_add_executable(crab-tool
    ${ROOT_SRC}
    ${CORE_SRC}
    ${WIDGETS_SRC}
    resources.qrc
)

target_include_directories(crab-tool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

target_link_libraries(crab-tool
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
)

# Platform-specific bundle stuff
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.crab-tool)
endif()

set_target_properties(crab-tool PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME CrabTool
)

include(GNUInstallDirs)
install(TARGETS crab-tool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(${QT_VERSION} VERSION_LESS 6.5.0)
    qt_finalize_executable(crab-tool)
endif()
