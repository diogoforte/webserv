
#ifndef SIMPLEFIELD_H
#define SIMPLEFIELD_H

#include "FieldInterface.h"

#include <string>

class SimpleField : public FieldInterface<std::string>
{
public:
    SimpleField(std::string const& value)
        : _value(value)
    {
    }

    virtual std::string get()
    {
        return _value;
    }

private:
    std::string _value;
};


#endif //SIMPLEFIELD_H
