cmake_minimum_required(VERSION 3.10)
project(daemonpp VERSION "0.0.1" DESCRIPTION "Simple C++ template for creating Linux daemons" LANGUAGES CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -pedantic -O0")
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
    set(CMAKE_CXX_FLAGS "-DNDEBUG -Wall -Wextra -pedantic -O3")
endif()

add_executable(${PROJECT_NAME} main.cpp daemon.hpp dlog.hpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

# Install the config file
install(FILES ${CMAKE_SOURCE_DIR}/daemonpp.conf DESTINATION /etc/daemonpp/)

# Install the systemd file
install(FILES ${CMAKE_SOURCE_DIR}/daemonpp.service DESTINATION /etc/systemd/system/)

# Install the binary program
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin/)
