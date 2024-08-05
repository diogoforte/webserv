
#ifndef SERVERCONF_H
#define SERVERCONF_H

#include "../singletons/BytesUnitMapSingleton.h"

#include "ComplexField.h"
#include "ConfigurationNode.h"
#include "LocationConf.h"
#include "SimpleField.h"

#include <cstdlib>
#include <stdint.h>
#include <list>
#include <map>
#include <string>

#include <iostream>

class ServerConf : public ConfigurationNode
{
public:
    ServerConf();

    virtual void init(std::string& parentContent);

    SimpleField listen;
    SimpleField serverName;
    SimpleField root;
    ComplexField<std::list<std::string> > index;
    ComplexField<std::list<std::string> > errorPage;
    ComplexField<uint64_t> maxClientBodySize;
    ComplexField<bool> autoIndex;

    std::list<LocationConf> locationsConfigsList;

private:
    virtual void actuallyInit();

    void initLocations();
};


#endif //SERVERCONF_H
