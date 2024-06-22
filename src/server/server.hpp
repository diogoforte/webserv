#ifndef SERVER_HPP
#define SERVER_HPP

#include <client.hpp>
#include <config.hpp>
#include <location.hpp>
#include <string>
#include <vector>

class Location;

class Config;

class Server {
public:
#pragma region Constructors &Destructors

  Server();

  ~Server();

#pragma endregion

#pragma region Operator Overload

  Server &operator=(const Server &server);

#pragma endregion

#pragma region Getters

  const int &get_port() const;

  const std::string &get_host();

  const std::vector<std::string> &get_server_name();

  Config &get_config();

  std::vector<Location> &get_locations();

  const int &get_socket() const;

  std::vector<Client> &get_connected_clients();

#pragma endregion

#pragma region Setters

  void set_host(const std::string &host);

  void set_port(const int &port);

  void set_server_name(const std::string &server_name);

  void set_socket(int socket);

#pragma endregion

private:
  std::string host_;
  int port_;
  std::vector<std::string> server_name_;
  Config config_;
  std::vector<Location> locations_;
  int socket_;
  std::vector<Client> connected_clients_;
};

#endif