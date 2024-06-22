#include "http_parser.hpp"
#include "webserver.hpp"

void HttpParser::parse_request(const string &request) {
  size_t pos = request.find(' ');
  if (pos != string::npos)
    headers_["method"] = request.substr(0, pos);
  else
    WebServer::log("Invalid HTTP request: no method", error);

  size_t uri_start = pos + 1;
  pos = request.find(' ', uri_start);
  if (pos != string::npos)
    headers_["uri"] = request.substr(uri_start, pos - uri_start);
  else
    WebServer::log("Invalid HTTP request: no URI", error);
  size_t start = request.find("\r\n") + 2, end, separator;
  while ((end = request.find("\r\n", start)) != string::npos) {
    string line = request.substr(start, end - start);
    if ((separator = line.find(": ")) != string::npos)
      headers_[line.substr(0, separator)] = line.substr(separator + 2);
    start = end + 2;
  }
  pos = request.find("\r\n\r\n");
  if (pos != string::npos)
    headers_["body"] = request.substr(pos + 4);
  else
    WebServer::log("Invalid HTTP request: no body", error);
}

HttpParser::HttpParser(const string &request) { parse_request(request); }

HttpParser::~HttpParser() {}

const std::map<string, string> &HttpParser::get_headers() const {
  return headers_;
}
