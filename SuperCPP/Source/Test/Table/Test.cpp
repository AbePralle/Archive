#include <stdio.h>
#include "SuperCPPStringTable.h"
#include "SuperCPPIntTable.h"

using namespace SuperCPP;

int main()
{
  StringTable<int> names_to_numbers;
  names_to_numbers[ "three" ] = 3;
  names_to_numbers[ "four" ] = 4;
  printf( "%d\n", names_to_numbers.contains("three") );
  printf( "%d\n", names_to_numbers["three"] );

  StringTableEntry<int>* entry = names_to_numbers.find( "three" );
  if (entry) printf( "%s means %d\n", entry->key, entry->value );

  entry = names_to_numbers.find( "five" );
  if (entry) printf( "%s means %d\n", entry->key, entry->value );

  IntTable<char*> numbers_to_names;

  {
    StringTable<int>::Reader reader = names_to_numbers.reader();
    while (reader.has_another())
    {
      entry = reader.read();
      numbers_to_names[ entry->value ] = entry->key;
    }
  }

  {
    IntTable<char*>::Reader reader = numbers_to_names.reader();
    while (reader.has_another())
    {
      IntTableEntry<char*>* number_entry = reader.read();
      printf( "%d -> %s\n", number_entry->key, number_entry->value );
    }
  }

  return 0;
}

