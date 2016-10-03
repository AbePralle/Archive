//=============================================================================
//  GGDemo.cpp
//
//  v1.0.2 - January 15, 2016
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include "ImageIO.h"
#include "GGBitmap.h"
using namespace GG;

Bitmap* load_image( const char* filepath );
int     save_png( Bitmap* bitmap, const char* filepath );

int main()
{
  Bitmap* bitmap;

  bitmap = load_image( "Cats.jpg" );
  if ( !bitmap ) return 1;

  save_png( bitmap, "Output/Original.png" );

  // Crop the image to just the cat.
  bitmap->crop( 100, 180, 60, 200 );
  save_png( bitmap, "Output/Cropped.png" );

  // Save a grayscale copy.  ImageIO doesn't currently support an 8-bit
  // image so it will be saved as 24-bit via a 32-bit save call.
  {
    Bitmap grayscale;
    grayscale.borrow( bitmap );
    grayscale.convert_to_bpp( 8 );
    grayscale.convert_to_bpp( 32 );
    save_png( &grayscale, "Output/Grayscale.png" );
  }

  // Make the image 1/4 size
  bitmap->resize( bitmap->width/4, bitmap->height/4 );

  // Convert the bitmap into repeated four quadrants with four colors
  {
    Bitmap quad;
    int w = bitmap->width;
    int h = bitmap->height;
    quad.init( 0, w*2, h*2, 32 );
    bitmap->blit( &quad, 0, 0, BlendFn_opaque );
    bitmap->blit( &quad, w, 0, BlendFn_opaque );
    bitmap->blit( &quad, 0, h, BlendFn_opaque );
    bitmap->blit( &quad, w, h, BlendFn_opaque );
    quad.fill( w, 0, w, h, 0x80ff0000, BlendFn_blend );
    quad.fill( 0, h, w, h, 0x8000ff00, BlendFn_blend );
    quad.fill( w, h, w, h, 0x800000ff, BlendFn_blend );
    bitmap->adopt( &quad );
  }
  save_png( bitmap, "Output/Quad.png" );

  // Extract and save the red, green, and blue parts of the quad as separate files
  {
    int w = bitmap->width / 2;
    int h = bitmap->height / 2;
    {
      Bitmap red( bitmap, w, 0, w, h );
      save_png( &red, "Output/Red.png" );
    }
    {
      Bitmap green( bitmap, 0, h, w, h );
      save_png( &green, "Output/Green.png" );
    }
    {
      Bitmap blue( bitmap, w, h, w, h );
      save_png( &blue, "Output/Blue.png" );
    }
  }

  delete bitmap;

  return 0;
}

Bitmap* load_image( const char* filepath )
{
  FILE*            fp;
  int              file_size;
  ImageIO::Byte*   buffer;
  ImageIO::Decoder decoder;
  Bitmap*        bitmap;

  printf( "Reading %s\n", filepath );
  fp = fopen( filepath, "rb" );
  if ( !fp )
  {
    printf( "  Unable to open %s for reading!\n", filepath );
    return 0;
  }

  // Determine file size
  fseek( fp, 0, SEEK_END );
  file_size = (int) ftell( fp );
  fseek( fp, 0, SEEK_SET );

  // Load the encoded image into a memory buffer.
  buffer = new ImageIO::Byte[ file_size ];
  fread( buffer, 1, file_size, fp );
  fclose( fp );

  // Initialize the ImageIO decoder and open the file to obtain
  // image dimensions.
  if ( !decoder.open(buffer,file_size) )
  {
    printf( "  Error opening %s - not a valid PNG or JPEG file.", filepath );
    delete buffer;
    return 0;
  }

  printf( "  %s dimensions: %dx%d\n", filepath, decoder.width, decoder.height );

  bitmap = new Bitmap( 0, decoder.width, decoder.height, 32 );
  if ( !decoder.decode_argb32((ImageIO::ARGB32*)bitmap->data) )
  {
    printf( "  Error decoding %s.\n", filepath );
    delete buffer;
    return 0;
  }

  return bitmap;
}

int save_png( Bitmap* _bitmap, const char* filepath )
{
  FILE*            fp;
  ImageIO::Encoder encoder;
  Bitmap         bitmap;

  printf( "Writing %s\n", filepath );
  fp = fopen( filepath, "wb" );
  if ( !fp )
  {
    printf( "  Unable to open %s for writing!\n", filepath );
    return 0;
  }

  // Work with a borrowed reference to the Bitmap parameter.
  // If the bitmap is already 32 bpp then we won't be duplicating data,
  // but if not then the bpp conversion will automatically make a copy
  // of the data and won't affect the original.
  bitmap.borrow( _bitmap );

  // Ensure bitmap is 32 bpp
  bitmap.convert_to_bpp( 32 );

  // Initialize the ImageIO encoder and encode the image.
  encoder.encode_argb32_png( (ImageIO::ARGB32*)bitmap.data, bitmap.width, bitmap.height );

  // Write out the image data.
  fwrite( encoder.encoded_bytes, 1, encoder.encoded_byte_count, fp );

  fclose( fp );

  return 1;
}

