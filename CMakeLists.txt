cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(PROJECT_NAME "app")
project(${PROJECT_NAME} LANGUAGES CXX)

option(USE_SYSTEM_NVTX "Link against system NVTX (–DUSE_SYSTEM_NVTX=[ON|OFF])" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Network Sql Svg PrintSupport Concurrent Qml Quick
)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED CMAKE_PREFIX_PATH)
  set(CMAKE_PREFIX_PATH "/opt/libtorch;/opt/Qt6" CACHE PATH "Where to find Qt6 and LibTorch" FORCE)
endif()

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Torch REQUIRED)
find_package(OpenCV 4.0 REQUIRED)

find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MLPACK REQUIRED mlpack)
include_directories(${MLPACK_INCLUDE_DIRS})
link_directories   (${MLPACK_LIBRARY_DIRS})

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

set(SRC_DIR   "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCES
    ${SRC_DIR}/main.cpp
        src/download.h
        src/indicators.h
        src/color.h
        src/load.h
        src/train.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${UI_FILES}
    ${QRC_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${MLPACK_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Sql
    Qt6::Svg Qt6::PrintSupport Qt6::Concurrent Qt6::Qml Qt6::Quick

    ${TORCH_LIBRARIES}
    ${OpenCV_LIBS}
    ${MLPACK_LIBRARIES}
    Eigen3::Eigen
    Python3::Python
    CURL::libcurl
    ZLIB::ZLIB
)

target_link_options(${PROJECT_NAME} PRIVATE
    -Wl,--no-as-needed
)

if(USE_SYSTEM_NVTX)
  message(STATUS "Linking against system NVTX")
else()
  message(STATUS "Using LibTorch‑bundled NVTX")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
