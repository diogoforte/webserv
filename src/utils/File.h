
#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <string>

class File
{
public:
    File(std::string const& filePath);

    std::string getContent();

private:
    std::ifstream _stream;
    bool _initialized;
    std::string _content;
};


#endif
