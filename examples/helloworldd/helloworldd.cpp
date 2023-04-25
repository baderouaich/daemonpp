#include "daemon.hpp"
using namespace daemonpp;

class helloworldd : public daemon
{
public:
    void on_start(const dconfig& cfg) override {
      /// Runs once after daemon starts:
      /// Initialize your code here...

      dlog::info("on_start: helloworldd version " + cfg.get("version") + " started!");
    }

    void on_update() override {
      /// Runs every DURATION set in set_update_duration()...
      /// Update your code here...

      dlog::info("Hello, World!");
    }

    void on_stop() override {
      /// Runs once before daemon is about to exit.
      /// Cleanup your code here...

      dlog::info("on_stop: helloworldd stopped.");
    }

    void on_reload(const dconfig& cfg) override {
      /// Runs once after your daemon is reloaded
      /// Runs once after your daemon's config or service files are updated then reloaded with `$ systemctl reload my_daemon`

      dlog::info("on_reload: helloworldd reloaded: " + cfg.get("version"));
    }
};


int main(int argc, const char* argv[]) {
  helloworldd dmn;
  dmn.set_name("helloworldd");
  dmn.set_update_duration(std::chrono::minutes(1));
  dmn.set_cwd("/");
  dmn.run(argc, argv);
  return 0;
}