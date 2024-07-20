
#ifndef COMPLEXFIELD_H
#define COMPLEXFIELD_H

#include "FieldInterface.h"

#include <string>

template <class T>
class ComplexField : public FieldInterface<T>
{
public:
    ComplexField(std::string const& string, T (*initializer)(std::string))
        : _initialized(false)
        , _value(NULL)
        , _valueString(string)
        , _initializer(initializer)
    {
    }

    virtual T get()
    {
        if (!_initialized)
        {
            *_value = (_initializer)(_valueString); // TODO -> remove pointer, its causing segfault
            _initialized = true;
        }

        return *_value;
    }

private:
    bool _initialized;
    T* _value;
    std::string _valueString;
    T (*_initializer)(std::string);
};

#endif //COMPLEXFIELD_H
