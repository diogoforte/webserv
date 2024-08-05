
#ifndef FIELD_H
#define FIELD_H

#include <string>

template <class T>
class FieldInterface
{
public:
    virtual T get() = 0;
    virtual void setSourceString(std::string const& sourceString) = 0;
};

#endif
