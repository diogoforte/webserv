#include <fstream>
#include <iostream>
#include <webserver.hpp>

/**
 * @brief Logs a message with a specific log level.
 *
 * This method logs a message with a specific log level. The message is output
 * to the console in a specific color depending on the log level and is also
 * written to the log file. If the log level is ERROR, a runtime error is
 * thrown.
 *
 * @param message The message to log.
 * @param log_level The log level of the message (INFO, WARNING, ERROR).
 */
void WebServer::log(const std::string &message, const int log_level) {
  std::string log_message;
  std::string color_message;
  switch (log_level) {
  case debug:
    log_message = "[DEBUG]\t\t" + message;
    color_message = PURPLE + log_message + RESET;
    break;
  case info:
    log_message = "[INFO]\t\t" + message;
    color_message = BLUE + log_message + RESET;
    break;
  case warning:
    log_message = "[WARNING]\t" + message;
    color_message = YELLOW + log_message + RESET;
    break;
  case error:
    log_message = "[ERROR]\t\t" + message;
    color_message = RED + log_message + RESET;
    break;
  default:
    log_message = "[ERROR]\tInvalid log level passed";
    color_message = RED + log_message + RESET;
    break;
  }
  std::cout << color_message << std::endl;
  log_file_ << log_message << std::endl;
  if (log_level == error)
    throw std::runtime_error(log_message);
}