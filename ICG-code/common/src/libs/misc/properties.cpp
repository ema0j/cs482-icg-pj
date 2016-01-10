// 
// Class Properties 
//

// Include ---------------------------------------------------------------

#include "properties.h"

#include <stdio.h>
#include <string.h>
//#include "ConsoleIO.h"

// Function implementation -----------------------------------------------

bool Properties::load(const char filename[]) {
  char buffer[1024];
  char varName[1024];
  char boolBuffer[1024];

  // open the file
  FILE* file = fopen(filename, "rt");
  if(!file) {
    printf("Cannot read from file %s\n", filename);
    return false;
  }

  // while not done
  while(fgets(buffer, 1024, file)) {
    // if it is a comment or empty, skip
    if(strlen(buffer) == 0) {
      continue;
    }
    if(buffer[0] == '%') {
      continue;
    }
    
    // get the variable name with semicolon
    sscanf(buffer, "%s", varName);
    if(!strlen(varName)) {
      // Rpobably an empty line
      continue;
    }
    if(varName[strlen(varName)-1] == ':') {
      // Remove the ':'
      varName[strlen(varName)-1] = '\0';
    }

    // find the index
    int idx;
    for(idx = 0; idx < nProperties; idx ++) {
      if(!strcmp(varName, name[idx])) {
        break;
      }
    }
    if(idx >= nProperties) {
      printf("Unknown property name: %s\n", varName);
      return false;
    }

    // parse the value
    switch(type[idx]) {
      case PROPERTIES_TYPE_INT:
        if(sscanf(buffer, "%s %i", varName, (int*)(var[idx])) != 2) {
          printf("Wrong parameter list in %s\n", buffer);
          return false;
        }
        break;
      case PROPERTIES_TYPE_FLOAT:
        if(sscanf(buffer, "%s %f", varName, (float*)(var[idx])) != 2) {
          printf("Wrong parameter list in %s\n", buffer);
          return false;
        }
        break;
      case PROPERTIES_TYPE_BOOL:
        if(sscanf(buffer, "%s %s", varName, boolBuffer) != 2) {
          printf("Wrong parameter list in %s\n", buffer);
          return false;
        }
        if(!strcmp(boolBuffer, "true") || 
          !strcmp(boolBuffer, "TRUE")) {
          *(bool*)(var[idx]) = true;
        } else if(!strcmp(boolBuffer, "false") || 
          !strcmp(boolBuffer, "FALSE")) {
          *(bool*)(var[idx]) = false;
        } else {
          printf("Wrong parameter list in %s\n", buffer);
          return false;
        }
        break;
      case PROPERTIES_TYPE_STRING:
        if(sscanf(buffer, "%s %s", varName, (char*)(var[idx])) != 2) {
          printf("Wrong parameter list in %s\n", buffer);
          return false;
        }
        break;
      case PROPERTIES_TYPE_TUPLE3F:
        if(sscanf(buffer, "%s %f %f %f", varName, 
          &(((float*)(var[idx]))[0]),
          &(((float*)(var[idx]))[1]),
          &(((float*)(var[idx]))[2])) != 4) {
          printf("Wrong parameter list in %s\n", buffer);
          return false;
        }
        break;
      default:
        printf("Unknown properties type\n");
        return false;
    }
  }

  fclose(file);

  return true;
}

bool Properties::store(const char filename[], const char title[]) {
  // open the file
  FILE* file = fopen(filename, "wt");
  if(!file) {
    printf("Cannot write to file %s\n", filename);
    return false;
  }

  bool ret = print(file, title);

  fclose(file);

  return ret;
}


bool Properties::print(FILE* file, const char title[]) {
  char buffer[1024];

  // store title
  sprintf(buffer, "%% %s", title);
  fprintf(file, "%s\n", buffer);
  sprintf(buffer, "%%");
  fprintf(file, "%s\n", buffer);

  // for each value
  for(int i = 0; i < nProperties; i ++) {
    // store the value
    switch(type[i]) {
      case PROPERTIES_TYPE_INT:
        sprintf(buffer, "%s: %i", name[i], 
          *(int*)(var[i]));        
        break;
      case PROPERTIES_TYPE_FLOAT:
        sprintf(buffer, "%s: %f", name[i], 
          *(float*)(var[i]));        
        break;
      case PROPERTIES_TYPE_BOOL:
        sprintf(buffer, "%s: %s", name[i], 
          ((*(bool*)(var[i]))?"true":"false") );        
        break;
      case PROPERTIES_TYPE_STRING:
        sprintf(buffer, "%s: %s", name[i], 
          (char*)(var[i]));        
        break;
      case PROPERTIES_TYPE_TUPLE3F:
        sprintf(buffer, "%s: %f %f %f", name[i], 
          ((float*)(var[i]))[0],
          ((float*)(var[i]))[1],
          ((float*)(var[i]))[2]);        
        break;
      default:
        printf("Unknown properties type\n");
        return false;
    }
    fprintf(file, "%s\n", buffer);
  }

  return true;
}