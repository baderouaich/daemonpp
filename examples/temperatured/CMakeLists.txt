cmake_minimum_required(VERSION 3.10)
project(temperatured VERSION "0.0.1" DESCRIPTION "Daemon that monitors cpu temperature each second" LANGUAGES CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -pedantic -O0")
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
    set(CMAKE_CXX_FLAGS "-DNDEBUG -Wall -Wextra -pedantic -O3")
endif()

include_directories(../../include)
add_executable(${PROJECT_NAME} temperatured.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_link_libraries(${PROJECT_NAME} PRIVATE
        X11
        Xfixes
)

# Install the config file .conf
install(FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.conf DESTINATION /etc/${PROJECT_NAME}/)

# Install the systemd file .service
install(FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.service DESTINATION /etc/systemd/system/)

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
