#include "location.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <server.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <webserver.hpp>

#pragma region Constructors &Destructors

Server::Server()
    : host_("127.0.0.1"), port_(8080), config_(), socket_(-1),
      connected_clients_() {
  config_.set_root("/");
  config_.set_max_client_body_size("1m");
  config_.add_index("index.html");
  config_.set_auto_index(false);
}

Server::~Server() {
  if (socket_ > 0)
    close(socket_);
  for (std::vector<Client>::iterator it = connected_clients_.begin();
       it != connected_clients_.end(); ++it)
    close(it->get_socket());
};

#pragma endregion

#pragma region Operator Overload

Server &Server::operator=(const Server &other) {
  if (this != &other) {
    host_ = other.host_;
    port_ = other.port_;
    server_name_ = other.server_name_;
    config_ = other.config_;
    locations_ = other.locations_;
    socket_ = other.socket_;
    connected_clients_ = other.connected_clients_;
  }
  return *this;
}

#pragma endregion

#pragma region Setters

void Server::set_host(const std::string &host) { host_ = host; }

void Server::set_port(const int &port) { port_ = port; }

void Server::set_server_name(const std::string &server_name) {
  server_name_.push_back(server_name);
}

void Server::set_socket(const int socket) { socket_ = socket; }

#pragma endregion

#pragma region Getters

const int &Server::get_port() const { return (port_); }

const std::string &Server::get_host() { return (host_); }

const std::vector<std::string> &Server::get_server_name() {
  return (server_name_);
}

Config &Server::get_config() { return (config_); }

std::vector<Location> &Server::get_locations() { return (locations_); }

const int &Server::get_socket() const { return (socket_); }

std::vector<Client> &Server::get_connected_clients() {
  return (connected_clients_);
}

#pragma endregion