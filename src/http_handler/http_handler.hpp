#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "error_page_handler.hpp"
#include "http_parser.hpp"

#define IS_VALID_BUT_NOT_SUPPORTED(method)                                     \
  (method == "PUT" || method == "PATCH" || method == "TRACE" ||                \
   method == "CONNECT" || method == "OPTIONS" || method == "HEAD")

typedef struct stat Stat;

enum redirectionType { REDIRECT_307, REDIRECT_308 };

class HttpHandler {
public:
  HttpHandler(const string &request, Server &server, Client &client);

  ~HttpHandler();

  string process_request();

private:
  Server *server_;
  std::map<string, string> headers_;
  ErrorPageHandler error_page_handler_;
  Client *client_;

  string process_get();

  string process_post();

  string process_delete();

  string create_redirection_response();

  string get_file_path(const string &uri);

  string get_location_path(const string &uri);

  bool is_method_allowed(const string &method);

  bool has_redirection(const string &uri);

  bool is_cgi_script();

  string process_cgi();
};

#endif