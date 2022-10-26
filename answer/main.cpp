#include <stdio.h>
#include "Map.h"

int main() {
  //Create a map called foo which has a key type of const char*
  const char* foo = CREATE_MAP(char*);
  // Put a value in the map
  char* key ="test";
  char* value = "testValue";
  PUT_IN_MAP(char*, foo, key, (void*)value);
  return 0;
}