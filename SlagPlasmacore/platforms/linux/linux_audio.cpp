#include "plasmacore.h"

#include <audiere.h>
using namespace audiere;

void LOG( char* mesg, int value );

Archive sound_archive("sounds.zip");

//====================================================================
//  AudiereCore
//====================================================================
struct SoundData
{
  char* data;
  int  length;
  int  sample_rate, total_samples;
  int  reference_count;

  SoundData( char* _data, int len )
  {
    data = _data;
    length = len;
    reference_count = 1;
    sample_rate = 0;
    total_samples = 0;
  }

  ~SoundData()
  {
    if (data)
    {
      delete data;
      data = 0;
    }
  }

  void retain() { ++reference_count; }

  void release()
  {
    if (--reference_count == 0) delete this; 
  }
};


// extending Audiere's File class
class MemoryFile : public RefImplementation<File>
{
  public:
    SoundData* sound_data;
    char *data;
    int pos;
    int length;

    MemoryFile( SoundData* _sound_data )
    {
      sound_data = _sound_data;
      sound_data->retain();
      data   = sound_data->data;
      length = sound_data->length;
      pos    = 0;
    }

    ~MemoryFile() 
    { 
      sound_data->release();
      sound_data = 0;
      data = 0;
    }


    /**
     * Read size bytes from the file, storing them in buffer.
     *
     * @param buffer  buffer to read into
     * @param size    number of bytes to read
     *
     * @return  number of bytes successfully read
     */
    int ADR_CALL read(void* buffer, int size) 
    {
      char* dest = (char*) buffer;

      if (pos + size > length)
      {
        size = length - pos;
      }

      if (size > 0)
      {
        memcpy( dest, data+pos, size );
        pos += size;
      }

      return size;
    }

    /**
     * Jump to a new position in the file, using the specified seek
     * mode.  Remember: if mode is END, the position must be negative,
     * to seek backwards from the end of the file into its contents.
     * If the seek fails, the current position is undefined.
     *
     * @param position  position relative to the mode
     * @param mode      where to seek from in the file
     *
     * @return  true on success, false otherwise
     */
    bool ADR_CALL seek(int position, SeekMode mode)
    {
      switch (mode)
      {
        case BEGIN:
          pos = position;
          break;

        case CURRENT:
          pos += position;
          break;

        case END:
          pos = length + position;
          break;
      }

      bool result = (pos>=0 && pos<=length);
      return result;
    }

    /**
     * Get current position within the file.
     *
     * @return  current position
     */
    int ADR_CALL tell()
    {
      int result = pos;
      return result;
    }
};

struct AudiereSound : BardResource
{
  OutputStreamPtr stream;
  SoundData*      data;

  AudiereSound( OutputStreamPtr _stream, SoundData* _data )
  {
    stream = _stream;
    data = _data;
    data->retain();
  }

  ~AudiereSound()
  {
    if ( !stream ) return;

    stream = 0;  // Audiere takes care of the stream release
    data->release();
  }
};

//====================================================================
//  SoundManager
//====================================================================
struct SoundManager
{
  AudioDevicePtr device;

  ~SoundManager() { shut_down(); }

  void init()
  {
    device = AudioDevicePtr(OpenDevice());
    if ( !device )
    {
      LOG( "ERROR: failed to open audio device." );
    }
  }

  void shut_down()
  {
    device = NULL;
  }

  bool ready()
  {
    return device != NULL;
  }

