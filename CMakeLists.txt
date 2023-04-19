cmake_minimum_required(VERSION 3.24)
project(daemonpp)

set(CMAKE_CXX_STANDARD 20)

# Directory with configuration files
set(DAEMON_CONF_DIR "/etc/daemon")

# Directory with systemd unit files
set(SYSTEMD_UNIT_DIR "/usr/lib/systemd/system/")

# Default directory for log file
set(DAEMON_LOG_DIR "/var/log/daemon")

# Default directory for PID file
set(DAEMON_PID_DIR "/run/daemon")

add_executable(daemonpp main.cpp daemon.hpp dlog.hpp)

# Default directory for PID file
set (DAEMON_PID_DIR "/run/daemon")

# Macro for installing configuration files
function(install_conf src dest)
    if(NOT IS_ABSOLUTE "${src}")
        set(src "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    endif()
    get_filename_component(src_name "${src}" NAME)
    if (NOT IS_ABSOLUTE "${dest}")
        set(dest "${CMAKE_INSTALL_PREFIX}/${dest}")
    endif()
    install(CODE "
    if(NOT EXISTS \"\$ENV{DESTDIR}${dest}/${src_name}\")
      #file(INSTALL \"${src}\" DESTINATION \"${dest}\")
      message(STATUS \"Installing: \$ENV{DESTDIR}${dest}/${src_name}\")
      execute_process(COMMAND \${CMAKE_COMMAND} -E copy \"${src}\"
                      \"\$ENV{DESTDIR}${dest}/${src_name}\"
                      RESULT_VARIABLE copy_result
                      ERROR_VARIABLE error_output)
      if(copy_result)
        message(FATAL_ERROR \${error_output})
      endif()
    else()
      message(STATUS \"Skipping  : \$ENV{DESTDIR}${dest}/${src_name}\")
    endif()
  ")
endfunction(install_conf)

# Install configuration file
install_conf (./daemon.conf ${DAEMON_CONF_DIR})

# Install systemd unit files
install_conf (./simple-daemon.service ${SYSTEMD_UNIT_DIR})
install_conf (./forking-daemon.service ${SYSTEMD_UNIT_DIR})

# Create empty directory for default log file
install(DIRECTORY DESTINATION ${DAEMON_LOG_DIR})

# Create empty directory for default PID file
install(DIRECTORY DESTINATION ${DAEMON_PID_DIR})