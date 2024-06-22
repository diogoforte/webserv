#include <arpa/inet.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

struct thread_data {
  int id;
  int socket;
  int requests;
  pthread_mutex_t *mutex;
  sockaddr_in server_addr;
};

std::string server_ip = "127.0.0.1";
int port = 8080;
int exit_flag = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

std::string itoa(int value) {
  pthread_mutex_lock(&mutex);
  std::stringstream ss;
  ss << value;
  pthread_mutex_unlock(&mutex);
  return ss.str();
}

void *print(const std::string &message) {
  pthread_mutex_lock(&mutex);
  std::cout << message << std::endl;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void *routine(void *arg) {
  thread_data *data = static_cast<thread_data *>(arg);
  data->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (data->socket == -1)
    return print("Failed to create socket");
  int opt = 1;
  if (setsockopt(data->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1)
    return print("Failed to set socket option SO_REUSEADDR");
  if (connect(data->socket, (struct sockaddr *)&data->server_addr,
              sizeof(data->server_addr)) == -1)
    return print("Connection failed");
  print(itoa(data->id) + " = (" + itoa(data->socket) + ")");
  while (data->requests--) {
    pthread_mutex_lock(&mutex);
    if (exit_flag) {
      pthread_mutex_unlock(&mutex);
      break;
    }
    pthread_mutex_unlock(&mutex);
    std::string message = "GET /index.html HTTP/1.1\r\n\r\n";
    if (send(data->socket, message.c_str(), message.size(), 0) < 0)
      return print("Send failed");
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    if (recv(data->socket, buffer, sizeof(buffer), 0) < 0)
      return print("Receive failed");
  }
  close(data->socket);
  return NULL;
}

void signal_handler(int signal) {
  (void)signal;
  pthread_mutex_lock(&mutex);
  exit_flag = 1;
  std::cout << "Exiting through SIGINT" << std::endl;
  pthread_mutex_unlock(&mutex);
  std::exit(0);
}

int main(int ac, char **av) {
  if (ac != 3) {
    std::cerr << "Usage: " << av[0]
              << " <number of clients> <number of requests>" << std::endl;
    return 1;
  }
  signal(SIGINT, signal_handler);
  const int clients = std::atoi(av[1]);
  const int requests = std::atoi(av[2]);
  timeval total_start, total_end;
  gettimeofday(&total_start, NULL);
  thread_data data[clients];
  pthread_t thread[clients];
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
  server_addr.sin_port = htons(port);
  for (int i = 0; i < clients; i++) {
    data[i].id = i;
    data[i].socket = -1;
    data[i].requests = requests;
    data[i].server_addr = server_addr;
    if (pthread_create(&thread[i], NULL, routine, (void *)&data[i])) {
      std::cerr << "Could not create thread" << std::endl;
      return 1;
    }
  }
  for (int i = 0; i < clients; i++)
    pthread_join(thread[i], NULL);
  gettimeofday(&total_end, NULL);
  double total_elapsed = (total_end.tv_sec - total_start.tv_sec) +
                         (total_end.tv_usec - total_start.tv_usec) / 1000000.0;
  std::cout << "Total time for all requests: " << total_elapsed << " s"
            << std::endl;
}