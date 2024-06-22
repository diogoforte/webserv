#include "error_page_handler.hpp"
#include "utils.hpp"
#include "webserver.hpp"

ErrorPageHandler::ErrorPageHandler() {}
ErrorPageHandler::ErrorPageHandler(Server &server,
                                   std::map<std::string, std::string> &headers)
    : headers_(headers), server_(&server) {}
ErrorPageHandler::~ErrorPageHandler() {}

/// @brief Get the error page
/// @param status_code The status code
/// @return The error page content
std::string ErrorPageHandler::get_error_page(const int status_code) {
  std::string content;
  std::string path;

  try {
    switch (status_code) {
    case 400:
      content = Utils::read_file(get_error_page_path(400));
      WebServer::log(std::string(HTTP_400) + headers_.at("uri"), warning);
      return Utils::response_builder("400", "Bad Request", "text/html",
                                     content.length()) +
             content;
    case 403:
      content = Utils::read_file(get_error_page_path(403));
      WebServer::log(std::string(HTTP_403) + headers_.at("uri"), warning);
      return Utils::response_builder("403", "Forbidden", "text/html",
                                     content.length()) +
             content;
    case 404:
      content = Utils::read_file(get_error_page_path(404));
      WebServer::log(std::string(HTTP_404) + headers_.at("uri"), warning);
      return Utils::response_builder("404", "Not Found", "text/html",
                                     content.length()) +
             content;
    case 405:
      content = Utils::read_file(get_error_page_path(405));
      WebServer::log(std::string(HTTP_405) + headers_.at("uri"), warning);
      return Utils::response_builder("405", "Method Not Allowed", "text/html",
                                     content.length()) +
             content;
    case 413:
      content = Utils::read_file(get_error_page_path(413));
      WebServer::log(std::string(HTTP_413) + headers_.at("uri"), warning);
      return Utils::response_builder("413", "Content Too Large", "text/html",
                                     content.length()) +
             content;
    case 501:
      content = Utils::read_file(get_error_page_path(501));
      WebServer::log(std::string(HTTP_501) + headers_.at("uri"), warning);
      return Utils::response_builder("501", "Not Implemented", "text/html",
                                     content.length()) +
             content;
    case 502:
      content = Utils::read_file(get_error_page_path(502));
      WebServer::log(std::string(HTTP_502) + headers_.at("uri"), warning);
      return Utils::response_builder("502", "Bad Gateway", "text/html",
                                     content.length()) +
             content;
    default:
      content = Utils::read_file(get_error_page_path(500));
      WebServer::log(std::string(HTTP_500) + headers_.at("uri"), warning);
      return Utils::response_builder("500", "Internal Server Error",
                                     "text/html", content.length()) +
             content;
    }
  } catch (...) {
    return Utils::response_builder("500", "Internal Server Error", "text/plain",
                                   22) +
           " Internal Server Error";
  }
}

/// @brief Get the path of the error page
/// @param status_code The status code
/// @return The path of the error page
std::string ErrorPageHandler::get_error_page_path(const int status_code) {
  std::string path;
  std::string location;
  Stat buffer;
  bool hasCustomErrorPage = false;
  bool isValidPath = false;

  get_route_error_page(hasCustomErrorPage, status_code, path);
  isValidPath = stat(path.c_str(), &buffer) == 0;
  switch (status_code) {
  case 400:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/bad_request.html"
               : path;
  case 403:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/forbidden.html"
               : path;
  case 404:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/not_found.html"
               : path;
  case 405:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/not_allowed.html"
               : path;
  case 413:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/content_too_large.html"
               : path;
  case 501:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/not_implemented.html"
               : path;
  case 502:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/bad_gateway.html"
               : path;
  default:
    return (!hasCustomErrorPage || !isValidPath)
               ? "default_pages/server_error.html"
               : path;
  }
}

/// @brief Get the error page for the route
/// @param hasCustomErrorPage Set value to true if the route has a custom error
/// page
/// @param status_code The status code
/// @param path Set the path of the error page
void ErrorPageHandler::get_route_error_page(bool &hasCustomErrorPage,
                                            const int status_code,
                                            std::string &path) {
  std::string location;
  location = headers_.at("uri").substr(0, headers_.at("uri").find_last_of('/'));
  std::vector<Location> locations = server_->get_locations();
  try {
    for (size_t i = 0; i < locations.size(); i++) {
      if (location == locations.at(i).get_path()) {
        hasCustomErrorPage =
            locations.at(i).get_config().get_error_pages().find(status_code) !=
            locations.at(i).get_config().get_error_pages().end();
        if (locations.at(i).get_config().get_root().empty())
          path = server_->get_config().get_root() +
                 locations.at(i).get_config().get_error_pages().at(status_code);
        else
          path = server_->get_config().get_root() +
                 locations.at(i).get_config().get_root() +
                 locations.at(i).get_config().get_error_pages().at(status_code);
        break;
      }
      if (i == locations.size() - 1) {
        hasCustomErrorPage =
            server_->get_config().get_error_pages().find(status_code) !=
            server_->get_config().get_error_pages().end();
        path = server_->get_config().get_root() +
               server_->get_config().get_error_pages().at(status_code);
      }
    }
  } catch (const std::out_of_range &e) {
    hasCustomErrorPage = false;
  }
}