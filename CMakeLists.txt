cmake_minimum_required(VERSION 3.15)
project(cpphttp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS system thread)

add_library(cpphttp STATIC
    src/cpphttp.cpp
    src/api/http_server.cpp
    src/api/http_client.cpp
    src/api/ws_server.cpp
    src/api/ws_client.cpp
    src/api/http_types.cpp
    src/core/router.cpp
    src/core/session.cpp
    src/core/connection.cpp
    src/core/buffer.cpp
    src/core/codec/http_codec.cpp
    src/core/codec/ws_codec.cpp
    src/platform/asio_wrapper.cpp
)

target_include_directories(cpphttp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cpphttp PUBLIC
    Boost::system
    Boost::thread
)

target_compile_options(cpphttp PRIVATE -Wall -Wextra)

add_executable(cpphttp_test
    test/main.cpp
)

target_link_libraries(cpphttp_test PRIVATE cpphttp)
target_compile_options(cpphttp_test PRIVATE -Wall -Wextra)

add_executable(http_server_demo demo/http_server_demo.cpp)
target_link_libraries(http_server_demo PRIVATE cpphttp)
target_compile_options(http_server_demo PRIVATE -Wall -Wextra)

add_executable(http_client_demo demo/http_client_demo.cpp)
target_link_libraries(http_client_demo PRIVATE cpphttp)
target_compile_options(http_client_demo PRIVATE -Wall -Wextra)

add_executable(ws_server_demo demo/ws_server_demo.cpp)
target_link_libraries(ws_server_demo PRIVATE cpphttp)
target_compile_options(ws_server_demo PRIVATE -Wall -Wextra)

add_executable(ws_client_demo demo/ws_client_demo.cpp)
target_link_libraries(ws_client_demo PRIVATE cpphttp)
target_compile_options(ws_client_demo PRIVATE -Wall -Wextra)

enable_testing()
add_test(NAME cpphttp_test COMMAND cpphttp_test)
