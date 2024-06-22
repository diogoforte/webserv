#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Server;
class Location;

// End of line check
#define IS_CRLF(c) (c == '\r')

class Config {
public:
#pragma region File Parser

  static void parse_config_file(const std::string &path,
                                std::vector<Server> &servers);

  static void display_configs(std::vector<Server> &servers);

  static void host(const std::string &line, Server &server);

  static void port(const std::string &line, Server &server);

  static void location(std::string line, Server &server, std::ifstream &file);

  static void limit_except(const std::string &line, Location &location);

  static void check_semicolon(const std::string &line);
#pragma endregion

#pragma region Constructors &Destructors

  Config();

  ~Config();

  Config(const Config &other);

#pragma endregion

#pragma region Getters

  const std::string &get_root();

  std::vector<std::string> &get_indexes();

  std::map<uint, std::string> &get_error_pages();

  const std::string &get_max_client_body_size();

  const bool &get_auto_index();

  const std::string &get_redirection();

#pragma endregion

#pragma region Setters

  void set_root(const std::string &root);

  void add_index(const std::string &index);

  void add_error_page(const uint &error_code, const std::string &error_page);

  void set_max_client_body_size(const std::string &max_client_body_size);

  void set_auto_index(const bool &autoindex);

  void set_redirection(const std::string &redirection);

#pragma endregion

private:
  std::string root_;
  std::vector<std::string> indexes_;
  std::map<uint, std::string> error_pages_;
  std::string max_client_body_size_;
  std::string redirection_;
  bool auto_index_;
};

#endif