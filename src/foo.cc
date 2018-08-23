#include <stdio.h>
#include <algorithm>
extern "C" {
#include "util.h"
}

int main(int argc, char** argv){
  int foo[5] = { 1, 2, 2, 3, 4 };
  printf("%d\n", std::lower_bound(foo, foo + 5, 2) - foo);
  printf("%d\n", search(foo, 0, 5, 2));
  return 0;
}
