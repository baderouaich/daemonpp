#include "daemon.hpp"
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h> // NOTE: don't forget to put in your .service file Environment="LD_LIBRARY_PATH=/usr/local/lib"
                    // so cpr can locate system libs it needs
using namespace daemonpp;
namespace nl = nlohmann;
namespace fs = std::filesystem;

class httpreqd : public daemon
{
private:
  /// See news bored docs: https://www.boredapi.com/ (Find random activities to fight boredom)
  const std::string BORED_API_ENDPOINT = "https://www.boredapi.com/api/activity";

  nl::json http_get(const std::string& endpoint){
    cpr::Response res = cpr::Get(cpr::Url{endpoint});
    if(res.status_code == cpr::status::HTTP_OK){
        return nl::json::parse(res.text);
    }
    throw std::runtime_error("HTTP GET " + endpoint + " failed");
  }

public:
    void on_start([[maybe_unused]] const dconfig& cfg) override {
      /// Called once after daemon starts automatically with system startup or when you manually call `$ systemctl start httpreqd`
      /// Initialize your code here...

      dlog::info("httpreqd::on_start()");
    }

    void on_update() override {
      /// Called every DURATION set in set_update_duration()...
      /// Update your code here...

      try {

        nl::json response = http_get(BORED_API_ENDPOINT);
        dlog::info(response["activity"]);

      } catch(const std::exception& e) {
        dlog::error(e.what());
      }

    }

    void on_stop() override {
      /// Called once before daemon is about to exit with system shutdown or when you manually call `$ systemctl stop httpreqd`
      /// Cleanup your code here...
      dlog::info("httpreqd::on_stop()");


    }

    void on_reload([[maybe_unused]] const dconfig& cfg) override {
      /// Called once after your daemon's config fil is updated then reloaded with `$ systemctl reload httpreqd`
      /// Handle your config updates here...

      dlog::info("httpreqd::on_reload()");
    }
};

int main(int argc, const char* argv[]) {
  httpreqd dmn;
  dmn.set_name("httpreqd");
  dmn.set_update_duration(std::chrono::minutes(1));
  dmn.set_cwd("/");
  dmn.run(argc, argv);
  return EXIT_SUCCESS;
}
