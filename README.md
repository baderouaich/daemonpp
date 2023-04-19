## daemonpp 
Simple C++ lib to create Linux daemons.

**(under development)**

### What are daemon services/processes ?
A dæmon is a program that runs silently in the background.<br>
Commonly, dæmon processes are created to offer a specific service.

**Dæmon** processes usually:
- live for a long time;
- are started at boot time;
- terminate only during shutdown;
- have no controlling terminal.

### Dæmon Characteristics
The previously listed characteristics have certain implications:

- do one thing, and one thing only
- resource leaks eventually surface
- consider current working directory
- no (or only limited) user-interaction possible
- how to create (debugging) output

### Target api (under development)
```cpp
#include "daemon.hpp"
using namespace daemonpp;

class my_simple_daemon : public daemon {
public:
    void on_start() override {
      /// Runs once after daemon starts:
      /// Initialize your code here...

      dlog::info("Started daemon "+ get_name() + " on working dir " + get_cwd());
    }

    void on_update() override {
      /// Runs every DURATION set in set_update_duration()...
      /// Update your code here...

      dlog::info("on_update: " + std::to_string(counter));
      counter++;
      if(counter >= 3) {
        dlog::info("Stopping daemon after 3 updates");
        this->stop();
      }
    }

    void on_exit() override {
      /// Runs once before daemon is about to exit.
      /// Cleanup your code here...

      dlog::info("on_exit");
    }

private:
    int counter = 0;
};

int main() {
  my_simple_daemon dmn; // create a daemon instance
  dmn.set_name("my_simple_daemon"); // set daemon name which can be identified in logs
  dmn.set_update_duration(std::chrono::seconds(3)); // set duration to sleep before triggering every update
  dmn.set_cwd("/"); // set daemon's current working directory
  dmn.run();  // run your daemon
  return 0;
}
```

### Logging
Use the built-in **dlog** static class which uses syslog internally. Then you can 
see your logs by:
- running `gnome-logs` gui if available.
- using cat by running `cat /var/log/syslog | grep your_daemon_name`. 
- using tail `tail -f /var/log/syslog` to show all logs or `tail -f /var/log/syslog | grep your_daemon_name`
- opening the `/var/log/syslog` file in a text editor and find `your_daemon_name` (not recommended since syslog can be huge).

### TODO
- [ ] prevent against multiple instances via a lockfile
- [ ] allow for easy determination of PID via a pidfile
- [ ] include a system initialization script (for /etc/rc.d/, /etc/init.d/, systemd, …)
- [ ] configuration file convention /etc/name.conf
- [ ] re-read configuration file upon SIGHUP
- [ ] relay information via event logging, often done using e.g., syslog(3)


### References
- [Jan Schaumann - Advanced Programming in the UNIX Environment Week 10, Segment 1: Dæmon Processes](./guide-linux-daemons.pdf)
