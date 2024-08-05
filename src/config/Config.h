
#ifndef CONFIG_H
#define CONFIG_H

#include "ServerConf.h"

#include <list>
#include <string>

class Config : public ConfigurationNode
{
public:
    Config();

    virtual void init(std::string& parentContent);

    std::list<ServerConf> serversConfigsList;

private:
    virtual void actuallyInit();
};


#endif