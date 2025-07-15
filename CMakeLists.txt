cmake_minimum_required(VERSION 3.16)

project(crab-tool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    execute_process(
        COMMAND powershell -Command "& {
            Get-ChildItem 'C:\\Qt' -Directory |
            Where-Object { $_.Name -match '^6\\.' } |
            Sort-Object Name -Descending |
            Select-Object -First 1 |
            ForEach-Object { $_.Name }
        }"
        OUTPUT_VARIABLE QT_VERSION_FOUND
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(QT_BASE_PATH "C:/Qt/${QT_VERSION_FOUND}/mingw_64")
    set(QT_CMAKE_PATH "${QT_BASE_PATH}/lib/cmake")

    message(STATUS "Detected Qt version: ${QT_VERSION_FOUND}")
    message(STATUS "Using Qt CMake path: ${QT_CMAKE_PATH}")

    set(CMAKE_PREFIX_PATH "${QT_CMAKE_PATH}")
elseif(UNIX AND NOT APPLE)
    execute_process(
        COMMAND bash -c "ls $ENV{HOME}/Qt | grep '^6\\.' | sort -V | tail -n 1"
        OUTPUT_VARIABLE QT_VERSION_FOUND
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(QT_LIB_PATH "$ENV{HOME}/Qt/${QT_VERSION_FOUND}/gcc_64/lib")
    set(QT_PLUGINS_PATH "$ENV{HOME}/Qt/${QT_VERSION_FOUND}/gcc_64/plugins")
    set(QT_CMAKE_PATH "${QT_LIB_PATH}/cmake")
    set(CMAKE_PREFIX_PATH "${QT_CMAKE_PATH}")
endif()

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent)
message(STATUS "Using Qt version: ${Qt${QT_VERSION_MAJOR}Core_VERSION}")

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
    BUILD_WITH_INSTALL_RPATH TRUE
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "crab-tool-${PROJECT_VERSION}"
)

if(${QT_VERSION} VERSION_LESS 6.5.0)
    qt_finalize_executable(crab-tool)
endif()

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "crab-tool")
set(CPACK_PACKAGE_VENDOR "Ivan Perehiniak")
set(CPACK_PACKAGE_CONTACT "iv.perehinik@gmail.com")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CrabTool - Lightweight image tagging tool")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/icon/crab.ico")
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;TGZ")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ivan Perehiniak")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "CrabTool is a Qt-based image tagging tool.\n"
    "  It helps you quickly annotate image datasets.\n"
    "  \n"
    "  Features:\n"
    "  - Projects with tagging data\n"
    "  - Objects tagging\n"
    "  - Export to YOLO training dataset\n"
    "  - Export to Haar training descriptors\n"
    "  - Fast folder scanning\n"
    "  - Thumbnail previews\n"
    )
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/perehinik/crab-tool")
    set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_CHANGELOG "${CMAKE_SOURCE_DIR}/CHANGELOG.md")

    set(APP_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/crab-tool")

    # Create bash script which will start actual binary
    file(WRITE "${CMAKE_BINARY_DIR}/crab-tool"
    "#!/bin/bash\n"
    "export LD_LIBRARY_PATH=/usr/lib/crab-tool/Qt\n"
    "export QT_PLUGIN_PATH=/usr/lib/crab-tool/plugins\n"
    "/usr/lib/crab-tool/crab-tool-${PROJECT_VERSION}"
    )
    file(CHMOD "${CMAKE_BINARY_DIR}/crab-tool" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    # install created bash script
    install(PROGRAMS ${CMAKE_BINARY_DIR}/crab-tool DESTINATION ${CMAKE_INSTALL_BINDIR})
    # install actual binary
    install(TARGETS crab-tool RUNTIME DESTINATION ${APP_LIB_DIR})

    # Install libraries
    file(GLOB QT6_RUNTIME_LIBS "${QT_LIB_PATH}/lib*.so*")
    install(FILES ${QT6_RUNTIME_LIBS} DESTINATION "${APP_LIB_DIR}/Qt")

    # Install plugins
    install(DIRECTORY "${QT_PLUGINS_PATH}/platforms"        DESTINATION "${APP_LIB_DIR}/plugins")
    install(DIRECTORY "${QT_PLUGINS_PATH}/imageformats"     DESTINATION "${APP_LIB_DIR}/plugins")
    install(DIRECTORY "${QT_PLUGINS_PATH}/platformthemes"   DESTINATION "${APP_LIB_DIR}/plugins")

endif()

include(CPack)
