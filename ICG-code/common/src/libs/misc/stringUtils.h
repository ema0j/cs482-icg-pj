// 
// Class ResizableArray 
//

#ifndef _STRINGUTILS_H_
#define _STRINGUTILS_H_

// Include ---------------------------------------------------------------

#include <string>
using std::string;
#include <sstream>

template<class T>
string toString(const T& v) {
    std::stringstream stream;
    stream << v;
    return stream.str();
}

#endif // _RESIZABLEARRAY_H_
