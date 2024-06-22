#include <config.hpp>
#include <csignal>
#include <cstring>
#include <iostream>
#include <vector>
#include <webserver.hpp>

void exitSignalHandler(int signum) {
  (void)signum;
  WebServer::is_running = false;
}

int main(const int ac, const char **av) {
  signal(SIGINT, exitSignalHandler);
  try {
    WebServer webserver;
    if (ac > 2)
      WebServer::log("webserv: too many arguments", error);
    Config::parse_config_file(ac == 1 ? "configs/default.conf" : av[1],
                              webserver.get_servers());
    // Config::display_configs(webserver.get_servers());
    webserver.server_routine();
    WebServer::log("Webserver stopped", info);
  } catch (const std::runtime_error &e) {
    if (!strstr(e.what(), "[ERROR]"))
      std::cerr << RED << "[FATAL]	" << e.what() << RESET << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cerr << RED << "[FATAL]	" << e.what() << RESET << std::endl;
    return 1;
  }
}