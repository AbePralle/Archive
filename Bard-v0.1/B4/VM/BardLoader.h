#ifndef BARD_LOADER_H
#define BARD_LOADER_H

#include "Bard.h"

#define BARD_CURRENT_BC_VERSION 0x20130601

struct BardVM;
struct BardMethod;

int BardLoader_load_bc_file( struct BardVM* vm, const char* filename );
int BardLoader_load_bc_data( struct BardVM* vm, const char* filename, char* data, int free_data );

int BardLoader_load_bc_from_reader( struct BardVM* vm, BardReader* reader );

int                BardLoader_load_count( struct BardVM* vm, BardReader* reader );
char*              BardLoader_load_indexed_id( struct BardVM* vm, BardReader* reader );
BardType*          BardLoader_load_indexed_type( struct BardVM* vm, BardReader* reader );
struct BardMethod* BardLoader_load_indexed_method( struct BardVM* vm, BardReader* reader );

#endif // BARD_LOADER_H
