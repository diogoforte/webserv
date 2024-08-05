
#include "Config.h"

Config::Config()
{
}

void Config::init(std::string& parentContent)
{
    _content = parentContent;

    actuallyInit();
}

void Config::actuallyInit()
{
    while (childExists("server"))
    {
        ServerConf conf;

        conf.init(_content);
        serversConfigsList.push_back(conf);
    }
}