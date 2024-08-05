
#ifndef COMPLEXFIELD_H
#define COMPLEXFIELD_H

#include "FieldInterface.h"

#include <string>

template <class T>
class ComplexField : public FieldInterface<T>
{
public:
    ComplexField(T (*initializer)(std::string))
    : _value(NULL)
    , _initializer(initializer)
    , _initialized(false)
    {
    }

    ComplexField(std::string const& string, T (*initializer)(std::string))
        : _valueString(string)
        , _initialized(true)
        , _initializer(NULL)
    {
        _value = new T((*initializer)(string));
    }

    virtual T get()
    {
        if (!_initialized)
        {
            _value = new T((*_initializer)(_valueString));
            _initialized = true;
        }
        return *_value;
    }

    virtual void setSourceString(std::string const& stringSource)
    {
        _valueString = stringSource;
    }

    ~ComplexField()
    {
        if (_initialized && _value)
        {
            delete _value;
        }
    }

private:
    std::string _valueString;
    T* _value;
    T (*_initializer)(std::string);
    bool _initialized;
};

#endif
