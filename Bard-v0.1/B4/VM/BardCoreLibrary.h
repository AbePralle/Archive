#ifndef BARD_CORE_LIBRARY_H
#define BARD_CORE_LIBRARY_H

#include "Bard.h"

typedef struct BardRandom
{
  BardObject header;
  BardInt64  state;
} BardRandom;

void BardCoreLibrary_configure( BardVM* vm );

void BardConsole__at_newline( BardVM* vm );
void BardConsole__write__Character( BardVM* vm );
void BardConsole__write__String( BardVM* vm );

void BardGenericArray__clear_references__Integer_Integer( BardVM* vm );
void BardGenericArray__shift__Integer_Integer_Integer( BardVM* vm );

void BardInteger64__create__Integer_Integer( BardVM* vm );
void BardInteger64__high( BardVM* vm );
void BardInteger64__low( BardVM* vm );
void BardInteger64__operatorPLUS__Integer64_Integer64( BardVM* vm );
void BardInteger64__operatorMINUS__Integer64_Integer64( BardVM* vm );
void BardInteger64__operatorTIMES__Integer64_Integer64( BardVM* vm );
void BardInteger64__operatorDIVIDEDBY__Integer64_Integer64( BardVM* vm );
void BardInteger64__to_Real( BardVM* vm );

void BardMath__floor__Real( BardVM* vm );
void BardMath__sqrt__Real( BardVM* vm );

void BardNativeFileReader__create( BardVM* vm );
void BardNativeFileReader__create__String( BardVM* vm );
void BardNativeFileReader__available( BardVM* vm );
void BardNativeFileReader__close( BardVM* vm );
void BardNativeFileReader__position( BardVM* vm );
void BardNativeFileReader__read__ByteList_Integer( BardVM* vm );

void BardNativeFileWriter__create( BardVM* vm );
void BardNativeFileWriter__create__String_Logical( BardVM* vm );
void BardNativeFileWriter__close( BardVM* vm );
void BardNativeFileWriter__position( BardVM* vm );
void BardNativeFileWriter__write__ByteList_Integer_Integer( BardVM* vm );

void BardObject__create_instance( BardVM* vm );

void BardRandom__next_state__Real( BardVM* vm );

void BardRuntime__describe__Integer( BardVM* vm );
void BardRuntime__ip__Integer( BardVM* vm );

void BardString__create( BardVM* vm );
void BardString__create__ByteList( BardVM* vm );
void BardString__create__CharacterList( BardVM* vm );
void BardString__count( BardVM* vm );
void BardString__get__Integer( BardVM* vm );
void BardString__hash_code( BardVM* vm );
//void BardString__compare_to__String( BardVM* vm );
//void BardString__equals__String( BardVM* vm );
//void BardString__trace_references( BardVM* vm );

//void BardStringBuilder__write__String( BardVM* vm );

void BardTime__current( BardVM* vm );

#endif // BARD_CORE_LIBRARY_H
