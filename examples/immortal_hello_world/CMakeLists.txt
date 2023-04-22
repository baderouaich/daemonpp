cmake_minimum_required(VERSION 3.10)
project(immortal_hello_world VERSION "0.0.1" DESCRIPTION "Daemon that logs Hello, World every minute, forever!" LANGUAGES CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -pedantic -O0")
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
    set(CMAKE_CXX_FLAGS "-DNDEBUG -Wall -Wextra -pedantic -O3")
endif()

include_directories(../../include) # to include daemon.hpp dlog.hpp and dconfig.hpp
add_executable(${PROJECT_NAME} immortal_hello_world.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

# Install the config file .conf
install(FILES ${CMAKE_SOURCE_DIR}/immortal_hello_world.conf DESTINATION /etc/immortal_hello_world/)

# Install the systemd file .service
install(FILES ${CMAKE_SOURCE_DIR}/immortal_hello_world.service DESTINATION /etc/systemd/system/)

# Install the binary program
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin/)


# make uninstall
add_custom_target("uninstall" COMMENT "Uninstall daemon files")
add_custom_command(
        TARGET "uninstall"
        POST_BUILD
        COMMENT "Uninstall files with install_manifest.txt"
        COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in install_manifest.txt to be uninstalled!
)