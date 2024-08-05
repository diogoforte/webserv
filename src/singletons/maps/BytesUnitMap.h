
#ifndef BYTESUNITMAP_H
#define BYTESUNITMAP_H

#include "../StaticSingleton.h"

#include <stdint.h>

#include <map>

class BytesUnitMap
{
public:
    BytesUnitMap();

    uint64_t map(char c);

private:
    std::map<char, uint64_t> _unityToMultiplierMap;

};


#endif //BYTESUNITMAP_H