  BardNativeData* load_sound( char* data, int length, int decode )
  {
    // decode can be 1 (do it), 0 (don't decode), or -1 (decode if
    // decoded is < 50k)
    if (device == NULL) return 0;

    SoundData *sound_data = new SoundData( data, length );
    if (sound_data == 0) return 0;

    FilePtr ptr = new MemoryFile( sound_data );

    // get existing format info
    SampleSourcePtr sample_source = OpenSampleSource( ptr );
    if ( !sample_source )
    {
      sound_data->release();
      sound_data = NULL;
      return 0;
    }

    int channels;
    SampleFormat format;
    sample_source->getFormat( channels, sound_data->sample_rate, format );
    int sample_size = GetSampleSize( format );
    sound_data->total_samples = sample_source->getLength();
    int decoded_size = sample_size * sound_data->total_samples * channels;

    if (decode==1 || (decode==-1 && decoded_size < 100*1024))
    {
      // if we're asked to decode or if the setting is on auto and
      // the decoded size is less than 100k, decode the sound into
      // a WAV format buffer
      char* decoded = new char[decoded_size + 44];
      write_id( decoded, "RIFF" );
      write_int32_low_high( decoded+4, decoded_size + 44 - 8 );
      write_id( decoded+8, "WAVE" );
      write_id( decoded+12, "fmt " );
      write_int32_low_high( decoded+16, 16 );
      write_int16_low_high( decoded+20, 1 );
      write_int16_low_high( decoded+22, channels );
      write_int32_low_high( decoded+24, sound_data->sample_rate );
      write_int32_low_high( decoded+28, sound_data->sample_rate * channels * sample_size );
      write_int16_low_high( decoded+32, channels * sample_size );
      write_int16_low_high( decoded+34, sample_size * 8 );
      write_id( decoded+36, "data" );
      write_int32_low_high( decoded+40, decoded_size );
      sample_source->read( sound_data->total_samples, decoded+44 );
      sound_data->release();
      sample_source = NULL;
      return load_sound( decoded, decoded_size + 44, 0 );
    }
    sample_source = NULL;

    OutputStreamPtr sound_stream( OpenSound(device, ptr, true ) );
    if (!sound_stream) 
    {
      sound_data->release();
      return 0;
    }

    AudiereSound *info = new AudiereSound( sound_stream, sound_data );
    BardNativeData* native_data = BardNativeData::create( info, BardNativeDataDeleteResource );
    sound_data->release();  // it's been retained by the AudiereSound

    return native_data;
  }

  void write_id( char* data, char* id )
  {
    data[0] = (char) id[0];
    data[1] = (char) id[1];
    data[2] = (char) id[2];
    data[3] = (char) id[3];
  }

  void write_int16_low_high( char* data, int n )
  {
    data[0] = (char) n;
    data[1] = (char) (n>>8);
  }

  void write_int32_low_high( char* data, int n )
  {
    write_int16_low_high( data, n );
    write_int16_low_high( data+2, n>>16 );
  }

  BardNativeData* duplicate_sound( AudiereSound* sound )
  {
    if (device == NULL || sound == NULL) return 0;

    FilePtr ptr = new MemoryFile( sound->data );

    OutputStreamPtr sound_stream( OpenSound(device, ptr, true ) );
    if (!sound_stream) return NULL;

    AudiereSound *info = new AudiereSound( sound_stream, sound->data );
    return BardNativeData::create( info, BardNativeDataDeleteResource );
  }
};

SoundManager sound_manager;


// kludge
void sound_manager_init()
{
  sound_manager.init();
}

void sound_manager_shut_down()
{
  sound_manager.shut_down();
}


AudiereSound* get_sound_from_stack()
{
  BardObject* sound_obj = BARD_POP_REF(); // sound object 

  BARD_GET( BardNativeData*, native_data, sound_obj, "native_data" );
  if ( !native_data ) return NULL;

  return (AudiereSound*) (native_data->data);
}

void NativeSound__init__String()
{
  // NativeSound::init(String)
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BardObject* sound_obj = BARD_POP_REF();

  char filename[PATH_MAX];
  filename_obj->to_ascii( filename, PATH_MAX );

  int data_size;
  char* data = sound_archive.load( filename_obj, &data_size );

  if (data) 
  {
    BARD_PUSH_REF( sound_obj );
    BardNativeData* native_data = sound_manager.load_sound( data, data_size, -1 );
    sound_obj = BARD_POP_REF();
    BARD_SET_REF( sound_obj, "native_data", native_data );
  }
  else
  {
    bard_throw_file_error( filename );
  }
}

