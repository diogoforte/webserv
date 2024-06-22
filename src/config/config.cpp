#include "config.hpp"
#include "config.tpp"
#include "location.hpp"
#include "server.hpp"
#include <algorithm>
#include <iostream>
#include <webserver.hpp>

#pragma region Constructors
Config::Config() {}

Config::~Config() {}

Config::Config(const Config &other) { *this = other; }

#pragma endregion

#pragma region Setters

void Config::set_root(const std::string &root) { root_ = root; }

void Config::add_index(const std::string &index) { indexes_.push_back(index); }

void Config::add_error_page(const uint &error_code,
                            const std::string &error_page) {
  error_pages_.insert(std::pair<uint, std::string>(error_code, error_page));
}

void Config::set_max_client_body_size(const std::string &max_client_body_size) {
  max_client_body_size_ = max_client_body_size;
}

void Config::set_auto_index(const bool &autoindex) { auto_index_ = autoindex; }

void Config::set_redirection(const std::string &redirection) {
  redirection_ = redirection;
  if (redirection_.at(redirection_.size() - 1) == ';')
    redirection_.erase(redirection_.size() - 1);
  std::string type = redirection_.substr(redirection_.find_last_of(' ') + 1);
  if (type != "permanent" && type != "redirect")
    WebServer::log(ERR_CFG_REWRITE, error);
}
#pragma endregion

#pragma region Getters
const std::string &Config::get_root() { return (root_); }

std::vector<std::string> &Config::get_indexes() { return (indexes_); }

std::map<uint, std::string> &Config::get_error_pages() {
  return (error_pages_);
}

const std::string &Config::get_max_client_body_size() {
  return (max_client_body_size_);
}

const bool &Config::get_auto_index() { return (auto_index_); }

const std::string &Config::get_redirection() { return (redirection_); }
#pragma endregion

/// @brief Parse the IP address from the line from the config file and set it to
/// the server object
/// @param line The line from the config file
/// @param server The server object
void Config::host(const std::string &line, Server &server) {
  std::string host;
  size_t endPosition = line.size() - 1;

  if (line.size() > 0)
    endPosition =
        IS_CRLF(line.at(line.size() - 1)) ? line.size() - 2 : line.size() - 1;
  if (line.at(endPosition) != ';')
    WebServer::log(WARM_CFG_SEMICOLON, warning);
  if (line.find_first_of(':') != std::string::npos)
    host = line.substr(line.find_first_of(' ') + 1,
                       line.find_first_of(':') - line.find_first_of(' ') - 1);
  else
    host = line.substr(line.find_first_of(' ') + 1,
                       line.find_first_of(';') - line.find_first_of(' ') - 1);
  server.set_host(host);
}

/// @brief Parse the port from the line from the config file and set it to the
/// server object
/// @param line The line from the config file
/// @param server The server object
void Config::port(const std::string &line, Server &server) {
  int port = 80;
  size_t endPosition = line.size() - 1;

  if (line.size() > 0)
    endPosition =
        IS_CRLF(line.at(line.size() - 1)) ? line.size() - 2 : line.size() - 1;
  if (line.at(endPosition) != ';')
    WebServer::log(WARM_CFG_SEMICOLON, warning);
  if (line.find_first_of(':') != std::string::npos) {
    std::string port_str = line.substr(line.find_first_of(':') + 1);
    std::stringstream ss(port_str);
    ss >> port;
    if (ss.fail() || port < 0 || port > 65535) {
      WebServer::log(WARN_CFG_PORT, warning);
      port = 80;
    }
  } else
    WebServer::log(INFO_CFG_NOPORT, info);
  server.set_port(port);
}

