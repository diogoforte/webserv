
#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "ServerConf.h"

#include <cstdlib>
#include <iostream>
#include <list>
#include <stdint.h>
#include <string>

#define KEY_FOUND(position) ((position) != std::string::npos)

class ConfigParser
{
public:

    static std::string getVariable(std::string const& key, std::string const& serverConfigurationString)
    {
        std::string::size_type keyInstancePosition = serverConfigurationString.find(key);
        std::string variableString;

        if (KEY_FOUND(keyInstancePosition))
        {
            std::string::size_type variableInstancePosition = keyInstancePosition + key.size() + 1;

            variableString = treatString(variableInstancePosition, serverConfigurationString);
        }
        else
        {
            // TODO-> replace for something like a Logger::log()
            std::cout << " [FATAL] : Could not get key -> " << key << " from configuration, aborting\n";
            abort();
        }

        return variableString;
    }


    static std::string treatString(std::string::size_type const position, std::string const& serverConfigurationString)
    {
        std::string::size_type endPosition = getEndOfVariable(position, serverConfigurationString);

        return serverConfigurationString.substr(position, endPosition);
    }

    static std::string::size_type getEndOfVariable(std::string::size_type const start, std::string const& serverConfigurationString)
    {
        std::string const substring = serverConfigurationString.substr(start);

        return substring.find(';');
    }
};


#endif
