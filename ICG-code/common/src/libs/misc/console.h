#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "stdcommon.h"

class Console {
public:
    static void Assert(const string& msg, bool cond);
    static void NotImplemented();
};

#endif

