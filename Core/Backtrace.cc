#include "Core/Backtrace.h"
#include <execinfo.h>
#include <cxxabi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char **
getBacktrace(int &numFrames) {
  
  // Print Backtrace
  void *array[10];
  size_t size;
  char **strings;
  size_t i;
  char **returnStrings;
  
  size = backtrace (array, 10);
  numFrames = size;
  strings = backtrace_symbols (array, size);
  
  //printf ("Obtained %zd stack frames.\n", size);
  returnStrings = new char *[size]; 
  
  
  for (i = 0; i < size; i++) {
    int stat;
    char buffer[1024];
    memset(buffer,0,1024);
    char * startSpot = strchr(strings[i],'(');
    char * demangled = 0;
    if ( startSpot ) {
      char * endSpot = strchr(strings[i],'+');
      if ( endSpot ) {
	int numChars = endSpot - startSpot-1;
	strncpy(buffer, startSpot+1, numChars );
	demangled = abi::__cxa_demangle(buffer,0,0,&stat);
      }
    }
    
    if ( demangled ) {
      returnStrings[i] = new char[strlen(demangled)+1];
      strcpy(returnStrings[i], demangled);
    }
    else {
      returnStrings[i] = new char[strlen(strings[i])+1];
      strcpy(returnStrings[i], strings[i]);
    }
  }

  free(strings);

  return returnStrings;
}
