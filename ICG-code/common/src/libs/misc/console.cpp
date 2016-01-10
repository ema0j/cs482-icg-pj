#include "console.h"
#include <iostream>
using std::cerr;
#include "assert.h"

void Console::Assert(const string& msg, bool cond) {
    if(cond) return;
    cerr << msg;
    assert(0);
}

void Console::NotImplemented() {
    Assert("not implemented", false);
}
