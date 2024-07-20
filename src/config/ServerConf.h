
#ifndef SERVERCONF_H
#define SERVERCONF_H

#include "ComplexField.h"
#include "SimpleField.h"

#include <stdint.h>
#include <list>
#include <map>
#include <string>

#include <iostream> // TODO -> remove

class ServerConf
{
public:
    ServerConf(std::string const& src);

    SimpleField listen;
    SimpleField serverName;
    SimpleField root;
    ComplexField<std::list<std::string> > index;
    ComplexField<std::list<std::string> > errorPage;
    ComplexField<uint64_t> maxClientBodySize;
    ComplexField<bool> autoIndex;

//private:
    static std::list<std::string> toStringList(std::string indexString)
    {
        std::list<std::string> strings;
        bool isLast = false;

        do
        {
            std::string::size_type endPos = indexString.find(' ');


            strings.push_back(indexString.substr(0, endPos));

            std::cout << "Inserted " << indexString.substr(0, endPos);
            isLast = endPos == std::string::npos;
        }
        while(!isLast);

        return strings;
    }

    static uint64_t toInteger(std::string indexString)
    {
        (void)indexString;

        return 1;
    }

    static bool toBoolean(std::string indexString)
    {
        (void)indexString;

        return false;
    }
};


#endif //SERVERCONF_H
