#ifndef UTILS_HPP
#define UTILS_HPP

#include "server.hpp"
#include <string>
#include <sys/stat.h>

typedef struct stat Stat;

class Utils {
private:
  Utils();
  ~Utils();

  static bool is_text_file(const std::string &file_path);

public:
  static std::string read_file(const std::string &file_path);
  static std::string response_builder(const std::string &status_code,
                                      const std::string &status_message,
                                      const std::string &content_type,
                                      const long &content_length = 0);
  static std::string get_content_type(const std::string &file_path);
  static size_t get_max_size(const std::string &max_size);
  static bool should_generate_autoindex(const std::string &uri, Server &server);
  static const std::string create_autoindex(const std::string &path,
                                            const std::string &uri);
  static std::string path_finder(const std::string &command);
};

#endif