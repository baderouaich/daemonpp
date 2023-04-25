## daemonpp
Simple C++ header only template for creating Linux daemons

### What are daemon services/processes ?
A dæmon is a program that runs silently in the background.<br>
Commonly, dæmon processes are created to offer a specific service.

**Dæmon** processes usually:
- Live for a long time;
- Started at boot time;
- Terminate only during shutdown;
- Have no controlling terminal.

[//]: # (### Dæmon Characteristics)

[//]: # (The previously listed characteristics have certain implications:)

[//]: # ()
[//]: # (- do one thing, and one thing only)

[//]: # (- resource leaks eventually surface)

[//]: # (- consider current working directory)

[//]: # (- no &#40;or only limited&#41; user-interaction possible)

[//]: # (- how to create &#40;debugging&#41; output)

### Usage
daemonpp has a simple, straightforward api with callbacks to handle your daemon events.
```cpp
#include "daemon.hpp"
using namespace daemonpp;
using namespace std::chrono_literals;

class my_daemon : public daemon
{
public:
    void on_start(const dconfig& cfg) override {
      /// Called once after daemon starts automatically with system startup or when you manually call `$ systemctl start my_daemon`
      /// Initialize your code here...

      dlog::info("my_daemon::on_start(): my_daemon version: " + cfg.get("version") + " started successfully!");
    }

    void on_update() override {
      /// Called every DURATION set in set_update_duration()...
      /// Update your code here...

      dlog::info("my_daemon::on_update()");
      if(++counter >= 3) { 
          dlog::trace("Stopping my_daemon after 3 updates.");
          stop(EXIT_SUCCESS);
      }
    }

    void on_stop() override {
      /// Called once before daemon is about to exit with system shutdown or when you manually call `$ systemctl stop my_daemon`
      /// Cleanup your code here...

      dlog::info("my_daemon::on_stop()");
    }

    void on_reload(const dconfig& cfg) override {
      /// Called once after your daemon's config fil is updated then reloaded with `$ systemctl reload my_daemon`
      /// Handle your config updates here...

      dlog::info("my_daemon::on_reload(): new daemon version from updated config: " + cfg.get("version"));
    }
    
private:
    int counter = 0;
};

int main(int argc, const char* argv[]) {
  my_daemon dmn;                         // create a daemon instance
  dmn.set_name("my_daemon"); // set daemon name to identify logs in syslog
  dmn.set_update_duration(3s);   // set duration to sleep before triggering the on_update callback 3 seconds
  dmn.set_cwd("/");      // set daemon's current working directory to root /
  dmn.run(argc, argv);                   // run your daemon
  return EXIT_SUCCESS;
}
```

### Examples
See [examples](./examples)

## Tutorial - Create a new daemon project in 3 steps
Let's assume your daemon project is called **my_daemon**
1. First, clone this repository into a new folder named after your project:
```bash
git clone https://github.com/baderouaich/daemonpp my_daemon
```
Your project structure will be like this:
```text
my_daemon
├── examples/           # example daemon projects to inspire from
├── include/            # include files contains daemonpp single header files
├── systemd/            # .service.in and .conf.in files to be configured by cmake
├── daemonpp.cpp        # daemonpp.cpp sample daemon template
├── CMakeLists.txt      # CMake project file 
├── LICENSE             # MIT License file
└── README.md
```

2. Update your [CMakeLists.txt](CMakeLists.txt) file according to your project properties (name, version and description...)
```cmake
cmake_minimum_required(VERSION 3.10)
project(my_daemon VERSION "0.0.1" DESCRIPTION "My daemon description" LANGUAGES CXX)
          ^                                            ^
          │_____ update your project name              |
                                                       |
                                                       │_____ update your project description
```

3. Configure your project (to create .service and .conf required daemon files):
```bash
mkdir build && cd build
cmake ..
```
  
Now your project structure will look like this:
```text
my_daemon
├── examples/        
├── include/        
├── systemd/        
├── my_daemon.conf       
├── my_daemon.service    
├── my_daemon.cpp       
├── CMakeLists.txt   
├── LICENSE
└── README.md
```

That's it! you can now update your daemon code in the my_daemon.cpp file and remove examples/ folder if you don't need.

Also see how to:

- [Install your daemon](#install-your-daemon)

- [Enable/Disable your daemon to run with system startup](#enabledisable-your-daemon-to-run-with-system-startup)

- [Start/Stop your daemon](#startstop-your-daemon)

- [Restart your daemon](#'estart-your-daemon)

- [Reload your daemon](#install-your-daemon)

- [Uninstall your daemon](#uninstall-your-daemon)

## Install your daemon
```bash
sudo make install
```

## Enable/Disable your daemon to run with system startup
```bash
sudo systemctl [enable|disable] my_daemon
```

## Start/Stop your daemon
Start
```bash 
systemctl start my_daemon
```
> will trigger the on_start() callback, providing your config values.

Stop
```bash
systemctl stop my_daemon
``` 
> will trigger the on_stop() callback.

## Restart your daemon
```bash
systemctl restart my_daemon
```
> This is equivalent to `sudo systemctl stop your_daemon && sudo systemctl start your_daemon`

## Reload your daemon
if you change  your .service or .conf files, and you reinstalled your daemon, you have to reload 
your daemon by running:<br>
`systemctl reload my_daemon`
> will trigger the on_reload() callback, providing the new config values.

## Check your daemon's status
```bash
systemctl status my_daemon
```

## Uninstall your daemon
1. Stop daemon gracefully
```bash
systemctl stop my_daemon
systemctl disable my_daemon
```
2. Delete daemon files
```bash
sudo make uninstall
```
Or
```bash
sudo rm -rf /etc/my_daemon/my_daemon.conf /etc/systemd/system/my_daemon.service /usr/bin/my_daemon
```

## Logging
Use the built-in **dlog** static class which uses syslog internally. Then you can
see your logs by:
- running `gnome-logs` gui if available.
- using cat by running `cat /var/log/syslog | grep your_daemon_name`.
- using tail `tail -f /var/log/syslog` to show all logs or `tail -f /var/log/syslog | grep your_daemon_name`
- using systemctl status by running `systemctl status your_daemon_name`
- opening the `/var/log/syslog` file in a text editor and find `your_daemon_name` (not recommended since syslog can be huge).

### TODO
- [x] re-read configuration file upon SIGHUP
- [x] relay information via event logging, often done using e.g., syslog(3)
- [ ] prevent against multiple instances via a lockfile
- [ ] allow for easy determination of PID via a pidfile
- [ ] include a system initialization script (for /etc/rc.d/, /etc/init.d/, systemd, …) for other Linux distros such as RedHat 4/5/6 or CentOS that use init scripts instead of systemd
- [ ] configuration file convention /etc/name.conf

### Features and bugs
If you face any problems feel free to open an issue at the [issue tracker][tracker]. If you feel the library is missing a feature, please raise a ticket on Github. Pull request are also welcome.

[tracker]: https://github.com/baderouaich/daemonpp/issues

### References
- [Jan Schaumann - Advanced Programming in the UNIX Environment Week 10, Segment 1: Dæmon Processes](https://www.youtube.com/watch?v=YbYQqVMv7b8f)
