#include "daemon.hpp"
using namespace daemonpp;

class my_simple_daemon : public daemon
{
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
      if(counter >= 3){
        dlog::info("Stopping daemon after 3 updates");
        stop();
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
  my_simple_daemon dmn;
  dmn.set_name("my_simple_daemon");
  dmn.set_update_duration(std::chrono::seconds(3));
  dmn.set_cwd("/");
  dmn.run();
  return 0;
}







#if 0
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h> // daemon()
#include <signal.h>
int main() {

  /* It is also possible to use glibc function deamon()
		 * at this point, but it is useful to customize your daemon. */
  daemon(0, 0);


  while (true)
  {
    //TODO: Insert daemon code here.
    syslog (LOG_NOTICE, "Sample daemon started.");
    sleep (10);
    break;
  }

  syslog (LOG_NOTICE, "Sample daemon terminated.");
  closelog();
  return 0;
}
#endif
// todo provide a daemon main
