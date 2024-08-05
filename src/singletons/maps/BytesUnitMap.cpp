
#include "BytesUnitMap.h"

BytesUnitMap::BytesUnitMap()
{
    _unityToMultiplierMap.insert(std::make_pair<char, uint64_t>('b', 1));
    _unityToMultiplierMap.insert(std::make_pair<char, uint64_t>('k', 1000));
    _unityToMultiplierMap.insert(std::make_pair<char, uint64_t>('m', 1000000));
    _unityToMultiplierMap.insert(std::make_pair<char, uint64_t>('g', 1000000000));
}

uint64_t BytesUnitMap::map(char c)
{
    return _unityToMultiplierMap[c];
}