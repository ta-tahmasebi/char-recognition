cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# ------------------------------
# Project
# ------------------------------
project(DigitRecognizer LANGUAGES CXX)

# ------------------------------
# C++ Standard
# ------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------
# Qt Auto Tools
# ------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ------------------------------
# Default CMAKE_PREFIX_PATH for Qt6 / LibTorch
# ------------------------------
if (NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "/opt/libtorch;/opt/Qt6" CACHE PATH "Where to find Qt6 and LibTorch")
endif ()

# ------------------------------
# Find packages
# ------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql Svg PrintSupport Concurrent Qml Quick)
find_package(Torch REQUIRED)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)

# ------------------------------
# Source and include directories
# ------------------------------
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(SOURCES
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/UI.cpp
        ${SRC_DIR}/download.cpp
        ${SRC_DIR}/train.cpp
        ${SRC_DIR}/load.cpp
        ${SRC_DIR}/color.cpp

        ${SRC_DIR}/indicators.h
        ${SRC_DIR}/UI.h
        ${SRC_DIR}/download.h
        ${SRC_DIR}/train.h
        ${SRC_DIR}/load.h
        ${SRC_DIR}/color.h
)

# ------------------------------
# Executable
# ------------------------------
add_executable(${PROJECT_NAME}
        ${SOURCES}
)

# ------------------------------
# Include directories
# ------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
        ${INCLUDE_DIR}
)

# ------------------------------
# Link libraries
# ------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Sql
        Qt6::Svg Qt6::PrintSupport Qt6::Concurrent Qt6::Qml Qt6::Quick
        ${TORCH_LIBRARIES}
        CURL::libcurl
        ZLIB::ZLIB
)

# ------------------------------
# Platform-specific linker options
# ------------------------------
if (UNIX)
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,--no-as-needed)
endif ()


# ------------------------------
# Target properties
# ------------------------------
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
