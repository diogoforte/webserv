#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
  int socket_;
  std::string request_;
  std::string response_;

public:
  Client(int socket);
  ~Client();
  void set_request(const std::string &request);
  void set_response(const std::string &response);
  std::string &get_request();
  std::string &get_response();
  int get_socket();
  void set_socket(int socket);
};

#endif
