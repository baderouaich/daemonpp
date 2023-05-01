cmake_minimum_required(VERSION 3.10)
project(httpreqd VERSION "0.0.1" DESCRIPTION "This Daemon performs HTTP JSON request every 1 minute" LANGUAGES CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -pedantic -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "-DNDEBUG -Wall -Wextra -pedantic -O3")
endif()

########## CPR SUBMODULE BEGIN ############
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/cpr)
include_directories(${CPR_INCLUDE_DIRS})
##########################################

########## nlohmann json SUBMODULE BEGIN ############
include_directories(${CMAKE_SOURCE_DIR}/lib/json/include)
####################################################

include_directories(../../include)
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20) # update your C++ version here if you like
target_link_libraries(${PROJECT_NAME} PRIVATE
        pthread
        ${CPR_LIBRARIES}
)
# Configure .service file
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.service)
configure_file(${CMAKE_SOURCE_DIR}/systemd/daemonpp.service.in ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.service)
endif()

# Configure .conf file
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.conf)
configure_file(${CMAKE_SOURCE_DIR}/systemd/daemonpp.conf.in ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.conf)
endif()

# Rename default daemonpp.cpp to ${PROJECT_NAME}.cpp
if(EXISTS ${CMAKE_SOURCE_DIR}/daemonpp.cpp)
    file(RENAME ${CMAKE_SOURCE_DIR}/daemonpp.cpp ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.cpp)
endif()

# Install the config file .conf
install(FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.conf DESTINATION /etc/${PROJECT_NAME}/)

# Install the systemd file .service
install(FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.service DESTINATION /etc/systemd/system/)

# Install the binary program
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin/)

# make uninstall
add_custom_target("uninstall_daemon" COMMENT "Uninstall daemon files")
add_custom_command(
        TARGET "uninstall_daemon"
        POST_BUILD
        COMMENT "Uninstall files with install_manifest.txt"
        COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in install_manifest.txt to be uninstalled!
)