/// @brief Parse location block from the config file and add it to the server
/// object
/// @param line The line from the config file
/// @param server The server object
/// @param line_number The starting line number
/// @param file The config file stream
void Config::location(std::string line, Server &server, std::ifstream &file) {
  Location location;
  size_t endPosition;
  location.set_path(
      line.substr(line.find_first_of(' ') + 1,
                  line.find_last_of(' ') - line.find_first_of(' ') - 1));
  location.get_config().set_auto_index(server.get_config().get_auto_index());
  while (std::getline(file, line) && !file.eof() &&
         line.find('}') == std::string::npos) {
    if (line.size() > 0)
      endPosition =
          IS_CRLF(line.at(line.size() - 1)) ? line.size() - 2 : line.size() - 1;
    if (line.find("root") != std::string::npos)
      location.get_config().set_root(
          line.substr(line.find_first_of(' ') + 1,
                      line.find_first_of(';') - line.find_first_of(' ') - 1));
    if (line.find("index") != std::string::npos &&
        line.find("autoindex") == std::string::npos)
      location.get_config().add_index(
          line.substr(line.find_first_of(' ') + 1,
                      line.find_first_of(';') - line.find_first_of(' ') - 1));
    if (line.find("error_page") != std::string::npos)
      parse_error_page(line, location);
    if (line.find("max_client_body_size") != std::string::npos)
      location.get_config().set_max_client_body_size(
          line.substr(line.find_first_of(' ') + 1,
                      line.find_first_of(';') - line.find_first_of(' ') - 1));
    if (line.find("location") != std::string::npos &&
        line.at(endPosition) == '{')
      WebServer::log(ERR_CFG_MISSING_BRACKET, error);
    if (line.find("limit_except") != std::string::npos)
      limit_except(line, location);
    if (line.find("autoindex") != std::string::npos) {
      std::string autoindex = line.substr(line.find_first_of(' ') + 1);
      location.get_config().set_auto_index(autoindex != "off");
    }
    if (line.find("rewrite") != std::string::npos) {
      if (line.find_first_of(' ') != std::string::npos)
        location.get_config().set_redirection(
            line.substr(line.find_first_of(' ') + 1));
    }
  }
  server.get_locations().push_back(location);
  int i = 0;
  i++;
}

/// @brief Parse the limit_except block from the config file and add it to the
/// location object
/// @param line The line from the config file
/// @param location The location object
void Config::limit_except(const std::string &line, Location &location) {
  std::stringstream ss(line);
  std::string word;
  while (ss >> word) {
    if (word != "limit_except" && word.find_first_of('{') == std::string::npos)
      location.add_allowed_methods(word);
  }
}

/// @brief Check line for ending semicolon and log a warning if it's missing
/// @param line The line from the config file
void Config::check_semicolon(const std::string &line) {
  if (line.size() == 0)
    return;
  size_t endPosition = line.size() - 1;

  if (line.size() > 2)
    endPosition =
        IS_CRLF(line.at(line.size() - 1)) ? line.size() - 2 : line.size() - 1;
  if (line.at(endPosition) != '{' && line.at(endPosition) != '}' &&
      line.at(endPosition) != ';' && line.at(endPosition) != '\r')
    WebServer::log(WARM_CFG_SEMICOLON, warning);
}

/// @brief Parse the config file and create server objects
/// @param path The path to the config file
/// @param servers The vector of server objects
void Config::parse_config_file(const std::string &path,
                               std::vector<Server> &servers) {
  std::ifstream file(path.c_str());
  if (!file.is_open())
    WebServer::log(ERR_CANT_OPEN_FILE + path, error);
  std::string line;
  size_t endPosition = 0;
  while (std::getline(file, line) && !file.eof()) {
    if (line.size() >= 2)
      endPosition =
          IS_CRLF(line.at(line.size() - 1)) ? line.size() - 2 : line.size() - 1;
    if (line.size() >= 2 && line.find("server") != std::string::npos &&
        line.at(endPosition) == '{') {
      Server server;
      while (std::getline(file, line) && !file.eof() &&
             line.find('}') == std::string::npos) {
        Config::check_semicolon(line);
        if (line.size() >= 2)
          endPosition = IS_CRLF(line.at(line.size() - 1)) ? line.size() - 2
                                                          : line.size() - 1;
        if (line.find("listen") != std::string::npos) {
          if (std::count(line.begin(), line.end(), ':') > 1)
            WebServer::log(ERR_CFG_LISTEN, error);
          host(line, server);
          port(line, server);
        }
        if (line.find("server_name") != std::string::npos)
          multi_value(line, server, std::string("server_name"));
        if (line.find("root") != std::string::npos) {
          if (line.find_first_of(' ') != std::string::npos)
            server.get_config().set_root(line.substr(
                line.find_first_of(' ') + 1,
                line.find_first_of(';') - line.find_first_of(' ') - 1));
        }
        if (line.find("index") != std::string::npos &&
            line.find("autoindex") == std::string::npos)
          multi_value(line, server, "index");
        if (line.find("error_page") != std::string::npos)
          parse_error_page(line, server);
        if (line.find("max_client_body_size") != std::string::npos)
          server.get_config().set_max_client_body_size(line.substr(
              line.find_first_of(' ') + 1,
              line.find_first_of(';') - line.find_first_of(' ') - 1));
        if (line.find("location") != std::string::npos) {
          if (line.at(endPosition) == '{')
            location(line, server, file);
          else
            WebServer::log(ERR_CFG_MISSING_BRACKET, error);
        }
        if (line.find("autoindex") != std::string::npos) {
          std::string autoindex = line.substr(line.find_first_of(' ') + 1,
                                              line.find_first_of(';') -
                                                  line.find_first_of(' ') - 1);
          server.get_config().set_auto_index(autoindex != "off");
        }
        if (line.find("rewrite") != std::string::npos) {
          if (line.find_first_of(' ') != std::string::npos)
            server.get_config().set_redirection(line.substr(
                line.find_first_of(' ') + 1,
                line.find_first_of(';') - line.find_first_of(' ') - 1));
        }
        if (line.find("server") != std::string::npos &&
            line.at(endPosition) == '{')
          WebServer::log(ERR_CFG_MISSING_BRACKET, error);
      }
      if (line.find('}') == std::string::npos) {
        WebServer::log(ERR_CFG_MISSING_BRACKET, error);
      }
      servers.push_back(server);
    }
  }
  file.close();
}

