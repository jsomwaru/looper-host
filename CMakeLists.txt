include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.7.3)

FetchContent_Declare(cryptlite
  GIT_REPOSITORY https://github.com/lyokato/cpp-cryptlite.git)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(cryptlite)
if(NOT cryptlite_POPULATED)
    #FetchContent_MakeAvailable(cryptlite)
    FetchContent_Populate(cryptlite)
    add_subdirectory(${cryptlite_SOURCE_DIR} ${cryptlite_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

find_package(Boost 1.67 COMPONENTS REQUIRED)

cmake_minimum_required(VERSION 3.0)

project(looper_host)

set(CMAKE_SOURCE_DIR)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-Wall  -std=c++14 -pthread")

set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(SOURCE_FILES src/main.cpp src/protocol.cpp src/socket.cpp src/client_handler.cpp src/looper.cpp)

include_directories(looper_host PUBLIC src/include)

include_directories(${cryptlite_SOURCE_DIR}/include)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

add_executable(looper_host ${SOURCE_FILES})

target_link_libraries(looper_host PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(looper_host PRIVATE jack)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
