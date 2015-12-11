#ifndef BARD_TYPE_H
#define BARD_TYPE_H

#include "Bard.h"

// Forward declarations
struct BardVM;
struct BardMethod;

//=============================================================================
//  BardType
//=============================================================================
typedef struct BardType
{
  struct BardVM*        vm;
  char*                 name;

  char**                tags;
  int                   tag_count;

  struct BardType**     base_types;
  int                   base_type_count;

  struct BardProperty*  settings;
  int                   settings_count;
  unsigned char*        settings_data;

  struct BardProperty*  properties;
  int                   property_count;

  struct BardMethod**   methods;
  int                   method_count;

  int                   aspect_count;
  struct BardType**     aspect_types;
  struct BardMethod***  aspect_call_tables;

  BardInteger           attributes;
  BardInteger           object_size;
  BardInteger           reference_size;
  BardInteger           stack_slots;
  BardInteger           stack_size;   // # == stack slots * 8 bytes

  BardObject*           singleton;

  BardInteger           data_offset;

  struct BardMethod*    m_init_defaults;

} BardType;

BardType*   BardType_create( struct BardVM* vm );
BardType*   BardType_init( BardType* type, char* name );
void        BardType_free( BardType* type );

void        BardType_set_reference_sizes( BardType* type );
void        BardType_set_up_settings( BardType* type );
void        BardType_set_property_offsets( BardType* type );
void        BardType_organize( BardType* type );

struct BardProperty* BardType_find_property( BardType* type, const char* name );

struct BardMethod* BardType_find_method( BardType* type, const char* signature );
BardObject*        BardType_create_object( BardType* type );
BardObject*        BardType_create_object_of_size( BardType* type, int byte_size );

int         BardType_partial_instance_of( BardType* type, BardType* ancestor_type );

#endif // BARD_TYPE_H
