#include "http_handler.hpp"
#include "error_page_handler.hpp"
#include "utils.hpp"
#include "webserver.hpp"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

HttpHandler::HttpHandler(const string &request, Server &server,
                         Client &client, std::vector<Server> &servers)
    : server_(&server)
      , client_(&client)
{
    HttpParser parser(request);
    headers_ = parser.get_headers();
    server_ = find_server(servers, headers_, server);
    error_page_handler_ = ErrorPageHandler(*server_, headers_);
    (void) client_;
}

HttpHandler::~HttpHandler()
{
}

#pragma region HTTP Methods

/// @brief Process a HTTP request
/// @return The response created by the request
string HttpHandler::process_request()
{
    std::cout << "\n process_request()\n";

    if (is_cgi_script())
    {
        std::cout << "IS CGI\n";
        return process_cgi();
    }

    if (!is_method_allowed(headers_.at("method")))
    {
        std::cout << "methed not allowd\n";
        return error_page_handler_.get_error_page(405);
    }

    if (has_redirection(headers_.at("uri")))
    {
        std::cout << "has redirection\n";
        return create_redirection_response();
    }

    if (headers_.at("method") == "GET")
    {
        std::cout << "GETGETGETGET\n";
        return process_get();
    } else if (headers_.at("method") == "POST")
        return process_post();

    else if (headers_.at("method") == "DELETE")
        return process_delete();

    else if (IS_VALID_BUT_NOT_SUPPORTED(headers_.at("method")))
        return error_page_handler_.get_error_page(501);

    else
        return error_page_handler_.get_error_page(400);
}

Server *HttpHandler::find_server(std::vector<Server> &servers, std::map<string, string> headers, Server &defaultServer)
{
    std::cout << "\nfind_server()\n";

    std::string const &host = headers["Host"];

    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        std::vector<std::string> serverNames = it->get_server_name();
        for (std::vector<std::string>::iterator sn = serverNames.begin(); sn != serverNames.end(); sn++)
        {
            std::stringstream ss;

            ss << *sn << ":" << it->get_port();

            std::string composite = ss.str();

            std::cout << "\n ss.str() -> " << composite << "\n";

            if (host == ss.str())
            {
                std::cout << "returning server -> " << host << "\n";
                return &*it;
            }
            if (host == *sn)
            {
                std::cout << "returning server -> " << host << "\n";
                return &*it;
            }
        }
    }

    return &defaultServer;
}

