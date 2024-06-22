#include "tcp_server.hpp"

TcpServer::TcpServer(const string &ip, const int &port) : ip_(ip), port_(port) {}
TcpServer::~TcpServer() {}

void TcpServer::add_virtual_server(const string &server_name, const Server &server) {
  virtualServers_[server_name] = server;
}

const string &TcpServer::get_ip(void) {
  return ip_;
}

const int &TcpServer::get_port(void) {
  return port_;
}

const int &TcpServer::get_socket(void) {
  return socket_;
}

map<string, Server> &TcpServer::get_virtual_servers(void) {
  return virtualServers_;
}