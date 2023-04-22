#include "include/daemon.hpp"
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