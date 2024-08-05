
#include "LocationConf.h"

#include "ConfigParser.h"

LocationConf::LocationConf()
    : acceptedHttpMethods(&TypeConverter::stringToStringList)
    , redirection(&TypeConverter::stringtoUnsingedLong)
    , directoryListing(&TypeConverter::stringToBoolean)
{

}

void LocationConf::init(std::string& parentContent)
{
    location.setSourceString(ConfigParser::getVariable("location", parentContent, true));

    std::string::size_type startPosition = setContent(parentContent, "location");
    std::string::size_type endPosition = cutContentsEnd();

    parentContent.erase(startPosition, endPosition - startPosition);

    actuallyInit();
}

void LocationConf::actuallyInit()
{
    bind<std::list<std::string> >("accepted_http_methods", &acceptedHttpMethods, false);
    bind<uint64_t>("return", &redirection, false);
    bind<std::string>("root", &root, false);
    bind<bool>("directory_listing", &directoryListing, false);
    bind<std::string>("index", &index, false);
}