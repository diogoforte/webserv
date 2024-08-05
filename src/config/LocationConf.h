
#ifndef LOCATIONCONF_H
#define LOCATIONCONF_H

#include "ComplexField.h"
#include "ConfigurationNode.h"
#include "SimpleField.h"

#include <stdint.h>
#include <list>
#include <string>

class LocationConf : public ConfigurationNode
{
public:
    LocationConf();

    virtual void init(std::string& parentContent);

    SimpleField location;

    ComplexField<std::list<std::string> > acceptedHttpMethods;
    ComplexField<uint64_t> redirection;
    SimpleField root;
    ComplexField<bool> directoryListing;
    SimpleField index;

private:
    virtual void actuallyInit();
};

#endif
