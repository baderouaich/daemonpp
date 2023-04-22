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

class my_simple_daemon : public daemon
{
public:
    void on_start(const dconfig& cfg) override {
      /// Called once after daemon starts:
      /// Initialize your code here...
      dlog::info("on_start: daemon version from config: " + cfg.get("version"));
    }

    void on_update() override {
      /// Called every DURATION set in set_update_duration()...
      /// Update your code here...
      dlog::info("on_update: " + std::to_string(counter));

      counter++;
      if(counter >= 3){
        dlog::info("Stopping daemon after 3 updates");
        stop(EXIT_SUCCESS);
      }
    }

    void on_stop() override {
      /// Called once before daemon is about to exit.
      /// Cleanup your code here...
      dlog::info("on_stop");
    }

    void on_reload(const dconfig& cfg) override {
      /// Called once after your daemon's config or service files are updated
      /// then reloaded with `$ systemctrl reload my_daemon`
      dlog::info("on_reload: new daemon version from updated config: " + cfg.get("version"));
    }

private:
    int counter = 0;
};

int main(int argc, const char* argv[]) {
  my_simple_daemon dmn; // create a daemon instance
  dmn.set_name("my_simple_daemon"); // set daemon name to identify logs in syslog
  dmn.set_update_duration(std::chrono::seconds(3)); // set duration to sleep before triggering the on_update callback everytime
  dmn.set_cwd("/"); // set daemon's current working directory to root /
  dmn.run(argc, argv);  // run your daemon
  return 0;
}
```

### Examples
See [examples](./examples)

## Tutorial - Create a new daemon project step by step
Let's assume your daemon project is called **my_daemon**
1. First, clone this repository into a new folder named after your project:
```bash
git clone https://github.com/baderouaich/daemonpp my_daemon
```
2. Your project structure will be like this:
    ```text
    my_daemon
            ├── include/            # include files contains daemonpp single header files
            ├── examples/           # example daemon projects to inspire from
            ├── daemonpp.conf       # daemon config file (required)
            ├── daemonpp.service    # daemon service file for systemd (required)
            ├── daemonpp.cpp        # daemonpp.cpp simple daemon example
            ├── CMakeLists.txt      # CMake project file 
            ├── LICENSE
            └── README.md
    ```
* 2.1 Update project name according to your project name in the [CMakeLists.txt](CMakeLists.txt) file
    ```cmake
    cmake_minimum_required(VERSION 3.10)
    project(my_daemon VERSION "0.0.1" DESCRIPTION "My daemon description" LANGUAGES CXX)
              ^                                            ^
              │_____ update your project name              |
                                                           |
                                                           │_____ update your project description
    ```

* 2.1 Rename [daemonpp.service](daemonpp.service) to your project name `my_daemon.service`

* 2.2 Update your `my_daemon.service` file according to your project (replace `daemonpp` with `my_daemon`)
    ```unit file (systemd)
    [Unit]
    Description=SHORT DESCRIPTION ABOUT my_daemon
    After=network.target
    
    [Service]
    Type=forking
    ExecStart=/usr/bin/my_daemon --config /etc/my_daemon/my_daemon.conf
    ExecReload=/bin/kill -s SIGHUP $MAINPID
    ExecStop=/bin/kill -s SIGTERM $MAINPID
    User=root
    StandardError=syslog
    SyslogIdentifier=my_daemon
    
    [Install]
    WantedBy=multi-user.target
    ```

* 2.3 Rename [daemonpp.conf](daemonpp.conf) to your project name `my_daemon.conf`

* 2.4 Update your `my_daemon.conf` config file accordingly (optional)
    ```toml
    # here you can put your daemon configuration which you can get in the on_start and on_reload callbacks.
    name="my_daemon"
    version="0.0.1"
    ```
  
3. Now your project structure will look like this:
```text
my_daemon
├── include/        
├── examples/        
├── my_daemon.conf       
├── my_daemon.service    
├── my_daemon.cpp       
├── CMakeLists.txt   
├── LICENSE
└── README.md
```
 Build your daemon:
```bash 
mkdir build && cd build 
cmake ..
make my_daemon
```

* 3.1 [Install your daemon](#install-your-daemon)
* 3.2 [Enable/Disable your daemon to run with system startup](#enabledisable-your-daemon-to-run-with-system-startup)
* 3.3 [Start/Stop your daemon](#startstop-your-daemon)
* 3.4 [Restart your daemon](#'estart-your-daemon)
* 3.5 [Reload your daemon](#install-your-daemon)
* 3.6 [Uninstall your daemon](#uninstall-your-daemon)

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
> will trigger the on_start() callback.

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
if you change  your .service or .conf files, you have to reload your daemon by executing:<br>
`systemctl reload my_daemon` which will call
> will trigger the on_reload() callback.

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
- using systemctrl status by running `systemctrl status your_daemon_name`
- opening the `/var/log/syslog` file in a text editor and find `your_daemon_name` (not recommended since syslog can be huge).

### TODO
- [x] re-read configuration file upon SIGHUP
- [x] relay information via event logging, often done using e.g., syslog(3)
- [ ] prevent against multiple instances via a lockfile
- [ ] allow for easy determination of PID via a pidfile
- [ ] include a system initialization script (for /etc/rc.d/, /etc/init.d/, systemd, …)
- [ ] configuration file convention /etc/name.conf

### Features and bugs
If you face any problems feel free to open an issue at the [issue tracker][tracker]. If you feel the library is missing a feature, please raise a ticket on Github. Pull request are also welcome.

[tracker]: https://github.com/baderouaich/daemonpp/issues

### References
- [Jan Schaumann - Advanced Programming in the UNIX Environment Week 10, Segment 1: Dæmon Processes](./guide-linux-daemons.pdf)