#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <http_handler.hpp>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <webserver.hpp>

std::ofstream WebServer::log_file_;

bool WebServer::is_running = true;

#pragma region Constructors & Destructors

WebServer::WebServer()
    : epoll_fd_(-1), events_(), server_number_(0), status_code_() {
  log_file_.open("logs/webserv.log", std::ios::out | std::ios::trunc);
  if (!log_file_)
    throw std::runtime_error("Failed to open log file");
  log("WebServer started", info);
}

WebServer::~WebServer() {
  log_file_.close();
  if (epoll_fd_ > 0)
    close(epoll_fd_);
}

#pragma endregion

#pragma region Getters

std::vector<Server> &WebServer::get_servers() { return servers_; }

#pragma endregion

#pragma region Setup

void WebServer::setup_sockets() {
  for (std::vector<Server>::iterator it = servers_.begin();
       it != servers_.end(); ++it)
    it->socket_setup();
}

void WebServer::setup_epoll() {
  epoll_fd_ = epoll_create(1);
  if (epoll_fd_ == -1)
    log("Failed to create epoll file descriptor", error);
  for (std::vector<Server>::iterator it = servers_.begin();
       it != servers_.end(); ++it)
    insert_epoll(it->get_socket_fd());
}

void WebServer::insert_epoll(int socket_fd) const {
  epoll_event event = {};
  event.events = EPOLLIN;
  event.data.fd = socket_fd;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd, &event) == -1)
    log("Failed to add file descriptor to epoll", error);
}

#pragma endregion

#pragma region Connection Handling

bool WebServer::is_server_socket() {
  for (std::vector<Server>::iterator it = servers_.begin();
       it != servers_.end(); ++it)
    if (it->get_socket_fd() == events_[server_number_].data.fd)
      return true;
  return false;
}

void WebServer::accept_connection() {
  sockaddr_in address = {};
  socklen_t addrlen = sizeof(address);
  const int client_socket =
      accept(events_[server_number_].data.fd,
             reinterpret_cast<struct sockaddr *>(&address), &addrlen);
  if (client_socket == -1)
    log("Failed to accept the new client connection", warning);
  const int flags = fcntl(client_socket, F_GETFL, 0);
  if (flags == -1 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1)
    log("Failed to set server socket to non-blocking mode", warning);
  insert_epoll(client_socket);
  log("Connection accepted", info);
  servers_[server_number_].get_connected_clients().push_back(client_socket);
}

void WebServer::handle_connection() {
  char buffer[BUFFER_SIZE];
  const ssize_t bytes_received =
      recv(events_[server_number_].data.fd, buffer, BUFFER_SIZE, 0);
  if (!bytes_received || bytes_received == -1) {
    end_connection();
    std::vector<int>::iterator it =
        servers_[server_number_].get_connected_clients().begin();
    for (; it != servers_[server_number_].get_connected_clients().end(); ++it)
      if (*it == events_[server_number_].data.fd)
        break;
    servers_[server_number_].get_connected_clients().erase(it);
    log(!bytes_received ? "Client disconnected"
                        : "Failed to receive data from the client",
        info);
    return;
  }
  const std::string data_received(buffer);
  HttpHandler http_handler(data_received, events_[server_number_].data.fd,
                           servers_);
  http_handler.processRequest();
}

void WebServer::end_connection() const {
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, events_[server_number_].data.fd,
                NULL) == -1)
    log("Failed to remove file descriptor from epoll", warning);
  close(events_[server_number_].data.fd);
}

void WebServer::server_routine() {
  while (WebServer::is_running) {
    const int num_events = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 200);
    if (num_events == -1)
      log("epoll failed", error);
    for (server_number_ = 0; server_number_ < num_events; server_number_++) {
      if (events_[server_number_].events & EPOLLIN) {
        is_server_socket() ? accept_connection() : handle_connection();
      }
    }
  }
  // TODO: close all sockets
}

#pragma endregion