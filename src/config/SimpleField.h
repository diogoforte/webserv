
#ifndef SIMPLEFIELD_H
#define SIMPLEFIELD_H

#include "FieldInterface.h"

#include <string>

class SimpleField : public FieldInterface<std::string>
{
public:
    SimpleField()
        : _initialized(false)
    {
    }

    SimpleField(std::string const& value)
        : _value(value)
        , _initialized(true)
    {
    }

    virtual std::string get()
    {
        return _value;
    }

    virtual void setSourceString(std::string const& sourceString)
    {
        _initialized = true;
        _value = sourceString;
    }

private:
    std::string _value;
    bool _initialized;
};


#endif
