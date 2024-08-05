
#ifndef STATICSINGLETON_H
#define STATICSINGLETON_H

#include <exception>

template <class T>
class StaticSingleton
{
public:
    static T& instance()
    {
        static T instance;

        return instance;
    }

    StaticSingleton(StaticSingleton<T> const&) { throw std::exception(); };
    void operator = (StaticSingleton<T> const&) { throw std::exception(); };

private:
    StaticSingleton() {}
};

#endif //STATICSINGLETON_H