void NativeSound__init__ArrayList_of_Byte()
{
  // NativeSound::init(Byte[])
  BardArrayList* list = (BardArrayList*) BARD_POP_REF();
  BardObject* sound_obj = BARD_POP_REF();

  char* data = (char*) new char[ list->count ];
  memcpy( data, list->array->data, list->count );

  BARD_PUSH_REF( sound_obj );
  BardNativeData* native_data = sound_manager.load_sound( data, list->count, -1 );
  sound_obj = BARD_POP_REF();

  BARD_SET_REF( sound_obj, "native_data", native_data );
}

void NativeSound__create_duplicate()
{
  // NativeSound::create_duplicate().Sound 

  AudiereSound* sound = get_sound_from_stack();
  if (sound)
  {
    BardNativeData* new_data = sound_manager.duplicate_sound( sound );

    BARD_FIND_TYPE( type_sound, "NativeSound" );
    BardObject* sound_obj = type_sound->create();

    BARD_SET_REF( sound_obj, "native_data", new_data );
    BARD_PUSH_REF( sound_obj );
  }
  else
  {
    BARD_PUSH_REF( NULL );
  }
}

void NativeSound__play()
{
  // NativeSound::play() 

  AudiereSound* sound = get_sound_from_stack();
  if (sound) 
  {
    sound->stream->play();
  }
}

void NativeSound__pause()
{
  // NativeSound::pause() 

  AudiereSound* sound = get_sound_from_stack();
  if (sound) sound->stream->stop();
}

void NativeSound__is_playing()
{
  // NativeSound::is_playing().Logical 

  AudiereSound* sound = get_sound_from_stack();
  if (sound) 
  {
    BARD_PUSH_INT32( sound->stream->isPlaying() );
  }
  else
  {
    BARD_PUSH_INT32( 0 );
  }
}

void NativeSound__volume__Real64()
{
  // NativeSound::volume(Real64) 

  double volume = BARD_POP_REAL64();
  AudiereSound* sound = get_sound_from_stack();
  if (sound) sound->stream->setVolume( (float) volume );
}

void NativeSound__pan__Real64()
{
  // NativeSound::pan(Real64) 
  double setting = BARD_POP_REAL64();
  AudiereSound* sound = get_sound_from_stack();
  if (sound) sound->stream->setPan( (float) setting );
}

void NativeSound__pitch__Real64()
{
  // NativeSound::pitch(Real64) 
  double setting = BARD_POP_REAL64();
  AudiereSound* sound = get_sound_from_stack();

  if (setting < 0.5) setting = 0.5;
  if (setting > 2.0) setting = 2.0;

  if (sound) sound->stream->setPitchShift( (float) setting );
}

void NativeSound__repeats__Logical()
{
  // NativeSound::repeats(Logical) 

  int setting = BARD_POP_INT32();
  AudiereSound* sound = get_sound_from_stack();
  if (sound) sound->stream->setRepeat( 0 < setting );
}

void NativeSound__current_time()
{
  // NativeSound::current_time().Time 

  AudiereSound* sound = get_sound_from_stack();
  if (sound) 
  {
    BardInt64 position = sound->stream->getPosition();
    BARD_PUSH_REAL64( (BardReal64)(position / (double) sound->data->sample_rate) );
  }
  else
  {
    BARD_PUSH_REAL64( 0.0 );
  }
}

void NativeSound__current_time__Real64()
{
  // NativeSound::current_time(Real64) 

  BardReal64 new_time = BARD_POP_REAL64();
  AudiereSound* sound = get_sound_from_stack();
  if (sound) 
  {
    sound->stream->setPosition( (int) (new_time * sound->data->total_samples) );
  }
}

void NativeSound__duration()
{
  // NativeSound::duration().Time 

  AudiereSound* sound = get_sound_from_stack();
  if (sound) 
  {
    BARD_PUSH_REAL64( (sound->data->total_samples / (double) sound->data->sample_rate) );
  }
  else
  {
    BARD_PUSH_REAL64( 0.0 );
  }
}

