
#ifndef TYPECONVERTER_H
#define TYPECONVERTER_H

#include "../singletons/BytesUnitMapSingleton.h"

#include <cstdlib>
#include <list>
#include <stdint.h>
#include <string>

class TypeConverter
{
public:
    static bool stringToBoolean(std::string toConvert)
    {
        return toConvert == "on";
    }

    static uint64_t stringtoUnsingedLong(std::string toConvert)
    {
        std::string const numericString = toConvert.substr(0, toConvert.size());
        std::string const unityString = toConvert.substr(toConvert.size() - 1);

        uint64_t result = strtol(numericString.c_str(), NULL, 10);
        result *= BytesUnitMapSingleton::instance().map(*unityString.c_str());

        return result;
    }

    static std::list<std::string> stringToStringList(std::string toConvert)
    {
        std::list<std::string> strings;

        std::string currentString = toConvert;
        bool isLast;

        do
        {
            std::string::size_type const endPos = currentString.find(' ');

            strings.push_back(currentString.substr(0, endPos));
            isLast = endPos == std::string::npos;
            currentString = currentString.substr(endPos + 1);

        } while (!isLast);

        return strings;
    }
};

#endif //TYPECONVERTER_H
