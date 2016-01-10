// 
// Class Properties 
//
// Utility class that allow to save an ASCII file with properties
// which are a set of names with associated values.
// It does not verify the validity of the input!
// It has not been coded for speed, but for easy of coding
// (interface won't change for a better implementation).
//
// File format
// % for comments
// name: value with at least one space between :,var and no spaces
// between name,:
//
// A property file rewritten on disk will not maintain the comments
// available on the loded file using the standard store implementation.
//
// Default can be stored before loading by simply setting the values 
// in the variable. This means that default values will not be
// maintaned after a reading.

#ifndef _PROPERTIES_H_
#define _PROPERTIES_H_

#define MAX_PROPERTIES 1024

// Include ---------------------------------------------------------------
#include "compilerOptions.h"
#include <stdio.h>
#include <assert.h>

// Declaration -----------------------------------------------------------

enum PropertiesType {
  PROPERTIES_TYPE_INT,        // integer
  PROPERTIES_TYPE_FLOAT,      // float
  PROPERTIES_TYPE_BOOL,       // bool
  PROPERTIES_TYPE_STRING,     // string (char*)
  PROPERTIES_TYPE_TUPLE3F,    // array of 3 floats separated by spaces
};

class Properties {

// Static data ------------------------------------------------------

// Data -------------------------------------------------------------
protected:
  int                         nProperties;
  PropertiesType              type[MAX_PROPERTIES];
  char*                       name[MAX_PROPERTIES];
  void*                       var[MAX_PROPERTIES];

  char                        commentLineStarter;

// Public methods ---------------------------------------------------
public:
  Properties();

  // Add the variable pointed by var of type type and named name
  // to the list of parsable variables
  void add(char* name, PropertiesType type, void* var);

  // Load the properties from a text file (false if something
  // goes wrong)
  bool load(const char filename[]);

  // Store the properties to a text file (false if something
  // goes wrong)
  bool store(const char filename[], const char title[]);

  // Print the properties to a file (or stdio)
  bool print(FILE* file, const char title[]);

// Private methods --------------------------------------------------

};

// Inline functions ------------------------------------------------------

inline Properties::Properties() {
  commentLineStarter = '%';
  nProperties = 0;
}

inline void Properties::add(char* name, PropertiesType type, void* var) {
  assert(nProperties < MAX_PROPERTIES);

  this->name[nProperties] = name;
  this->type[nProperties] = type;
  this->var[nProperties] = var;
  nProperties ++;
}

#endif // _PROPERTIES_H_
