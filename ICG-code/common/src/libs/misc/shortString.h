// 
// Class ShortString 
//

#ifndef _SHORTSTRING_H_
#define _SHORTSTRING_H_

#pragma warning (disable: 4800 )

#define SHORT_STRING_MAX_LENGHT 64

// Include ---------------------------------------------------------------
#include "compilerOptions.h"
#include <string.h>

// Declaration -----------------------------------------------------------
// This is a very simple string class for fixed lenght ShortString typically
// useful to store identifiers for shaders.
// Each ShortString has a maxim length specified and compile type and no
// allocation is performed, i.e. every ShortString has storage for that size.
class ShortString {

// Static data ------------------------------------------------------

// Data -------------------------------------------------------------
protected:
    char           _data[SHORT_STRING_MAX_LENGHT];

public:

// Public methods ---------------------------------------------------
	ShortString();
	ShortString(const char* c);
	ShortString(const ShortString& c);

    operator const char*();
    operator char*();

    ShortString& operator =(const ShortString& s);
    ShortString& operator =(const char* s);

    bool operator ==(const ShortString& s) const;
    bool operator !=(const ShortString& s) const;
    bool operator < (const ShortString& s) const;

    ShortString& operator+= (const ShortString& s);
    ShortString  operator+ (const ShortString& s) const;

    char& operator[](int idx);

    int Compare(const ShortString& s) const;
};

// Inline functions ------------------------------------------------------

inline ShortString::ShortString() {
    _data[0] = 0;
}

inline ShortString::ShortString(const char* s) {
    strcpy(_data, s);
}

inline ShortString::ShortString(const ShortString& s) {
    strcpy(_data, s._data);
}

inline ShortString::operator const char*() {
    return _data;
}

inline ShortString::operator char*() {
    return _data;
}

inline ShortString& ShortString::operator =(const ShortString& s) {
    strcpy(_data, s._data);
    return *this;
}

inline ShortString& ShortString::operator =(const char* s) {
    strcpy(_data, s);
    return *this;
}

inline bool ShortString::operator ==(const ShortString& s) const {
    return !strcmp(_data, s._data);
}

inline bool ShortString::operator !=(const ShortString& s) const {
    return strcmp(_data, s._data);
}

inline bool ShortString::operator <(const ShortString& s) const {
    return strcmp(_data, s._data) < 0;
}

inline ShortString& ShortString::operator+= (const ShortString& s) {
    strcat(_data, s._data);
    return *this;
}

inline ShortString ShortString::operator+ (const ShortString& s) const {
    ShortString ret;
    ret = *this;
    ret.operator +=(s);
    return ret;
}

inline int ShortString::Compare(const ShortString& s) const {
    return strcmp(_data, s._data);
}

inline char& ShortString::operator[](int idx) {
    return _data[idx];
}

#endif // _SHORTSTRING_H_
