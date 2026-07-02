cmake_minimum_required(VERSION 3.20)

project(KeyEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Build/Linux")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Binaries/Linux")
set(CMAKE_DEBUG_POSTFIX "_d")

add_subdirectory(Dependencies)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(LLVM REQUIRED CONFIG)

find_library(LIBCLANG_LIBRARY
    NAMES clang libclang
    HINTS ${LLVM_LIBRARY_DIRS}
    REQUIRED
)

add_compile_definitions(
    PLATFORM_LINUX
    BUILD_OS_LINUX
    $<$<CONFIG:Debug>:_DEBUG>
)

add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-register>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-private-field>
)

file(GLOB_RECURSE KEY_RUNTIME_SOURCES CONFIGURE_DEPENDS
    "Source/Runtime/*.cpp"
)

add_library(KeyRuntime SHARED ${KEY_RUNTIME_SOURCES})
target_compile_definitions(KeyRuntime
    PRIVATE RUNTIME_EXPORTS
    PUBLIC RUNTIME_DLL
)
target_include_directories(KeyRuntime
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime"
)
target_link_libraries(KeyRuntime
    PUBLIC
        Threads::Threads
        KeyThirdParty::base64
        KeyThirdParty::glm
        KeyThirdParty::rapidjson
)
set_target_properties(KeyRuntime PROPERTIES OUTPUT_NAME "KeyRuntime")

file(GLOB_RECURSE KEY_EDITOR_SOURCES CONFIGURE_DEPENDS
    "Source/Editor/*.cpp"
)

add_library(KeyEditor SHARED ${KEY_EDITOR_SOURCES})
target_compile_definitions(KeyEditor
    PRIVATE EDITOR_EXPORTS
    PUBLIC EDITOR_DLL RUNTIME_DLL
)
target_include_directories(KeyEditor
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Editor"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime"
)
target_link_libraries(KeyEditor PUBLIC KeyRuntime KeyThirdParty::glm ${CMAKE_DL_LIBS})
set_target_properties(KeyEditor PROPERTIES OUTPUT_NAME "KeyEditor")

file(GLOB KEY_PROJECT_CREATOR_SOURCES CONFIGURE_DEPENDS
    "Source/ProjectCreator/*.cpp"
)

add_executable(ProjectCreator ${KEY_PROJECT_CREATOR_SOURCES})
target_include_directories(ProjectCreator
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/ProjectCreator"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime"
)
target_link_libraries(ProjectCreator PRIVATE KeyRuntime KeyThirdParty::mustache KeyThirdParty::tclap)

file(GLOB KEY_REFLECTION_PARSER_SOURCES CONFIGURE_DEPENDS
    "Source/ReflectionParser/*.cpp"
)

add_executable(ReflectionParser ${KEY_REFLECTION_PARSER_SOURCES})
target_include_directories(ReflectionParser
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/ReflectionParser"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime"
        "${LLVM_INCLUDE_DIRS}"
)
target_link_libraries(ReflectionParser PRIVATE KeyRuntime KeyThirdParty::mustache KeyThirdParty::tclap ${LIBCLANG_LIBRARY})

file(GLOB KEY_QTPROPERTYBROWSER_SOURCES CONFIGURE_DEPENDS
    "Dependencies/qtpropertybrowser/src/*.cpp"
)

add_library(QtPropertyBrowser STATIC
    ${KEY_QTPROPERTYBROWSER_SOURCES}
    "Dependencies/qtpropertybrowser/src/qtpropertybrowser.qrc"
)
target_include_directories(QtPropertyBrowser
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/qtpropertybrowser/src"
)
target_link_libraries(QtPropertyBrowser PUBLIC KeyThirdParty::glm Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(QtPropertyBrowser PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)

add_executable(KeyEditorApp
    "Source/EditorApp/main.cpp"
    "Source/EditorApp/MainWindow.cpp"
    "Source/EditorApp/ContentBrowserWidget.cpp"
    "Source/EditorApp/InspectorWidget.cpp"
    "Source/EditorApp/NewProjectDialog.cpp"
    "Source/EditorApp/OutlinerWidget.cpp"
    "Source/EditorApp/MainWindow.h"
    "Source/EditorApp/ContentBrowserWidget.h"
    "Source/EditorApp/InspectorWidget.h"
    "Source/EditorApp/NewProjectDialog.h"
    "Source/EditorApp/OutlinerWidget.h"
    "Source/EditorApp/Forms/MainWindow.ui"
    "Source/EditorApp/Forms/ContentBrowserWidget.ui"
    "Source/EditorApp/Forms/InspectorWidget.ui"
    "Source/EditorApp/Forms/NewProjectDialog.ui"
    "Source/EditorApp/Resources/Icons/icons.qrc"
    "Source/EditorApp/Resources/Styles/Dark/dark.qrc"
)
target_compile_definitions(KeyEditorApp PRIVATE RUNTIME_DLL EDITOR_DLL)
target_include_directories(KeyEditorApp
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/EditorApp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Editor"
        "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/qtpropertybrowser/src"
        "${CMAKE_CURRENT_BINARY_DIR}/KeyEditorApp_autogen/include"
)
target_link_libraries(KeyEditorApp
    PRIVATE
        KeyRuntime
        KeyEditor
        QtPropertyBrowser
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::OpenGL
        GL
)
set_target_properties(KeyEditorApp PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Source/EditorApp/Forms"
)
set_target_properties(KeyEditorApp PROPERTIES OUTPUT_NAME "KeyEditor")

add_executable(KeyEditorApp2
    "Source/EditorApp2/main.cpp"
    "Source/EditorApp2/MainWindow.cpp"
    "Source/EditorApp2/ContentBrowserWidget.cpp"
    "Source/EditorApp2/InspectorWidget.cpp"
    "Source/EditorApp2/IconManager.cpp"
    "Source/EditorApp2/OutlinerWidget.cpp"
    "Source/EditorApp2/Utils.cpp"
    "Source/EditorApp2/MainWindow.h"
    "Source/EditorApp2/ContentBrowserWidget.h"
    "Source/EditorApp2/InspectorWidget.h"
    "Source/EditorApp2/IconManager.h"
    "Source/EditorApp2/OutlinerWidget.h"
    "Source/EditorApp2/Utils.h"
)
target_compile_definitions(KeyEditorApp2 PRIVATE RUNTIME_DLL EDITOR_DLL)
target_include_directories(KeyEditorApp2
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/EditorApp2"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Runtime"
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/Editor"
)
target_link_libraries(KeyEditorApp2
    PRIVATE
        KeyRuntime
        KeyEditor
        KeyThirdParty::imgui
)
set_target_properties(KeyEditorApp2 PROPERTIES OUTPUT_NAME "KeyEditor2")
