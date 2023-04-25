#include "daemon.hpp"
using namespace daemonpp;
using namespace std::chrono_literals;

class temperatured : public daemon
{
public:
    void on_start(const dconfig& cfg) override {
      /// Called once after daemon starts:
      /// Initialize your code here...
      dlog::info("on_start: temperatured started!");

      // Note that our current working directory is pointed at /tmp (see main function)
      // this file will be created at /tmp/temperatured.txt
      temperature_history_file.open("temperatured.txt", std::ios::app);
    }

    void on_update() override {
      /// Called every DURATION set in set_update_duration()...
      /// Update your code here...

      double curr_temperature_celsius = get_cpu_temperature();
      temperature_history_file << '[' << get_current_date_time() << "]: " << curr_temperature_celsius << "°C ";
      if(is_between(curr_temperature_celsius, 10.0, 40.0))
        temperature_history_file << "(Cool)";
      else if(is_between(curr_temperature_celsius, 41.0, 65.0))
        temperature_history_file << "(Normal)";
      else if(is_between(curr_temperature_celsius, 66.0, 70.0))
        temperature_history_file << "(Average)";
      else if(is_between(curr_temperature_celsius, 71.0, 80.0))
        temperature_history_file << "(High)";
      else if(is_between(curr_temperature_celsius, 81.0, 100.0))
        temperature_history_file << "(Cooking)";
      else
        temperature_history_file << "(Unknown)";

      temperature_history_file << std::endl;

    }

    void on_stop() override {
      /// Called once before daemon is about to exit.
      /// Cleanup your code here...
      if(temperature_history_file.is_open())
        temperature_history_file.close();
      dlog::info("on_stop: temperatured stopped.");
    }

    void on_reload(const dconfig& cfg) override {
      /// Called once after your daemon's config or service files are updated
      /// then reloaded with `$ systemctl reload my_daemon`
      dlog::info("on_reload: temperatured reloaded: " + cfg.get("version"));
    }

private:
    /**
     * Returns cpu average temperature of all cores
     * @return floating point cpu temperature in celsius
     */
    double get_cpu_temperature() {
      std::vector<double> cores_temp;
      for(int i = 0; ; i++)
      {
        std::ifstream core_temp_file("/sys/class/thermal/thermal_zone"+std::to_string(i)+"/temp");
        if(!core_temp_file.good()) break;
        std::string line;
        while (std::getline(core_temp_file, line)) {
          double temp{};
          std::istringstream line_ss(line);
          line_ss >> temp;
          temp /= 1000.0;
          cores_temp.push_back(temp);
        }
      }

      double average_temp = 0.0;
      for(const double& temp : cores_temp)
          average_temp += temp;
      average_temp /= static_cast<double>(cores_temp.size());

      return average_temp;
    }

    std::string get_current_date_time() {
      char buffer[64]{};
      std::time_t now = std::time(nullptr);
      std::tm* tm_ = std::localtime(&now);
      std::strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S", tm_);
      return std::string{buffer};
    }

    template<typename T>
    bool is_between(const T& value, const T& min, const T& max){
      return value >= min && value <= max;
    }

private:
    std::ofstream temperature_history_file;
};


int main(int argc, const char* argv[]) {
  temperatured dmn;
  dmn.set_name("temperatured");
  dmn.set_update_duration(1s);
  dmn.set_cwd("/tmp");
  dmn.run(argc, argv);
  return 0;
}
