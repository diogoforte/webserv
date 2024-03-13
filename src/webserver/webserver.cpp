#include "webserver.hpp"
#include <parser/AParserFunctions.hpp>
#include <iostream>


std::vector<server> &WebServer::get_servers() { return servers; }


std::ofstream WebServer::log_file_;

WebServer::WebServer() {
    log_file_.open("logs/webserv.log", std::ios::out | std::ios::trunc);
    if (!log_file_)
        throw std::runtime_error("Failed to open log file");
    log("WebServer started", info);
}

WebServer::~WebServer() {
    log_file_.close();
}