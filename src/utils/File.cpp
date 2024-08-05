
#include "File.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

File::File(std::string const& filePath)
    : _stream(filePath.c_str())
    , _initialized(false)
{
    if (!_stream.is_open())
    {
        std::cout << "Could not open file -> " << filePath << "\n";
        abort();
    }
}

std::string File::getContent()
{
    if (!_initialized)
    {
        std::ostringstream oss;
        oss << _stream.rdbuf();

        _content = oss.str();
    }

    return _content;
}