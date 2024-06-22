#include "utils.hpp"
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>

Utils::Utils(void) {}
Utils::~Utils() {}

#pragma region Public

/// @brief Read requested file
/// @param file_path The path of the file
/// @return The content of the file
std::string Utils::read_file(const std::string &file_path) {
  std::fstream file;
  std::string content;

  const bool text_file = is_text_file(file_path);
  if (!text_file)
    file.open(file_path.c_str(), std::ios::in | std::ios::binary);
  else
    file.open(file_path.c_str(), std::ios::in);
  if (file.is_open()) {
    char buffer[1024];
    if (!text_file) {
      while (file.read(buffer, 1024))
        content.append(buffer, 1024);
    } else {
      while (file.getline(buffer, 1024))
        content += buffer;
    }
    file.close();
  } else
    throw std::runtime_error("404");
  return (content);
}

/// @brief Build the response header
/// @param status_code The status code
/// @param status_message The status message
/// @param content_type The content type
/// @param content_length The content length
/// @return The response header
std::string Utils::response_builder(const std::string &status_code,
                                    const std::string &status_message,
                                    const std::string &content_type,
                                    const long &content_length) {
  std::stringstream ss;

  ss << content_length;
  std::string response = "HTTP/1.1 " + status_code + " " + status_message +
                         "\r\n" + "Content-Type: " + content_type + "\r\n" +
                         "Content-Length:" + ss.str() + "\r\n\r\n";
  return (response);
}

/// @brief Get the content type of a file
/// @param file_path The path of the file
/// @return The content type of the file
std::string Utils::get_content_type(const std::string &file_path) {
  const std::string extension =
      file_path.substr(file_path.find_last_of('.') + 1);
  if (extension == "html" || extension == "css")
    return ("text/" + extension);
  if (extension == "js")
    return ("text/javascript");
  if (extension == "jpeg" || extension == "jpg")
    return ("image/jpeg");
  if (extension == "png")
    return ("image/png");
  if (extension == "gif")
    return ("image/gif");
  if (extension == "svg")
    return ("image/svg+xml");
  if (extension == "json")
    return ("application/json");
  if (extension == "xml")
    return ("application/xml");
  if (extension == "form")
    return ("application/x-www-form-urlencoded");
  return ("text/plain");
}

/// @brief Get the maximum size of the client body
/// @param max_size The maximum size set in the configuration file
/// @return The maximum size of the client body in bytes
size_t Utils::get_max_size(const std::string &max_size) {
  size_t size = 0;
  char unit;

  if (isalpha(*(max_size.end() - 1)))
    unit = static_cast<char>(std::tolower(*(max_size.end() - 1)));
  else
    unit = 'b';
  switch (unit) {
  case 'k':
    size = std::atoi(max_size.c_str()) * 1024;
    break;
  case 'm':
    size = std::atoi(max_size.c_str()) * 1024 * 1024;
    break;
  case 'g':
    size = std::atoi(max_size.c_str()) * 1024 * 1024 * 1024;
    break;
  default:
    size = std::atoi(max_size.c_str());
    break;
  }
  return size;
}

/// @brief Check if the server should generate an autoindex page
/// @param uri The URI of the request
/// @param server A reference to the server
/// @return True if the server should generate an autoindex page, false
/// otherwise
bool Utils::should_generate_autoindex(const std::string &uri, Server &server) {
  if (uri == "/" && server.get_config().get_auto_index())
    return true;
  else if (*(uri.end() - 1) == '/') {
    std::string fixed_uri(uri);
    fixed_uri.erase(fixed_uri.end() - 1);
    std::vector<Location> locations = server.get_locations();
    if (locations.empty() && server.get_config().get_auto_index())
      return true;
    for (size_t i = 0; i < locations.size(); i++) {
      if (fixed_uri == locations.at(i).get_path() &&
          locations.at(i).get_config().get_auto_index())
        return true;
    }
  }
  return false;
}

/// @brief Convert the size of a file to a human-readable format
/// @param size The size of the file in bytes
/// @return The size of the file in a human-readable format (e.g. 1 KB, 2 MB)
static std::string size_converter(off_t size) {
  std::string size_str;
  std::stringstream ss;
  float fsize = static_cast<float>(size);
  long KB = 1024;
  long MB = 1024 * 1024;
  long GB = 1024 * 1024 * 1024;

  if (size < KB)
    return (ss << size, ss.str() + " B");
  if (size < MB)
    return (ss << std::fixed << std::setprecision(2) << fsize / KB,
            ss.str() + " KB");
  if (size < GB)
    return (ss << std::fixed << std::setprecision(2) << fsize / MB,
            ss.str() + " MB");
  if (size < GB * 1024)
    return (ss << std::fixed << std::setprecision(2) << fsize / GB,
            ss.str() + " GB");
  return (ss << size, ss.str() + " B");
}

/// @brief Create an autoindex page
/// @param path The path of the directory
/// @param uri The URI of the directory
/// @return The autoindex page content
const std::string Utils::create_autoindex(const std::string &path,
                                          const std::string &uri) {
  std::string autoindex;
  autoindex = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + uri +
              "</title>\n<link rel=\"stylesheet\" href=\"/styles.css\"> "
              "\n</head>\n<body>\n<h1 id=\"autoindex\">Index of " +
              uri + "</h1>\n";
  autoindex += "<table>\n<tr>\n<th>Name</th>\n<th>Last Modified</th>\n";
  autoindex += "<th>Size</th>\n</tr>\n";
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      struct stat st;
      std::string size;
      std::string file_path = path + "/" + std::string(ent->d_name);
      stat(file_path.c_str(), &st);
      size = size_converter(st.st_size);
      std::string itemName = S_ISDIR(st.st_mode)
                                 ? std::string(ent->d_name) + "/"
                                 : std::string(ent->d_name);
      autoindex +=
          "<tr>\n<td><a href=\"" + itemName + "\">" + itemName + "</a></td>\n";
      autoindex += "<td>" + std::string(ctime(&st.st_mtime)) + "</td>\n";
      autoindex += "<td>" + size + "</td>\n</tr>\n";
    }
    closedir(dir);
  } else {
    return "";
  }
  autoindex += "</table>\n</body>\n</html>";
  return autoindex;
}

/// @brief Try to find the path of a cgi program
/// @param command The command to find
/// @return The path of the cgi program
std::string Utils::path_finder(const std::string &command) {
  std::string path;
  std::string env_path = std::getenv("PATH");
  if (env_path.empty())
    return "";
  std::vector<std::string> paths;
  std::stringstream ss(env_path);
  std::string item;
  while (std::getline(ss, item, ':'))
    paths.push_back(item);
  for (size_t i = 0; i < paths.size(); i++) {
    path = paths.at(i) + "/" + command;
    if (access(path.c_str(), X_OK) == 0)
      return path;
  }
  return "";
}
#pragma endregion

#pragma region Private

/// @brief Check if a file is a text file
/// @param file_path The path of the file
/// @return True if the file is a text file, false otherwise
bool Utils::is_text_file(const std::string &file_path) {
  const std::string extension =
      file_path.substr(file_path.find_last_of('.') + 1);
  if (extension == "html" || extension == "css" || extension == "js" ||
      extension == "json" || extension == "xml" || extension == "svg")
    return (true);
  return (false);
}
#pragma endregion
