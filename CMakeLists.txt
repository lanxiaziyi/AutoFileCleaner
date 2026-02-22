cmake_minimum_required(VERSION 3.16)

project(AutoFileCleaner LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS
    Core
    Gui
    Widgets
    Network
    Sql
    REQUIRED
)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/taskmanager.h
    src/taskmanager.cpp
    src/taskmodel.h
    src/taskmodel.cpp
    src/configmanager.h
    src/configmanager.cpp
    src/filecleaner.h
    src/filecleaner.cpp
    src/schedulermanager.h
    src/schedulermanager.cpp
    src/addtaskdialog.h
    src/addtaskdialog.cpp
    src/autostartutiility_win.h
    src/autostartutiility_win.cpp
    resources/resources.qrc
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Sql
)

# Windows specific
if(WIN32)
    target_link_libraries(${PROJECT_NAME} advapi32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
