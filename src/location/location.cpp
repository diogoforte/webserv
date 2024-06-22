#include "location.hpp"

#pragma region Constructors &Destructors

Location::Location() : config_() { config_.set_auto_index(false); }

Location::~Location() {}

#pragma endregion

#pragma region Getters

const std::string &Location::get_path() { return (path_); }

Config &Location::get_config() { return (config_); }

std::vector<std::string> &Location::get_allowed_methods() {
  return (allowed_methods_);
}

#pragma endregion

#pragma region Setters

void Location::set_path(const std::string &path) { path_ = path; }

void Location::add_allowed_methods(const std::string &method) {
  allowed_methods_.push_back(method);
}

#pragma endregion