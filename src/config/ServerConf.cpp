
#include "ServerConf.h"

#include "ConfigParser.h"

#define BIND(key) (ConfigParser::getVariable(key, src))


ServerConf::ServerConf(std::string const& src)
    : listen(BIND("listen"))
    , serverName(BIND("server_name"))
    , root(BIND("root"))
    , index(BIND("index"), ServerConf::toStringList)
    , errorPage(BIND("error_page"), ServerConf::toStringList)
    , maxClientBodySize(BIND("max_client_body_size"), ServerConf::toInteger)
    , autoIndex(BIND("autoindex"), ServerConf::toBoolean)
{
}