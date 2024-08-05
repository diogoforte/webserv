
#include "ServerConf.h"

ServerConf::ServerConf()
    : index(&TypeConverter::stringToStringList)
    , errorPage(&TypeConverter::stringToStringList)
    , maxClientBodySize(&TypeConverter::stringtoUnsingedLong)
    , autoIndex(&TypeConverter::stringToBoolean)
{
}

void ServerConf::init(std::string &parentContent)
{
    std::string::size_type startPosition = setContent(parentContent, "server");
    std::string::size_type endPosition = cutContentsEnd();

    parentContent.erase(startPosition, endPosition - startPosition);

    actuallyInit();
}

void ServerConf::actuallyInit()
{
    bind<std::string>("listen", &listen, true);
    bind<std::string>("server_name", &serverName, true);
    bind<std::string>("root", &root, true);
    bind<std::list<std::string> >("index", &index, true);
    bind<std::list<std::string> >("error_page", &errorPage, true);
    bind<uint64_t>("max_client_body_size", &maxClientBodySize, true);
    bind<bool>("autoindex", &autoIndex, true);

    initLocations();
}

void ServerConf::initLocations()
{
    while (childExists("location"))
    {
        LocationConf conf;

        conf.init(_content);
        locationsConfigsList.push_back(conf);
    }
}
