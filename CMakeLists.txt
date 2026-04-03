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
    test/buffer_test.cpp
    test/http_codec_test.cpp
    test/ws_codec_test.cpp
    test/router_test.cpp
)

target_link_libraries(cpphttp_test PRIVATE cpphttp)
target_compile_options(cpphttp_test PRIVATE -Wall -Wextra)

enable_testing()
add_test(NAME cpphttp_test COMMAND cpphttp_test)
