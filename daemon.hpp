//
// Created by bader on 4/18/23.
//
#pragma once
#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <csignal>
#include <sys/stat.h>
#include "dlog.hpp"
#include <stdexcept>
#include <chrono>
#include <thread>
#include <atomic>

namespace daemonpp {
    class daemon {
    private:
        static daemon* instance;

    public:
       /* Construct a new daemon process.
       * @note: only one daemon per application is possible.
       * @param name: name of daemon process
       * @param cwd: daemon current working directory, root "/" directory by default.
       * @param update_duration: duration to sleep before waking up the on_update() callback everytime, default 10 seconds
       */
        daemon(const std::string &name, const std::string& cwd = "/", const std::chrono::high_resolution_clock::duration& update_duration = std::chrono::seconds(10)) :
        m_name(name), m_cwd(cwd), m_update_duration(update_duration), m_is_running(false)
        {
          if(instance) {
            dlog::error("Only one daemon instance is possible.");
            std::exit(EXIT_FAILURE);
          }
          instance = this;
        }

        daemon() : m_name("<unknown_daemon>"), m_cwd("/"), m_update_duration(std::chrono::seconds(10)), m_is_running(false)
        {
          if(instance) {
            dlog::error("Only one daemon instance is possible.");
            std::exit(EXIT_FAILURE);
          }
          instance = this;
        }

        void run()
        {
          if(m_is_running.load()) {
            dlog::error("Daemon '"+m_name+"' is already running.");
            return;
          }

          daemonize();
          on_start();
          m_is_running = true;
          while(m_is_running.load())
          {
            on_update();
            std::this_thread::sleep_for(m_update_duration);
          }
          on_exit();
        }

        void stop() // maybe pass in exit code ?
        {
          m_is_running.store(false);
        }


        ~daemon() {
          dlog::shutdown();
          // Terminate the child process when the daemon completes (loop stopped)
          // note that calling std::exit() inside the run function will not call dtor.
          std::exit(EXIT_SUCCESS);
        }

    protected:
        virtual void on_start() = 0;
        virtual void on_update() = 0;
        virtual void on_exit() = 0;
        virtual void on_restart() = 0;

    private:
        static void signal_handler(std::int32_t sig) {
          dlog::info("Signal " + std::to_string(sig) + " received.");
          switch (sig) {
            case SIGTERM:
            case SIGKILL: {
              if(!instance) return;
              instance->stop();
              break;
            }
            case SIGHUP: {
              instance->on_restart();
              break;
            }
            default:
              break;
          }
        }
    public: // getters & setters
        void set_update_duration(const std::chrono::high_resolution_clock::duration& duration) noexcept {
          m_update_duration = duration;
        }
        const std::chrono::high_resolution_clock::duration& get_update_duration() const noexcept {
          return m_update_duration;
        }

        void set_name(const std::string& daemon_name) noexcept {
          m_name = daemon_name;
        }
        const std::string& get_name() const noexcept { return m_name; }

        void set_cwd(const std::string& current_working_dir) noexcept {
          // Change the current working directory to a directory guaranteed to exist, provided by the user.
          if(chdir(current_working_dir.c_str()) < 0)
          {
            dlog::error("Could not change current working directory to '" +current_working_dir+ "': " + std::string(std::strerror(errno)));
            return;
          }
          m_cwd = current_working_dir;
        }
        const std::string& get_cwd() const noexcept { return m_cwd; }

    private:
        /**
         * Daemonize this program
         * @note: It is also possible to use glibc function deamon()
		     * at this point, but it is useful to customize your daemon.
         *
         */
        void daemonize() {
          // Fork off the parent process https://linux.die.net/man/3/fork
          pid_t pid = fork();
          // Success: The parent process continues with a process ID greater than 0
          if (pid > 0) {
            std::exit(EXIT_SUCCESS);
          }
          // An error occurred. A process ID lower than 0 indicates a failure in either process
          else if (pid < 0) {
            std::exit(EXIT_FAILURE);
          }
          // The parent process has now terminated, and the forked child process will continue
          // (the pid of the child process was 0)

          // Since the child process is a daemon, the umask needs to be set so files and logs can be written
          umask(0);

          // Initialize syslog for this daemon, here it's a good place to do so.
          dlog::init(m_name);
          dlog::notice("Daemon '" + m_name + "' started successfully.");

          // On success: The child process becomes session leader. Generate a session ID for the child process
          pid_t sid = setsid();
          if (sid < 0) {
            dlog::error("Could not set SID to child process: " + std::string(std::strerror(errno)));
            std::exit(EXIT_FAILURE);
          }

          // Ignore the Child terminated or stopped signal.
          std::signal(SIGCHLD, SIG_IGN);
          // Set signal handlers to detect daemon interrupt, restart..
          std::signal(SIGHUP, signal_handler);
          // when a sudo systemctl stop daemonpp.service is ran, by default,
          // a SIGTERM is sent, followed by 90 seconds of waiting followed by a SIGKILL.
          std::signal(SIGTERM, signal_handler); // if ran: sudo systemctl stop daemonpp.service
          std::signal(SIGKILL, signal_handler); // if ran: sudo systemctl stop daemonpp.service (after 90 seconds)

          // Change the current working directory to a directory guaranteed to exist, provided by the user
          if(chdir(m_cwd.c_str()) < 0)
          {
            dlog::error("Could not change current working directory to '" +m_cwd+ "': " + std::string(std::strerror(errno)));
            // If our guaranteed directory does not exist, terminate the child process to ensure
            // the daemon has not been hijacked
            std::exit(EXIT_FAILURE);
          }

          // A daemon cannot use the terminal, so close standard file descriptors for security reasons
          close(STDIN_FILENO);
          close(STDOUT_FILENO);
          close(STDERR_FILENO);
        }

    private:
        std::string m_name;
        std::string m_cwd;
        std::chrono::high_resolution_clock::duration m_update_duration;
        std::atomic<bool> m_is_running;
    };
   daemon* daemon::instance = nullptr;
}