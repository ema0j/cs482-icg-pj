#ifndef _FAR_STDCOMMON_H_
#define _FAR_STDCOMMON_H_

#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <iostream>
#include <memory>
#include <functional>
#include <sstream>
#include <exception>
#ifdef WIN32
using std::tr1::shared_ptr;
using std::tr1::dynamic_pointer_cast;
using std::tr1::static_pointer_cast;
using std::tr1::weak_ptr;
using std::tr1::hash;
#else
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;
using std::weak_ptr;
using std::hash;
#endif

using std::vector;
using std::type_info;
using std::string;
using std::map;
using std::pair;
using std::set;
using std::list;
using std::stringstream;
using std::endl;
using std::cout;
using std::cin;
using std::cerr;
using std::flush;

#endif // _RESIZABLEARRAY_H_
