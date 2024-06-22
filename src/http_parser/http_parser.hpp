#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <map>
#include <ostream>
#include <string>

using std::string;

class HttpParser {
private:
  std::map<string, string> headers_;
  void parse_request(const string &request);

public:
  explicit HttpParser(const string &request);
  ~HttpParser();
  const std::map<string, string> &get_headers() const;
};

#endif