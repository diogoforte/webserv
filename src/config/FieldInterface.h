//
// Created by abarbosa on 20-07-2024.
//

#ifndef FIELD_H
#define FIELD_H

template <class T>
class FieldInterface
{
public:
    virtual T get() = 0;
};

#endif //FIELD_H