//! DEBUG CODE
void Config::display_configs(std::vector<Server> &servers) {
  for (size_t i = 0; i < servers.size(); i++) {
    std::cout << BLUE << "Host " << servers[i].get_host() << ":"
              << servers[i].get_port() << RESET << std::endl;
    std::cout << "Server name: ";
    for (size_t j = 0; j < servers[i].get_server_name().size(); j++) {
      std::cout << servers[i].get_server_name()[j] << " ";
    }
    std::cout << std::endl;
    std::cout << "Root: " << servers[i].get_config().get_root() << std::endl;
    std::cout << "Index: ";
    for (size_t j = 0; j < servers[i].get_config().get_indexes().size(); j++) {
      std::cout << servers[i].get_config().get_indexes()[j] << " ";
    }
    std::cout << std::endl;
    std::cout << "Error pages: ";
    for (std::map<uint, std::string>::iterator it =
             servers[i].get_config().get_error_pages().begin();
         it != servers[i].get_config().get_error_pages().end(); it++) {
      std::cout << it->first << "=>" << it->second << " ";
    }
    std::cout << std::endl;
    std::cout << "Max client body size: "
              << servers[i].get_config().get_max_client_body_size()
              << std::endl;
    std::cout << "Autoindex: "
              << (servers[i].get_config().get_auto_index() ? "on" : "off")
              << std::endl;
    std::cout << "Locations: " << std::endl;
    for (size_t j = 0; j < servers[i].get_locations().size(); j++) {
      std::cout << "	" << servers[i].get_locations()[j].get_path()
                << std::endl;
      std::cout << "	"
                << "Root: "
                << servers[i].get_locations()[j].get_config().get_root()
                << std::endl;
      std::cout << "	"
                << "Index: ";
      for (size_t k = 0;
           k < servers[i].get_locations()[j].get_config().get_indexes().size();
           k++) {
        std::cout
            << servers[i].get_locations()[j].get_config().get_indexes()[k];
      }
      std::cout << std::endl;
      std::cout << "	"
                << "Error pages: ";
      for (std::map<uint, std::string>::iterator it = servers[i]
                                                          .get_locations()[j]
                                                          .get_config()
                                                          .get_error_pages()
                                                          .begin();
           it !=
           servers[i].get_locations()[j].get_config().get_error_pages().end();
           it++) {
        std::cout << "	" << it->first << " " << it->second << " ";
      }
      std::cout << std::endl;
      std::cout << "	"
                << "Max client body size: "
                << servers[i]
                       .get_locations()[j]
                       .get_config()
                       .get_max_client_body_size()
                << std::endl;
      std::cout << "	"
                << "Allowed methods: ";
      for (size_t k = 0;
           k < servers[i].get_locations()[j].get_allowed_methods().size();
           k++) {
        std::cout << servers[i].get_locations()[j].get_allowed_methods()[k]
                  << " ";
      }
      std::cout << std::endl;
      std::cout << "	"
                << "Autoindex: "
                << (servers[i].get_locations()[j].get_config().get_auto_index()
                        ? "on"
                        : "off")
                << std::endl;
      std::cout << std::endl;
    }
  }
}