/// @brief Process a CGI script
/// @return The response created by the CGI script
string HttpHandler::process_cgi()
{
    try
    {
        string cgi_script_path = get_file_path(headers_.at("uri"));
        if (access(cgi_script_path.c_str(), X_OK) == -1)
            return error_page_handler_.get_error_page(403);
        string extension =
                headers_.at("uri").substr(headers_.at("uri").find_last_of('.') + 1);
        char *args[3];
        if (extension == "py")
            args[0] = (char *) "python3";
        else if (extension == "php")
            args[0] = (char *) "php";
        args[1] = (char *) cgi_script_path.c_str();
        args[2] = NULL;
        char **envp = environ;
        int pipefd[2];
        if (pipe(pipefd) == -1)
            WebServer::log("Failed to create pipe", error);
        pid_t pid = fork();
        if (pid == -1)
            WebServer::log("Failed to fork", error);
        if (pid == 0)
        {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            if (execve(Utils::path_finder(args[0]).c_str(), args, envp) == -1)
                std::exit(EXIT_FAILURE);
        }
        // TODO handle timeout
        close(pipefd[1]);
        char buffer[1024];
        string cgi_output;
        ssize_t n;
        while ((n = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[n] = '\0';
            cgi_output += buffer;
        }
        close(pipefd[0]);
        if (cgi_output.empty())
            return error_page_handler_.get_error_page(502);
        string response =
                Utils::response_builder("200", "OK", "text/html", cgi_output.length());
        response += cgi_output;
        WebServer::log(response, debug);
        return response;
    } catch (const std::exception &e)
    {
        WebServer::log("Failed to process CGI script", debug);
        return error_page_handler_.get_error_page(500);
    }
}

/// @brief Process a GET request
string HttpHandler::process_get()
{
    std::fstream file;
    string content;
    string file_path;
    Stat buffer;

    std::cout << "\nGET\n";

    try
    {
        file_path = get_file_path(headers_.at("uri"));
        if (Utils::should_generate_autoindex(headers_.at("uri"), *server_))
        {
            file_path = server_->get_config().get_root() + headers_.at("uri");
            content = Utils::create_autoindex(file_path, headers_.at("uri"));
            if (content.empty())
                return error_page_handler_.get_error_page(404);
            string response =
                    Utils::response_builder("200", "OK", "text/html", content.length());
            WebServer::log(string(HTTP_200) + headers_.at("uri"), info);
            return response + content;
        }
        if (stat(file_path.c_str(), &buffer) != 0 || S_ISDIR(buffer.st_mode))
            throw std::runtime_error("404");
        content = Utils::read_file(file_path);
        string response = Utils::response_builder(
            "200", "OK", Utils::get_content_type(file_path), content.length());
        WebServer::log(string(HTTP_200) + headers_.at("uri"), info);
        return response + content;
    } catch (const std::runtime_error &e)
    {
        return error_page_handler_.get_error_page(404);
    }
}

/*
 *void			Response::postMethod(Request & request, RequestConfig & requestConf)
{
  ResponseHeader	head;

  if (requestConf.getCgiPass() != "")
  {
    CgiHandler	cgi(request, requestConf);
    size_t		i = 0;
    size_t		j = _response.size() - 2;

    _response = cgi.executeCgi(requestConf.getCgiPass());

    while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
    {
      std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
      if (str.find("Status: ") == 0)
        _code = std::atoi(str.substr(8, 3).c_str());
      else if (str.find("Content-Type: ") == 0)
        _type = str.substr(14, str.size());
      i += str.size() + 2;
    }
    while (_response.find("\r\n", j) == j)
      j -= 2;

    _response = _response.substr(i, j - i);
  }
  else
  {
    _code = 204;
    _response = "";
  }
  if (_code == 500)
    _response = this->readHtml(_errorMap[_code]);
  _response = head.getHeader(_response.size(), _path, _code, _type, requestConf.getContentLocation(), requestConf.getLang()) + "\r\n" + _response;
}
*/

/// @brief Process a POST request
string HttpHandler::process_post()
{
    size_t max_size = Utils::get_max_size(server_->get_config().get_max_client_body_size());

    if (headers_.at("body").size() > max_size)
    {
        return error_page_handler_.get_error_page(413);
    }

    string response = "Received" + headers_.at("body");

    return Utils::response_builder("201", "Created", "text/plain", response.length()) + response;
}

/// @brief Process a DELETE request
string HttpHandler::process_delete()
{
    string file_path;
    Stat buffer;

    file_path = server_->get_config().get_root() + headers_.at("uri");
    if (stat(file_path.c_str(), &buffer) != 0)
        return error_page_handler_.get_error_page(404);
    if (!(buffer.st_mode & S_IWOTH))
        return error_page_handler_.get_error_page(403);
    if (std::remove(file_path.c_str()))
        return error_page_handler_.get_error_page(403);
    return Utils::response_builder("204", "No Content", "text/plain", 0);
}

#pragma endregion

#pragma region Utility Functions

/// @brief Check if the request is a CGI script
/// @return True if the request is a CGI script, false otherwise
bool HttpHandler::is_cgi_script()
{
    std::cout << "is_cgi_script()\n";

    try
    {
        string extension = headers_.at("uri").substr(headers_.at("uri").find_last_of('.') + 1);

        if (extension == "py" || extension == "php")
            return true;
    } catch (std::exception &e)
    {
        (void) e;
    }

    std::cout << "is_cgi_script() end \n";

    return false;
}

/// @brief Check if the request has a redirection
/// @param uri The URI of the request
/// @return True if the request has a redirection, false otherwise
bool HttpHandler::has_redirection(const string &uri)
{
    std::vector<Location> locations = server_->get_locations();
    for (size_t i = 0; i < locations.size(); i++)
    {
        if (uri == locations.at(i).get_path() &&
            !locations.at(i).get_config().get_redirection().empty())
            return true;
    }
    if (!server_->get_config().get_redirection().empty())
        return true;
    return false;
}

/// @brief Create a redirection response
/// @return The redirection response
string HttpHandler::create_redirection_response()
{
    enum redirectionType type;
    string response;
    string redirection;

    redirection = server_->get_config().get_redirection();
    for (size_t i = 0; i < server_->get_locations().size(); i++)
    {
        if (headers_.at("uri") == server_->get_locations().at(i).get_path())
        {
            redirection =
                    server_->get_locations().at(i).get_config().get_redirection();
            break;
        }
    }
    if (redirection.substr(redirection.find_last_of(' ') + 1) == "redirect")
        type = REDIRECT_307;
    else
        type = REDIRECT_308;
    if (type == REDIRECT_307)
        response = "HTTP/1.1 307 Temporary Redirect\r\n";
    else
        response = "HTTP/1.1 308 Permanent Redirect\r\n";
    response +=
            "Location: " + redirection.substr(0, redirection.find_last_of(' ')) +
            "\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
    WebServer::log(string(type == REDIRECT_307 ? HTTP_307 : HTTP_308) +
                   headers_.at("uri"),
                   info);
    return response;
}

/// @brief Check if the method is allowed for the location
/// @param method The method to check
/// @return True if the method is allowed, false otherwise
bool HttpHandler::is_method_allowed(const string &method)
{
    string location =
            headers_.at("uri").substr(0, headers_.at("uri").find_last_of('/'));
    std::vector<Location> locations = server_->get_locations();
    for (size_t i = 0; i < locations.size(); i++)
    {
        if (location == locations.at(i).get_path())
        {
            for (size_t j = 0; j < locations.at(i).get_allowed_methods().size();
                 j++)
            {
                if (method == locations.at(i).get_allowed_methods().at(j))
                    return true;
            }
            return false;
        }
    }
    return true;
}

/// @brief Check if the server should generate an autoindex page
/// @param uri The URI of the request
/// @param server A reference to the server
/// @return True if the server should generate an autoindex page, false
/// otherwise
bool should_generate_autoindex(const string &uri, Server &server)
{
    if (uri == "/" && server.get_config().get_auto_index())
        return true;
    else if (*(uri.end() - 1) == '/')
    {
        std::vector<Location> locations = server.get_locations();
        if (locations.empty() && server.get_config().get_auto_index())
            return true;
        for (size_t i = 0; i < locations.size(); i++)
        {
            if (uri == locations.at(i).get_path() &&
                locations.at(i).get_config().get_auto_index())
                return true;
        }
    }
    return false;
}

/// @brief Get the path of the location
/// @param uri The URI of the request
/// @return The path of the location
string HttpHandler::get_location_path(const string &uri)
{
    string path;
    std::vector<Location> locations = server_->get_locations();
    Stat buffer;

    for (size_t i = 0; i < locations.size(); i++)
    {
        if (uri == locations.at(i).get_path())
        {
            std::vector<string> indexes =
                    locations.at(i).get_config().get_indexes();
            for (size_t j = 0; j < indexes.size(); j++)
            {
                path = server_->get_config().get_root() +
                       locations.at(i).get_config().get_root() + "/" + indexes.at(j);
                if (stat(path.c_str(), &buffer) == 0)
                    return path;
            }
        } else
        {
            path = server_->get_config().get_root() + uri;
            if (stat(path.c_str(), &buffer) == 0)
                break;
        }
    }
    return path;
}

/// @brief Get the path of the requested file
/// @param uri The URI of the request
/// @return The path of the requested file
string HttpHandler::get_file_path(const string &uri)
{
    string file_path;
    Stat buffer;

    if (uri == "/")
    {
        for (size_t i = 0; i < server_->get_config().get_indexes().size(); i++)
        {
            file_path = server_->get_config().get_root() + "/" +
                        server_->get_config().get_indexes().at(i);
            if (stat(file_path.c_str(), &buffer) == 0)
                break;
        }
    } else
    {
        if (server_->get_locations().empty())
            file_path = server_->get_config().get_root() + uri;
        else
            file_path = get_location_path(uri);
    }
    return file_path;
}

#pragma endregion
