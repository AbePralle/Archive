//=============================================================================
//  BossUtility.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once
#include "Boss.h"

namespace Boss
{

void    real_to_c_string( Real value, char* buffer, int size );
Integer calculate_hash_code( const char* value );
Integer calculate_hash_code( Character* characters, Integer count );

};  // namespace Boss
