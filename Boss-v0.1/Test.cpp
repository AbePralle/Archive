#include <cstdio>
using namespace std;

#include "Source/CPP/Boss.h"
using namespace Boss;

int main()
{
  VM vm;

  if (vm.load( "Test.boss" )) vm.execute();
  vm.collect_garbage();

  return 0;
}
