#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <config.hpp>
#include <string>

class Config;

class Location {
public:
#pragma region Constructors &Destructors

  Location();

  ~Location();

#pragma endregion

#pragma region Getters

  const std::string &get_path();

  Config &get_config();

  std::vector<std::string> &get_allowed_methods();

#pragma endregion

#pragma region Setters

  void set_path(const std::string &path);

  void add_allowed_methods(const std::string &method);

#pragma endregion

private:
  std::string path_;
  Config config_;
  std::vector<std::string> allowed_methods_;
};

#endif