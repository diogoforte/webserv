#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <string>
#include <map>
#include "server.hpp"

using std::string;
using std::map;

class TcpServer {
private:
  string ip_;
  int port_;
  int socket_;
  map<string, Server> virtualServers_;

public:
  TcpServer(const string &ip, const int &port);
  ~TcpServer();

  void add_virtual_server(const string &server_name, const Server &server);

  const string &get_ip(void);
  const int &get_port(void);
  const int &get_socket(void);
  map<string, Server> &get_virtual_servers(void);
};

#endif