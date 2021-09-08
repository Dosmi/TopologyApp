#ifndef TUPLE_H
#define TUPLE_H

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

// This is eugene going a little nuts with C++
template <typename TYPE, unsigned SIZE> class tuple
{
public:

    tuple() { }

    tuple(TYPE first, ...) // tuple<int, 3> (int one, ...)
    {
        data[ 0 ] = first;
        // FIRST, we create a POINTER that will be used
        // to point to the first element of the VARIABLE
        // ARGUMENT LIST:
        va_list args;

        // However currently, args is UNINITIALIZED,
        // SO, now we make args point to
        // the first argument in the list
        va_start( args, first );
        // Notice that first is the LAST MANDATORY ARGUMENT
        // that the tuple() function takes.
        // By "LAST MANDATORY ARGUMENT", I mean 'first'
        // is the last argument to the tuple() function
        // JUST BEFORE the "..."

        for( unsigned i = 1; i < SIZE; ++i )
        {
            data[ i ] = va_arg( args, TYPE );
        }
        // FINALLY, we clean up by saying
        // va_end().  Don't forget to do this
        // BEFORE the tuple() function returns!
        va_end( args );
    }

    tuple(const TYPE array[SIZE]) {
        memcpy(data, array, SIZE * sizeof(TYPE));
    }

    tuple(const tuple& other) {
        memcpy(this->data, &other.data, SIZE * sizeof(TYPE));
    }

    tuple& operator=(const tuple& other) {
        if (&other != this)
            memcpy(this->data, &other.data, SIZE * sizeof(TYPE));
        return *this;
    }

    tuple& operator=(const TYPE array[SIZE]) {
        memcpy(data, array, SIZE * sizeof(TYPE));
        return *this;
    }

    inline TYPE& operator[](unsigned i) {
        return data[i];
    }

    inline const TYPE& operator[](unsigned i) const {
        return data[i];
    }

private:
    TYPE data[SIZE];

};

template <typename TYPE, unsigned SIZE>
std::ostream& operator<<(std::ostream &out, const tuple<TYPE,SIZE> x)
{
    out << '[';
    for (unsigned i=0; i<SIZE; i++)
        out << ' ' << x[i];
    return out << " ]" <<  std::flush;
}

#endif
