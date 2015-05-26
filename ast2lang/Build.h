#ifndef BUILD_H_INCLUDED
#define BUILD_H_INCLUDED

#include "../Exception.h"
using namespace std::literals::string_literals;
exception_data* EXPN = nullptr;

#include <assert.h>
#include <math.h>
#include <iostream>
#include <locale>
#include <vector>
#include <algorithm>

template <typename T>
T __zero__ ()
{
    T res;
    return res;
}
#define ZERO(T) (__zero__<T> ())

std::string& CreateStringOnFail (const char* text, ...);

#include "../Errors.h"
#include "../Support.cpp"
#include "Tree.cpp"
#include "TreeLoader.cpp"
#include "Translator.cpp"


#endif // BUILD_H_INCLUDED
