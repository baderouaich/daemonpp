#include "daemon.hpp"
using namespace daemonpp;

class my_simple_daemon : public daemon
{
public:
    void on_start() override {
      /// Runs once after daemon starts:
      /// Initialize your code here...

      dlog::info("on_start");
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

    void on_restart() override {
      dlog::info("on_restart");
    }

private:
    int counter = 0;
};


int main(int argc, const char* argv[]) {
  my_simple_daemon dmn;
  dmn.set_name("my_simple_daemon");
  dmn.set_update_duration(std::chrono::seconds(3));
  dmn.set_cwd("/");
  dmn.run();
  return 0;
}