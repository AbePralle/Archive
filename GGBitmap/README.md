# GGBitmap Library

<table>
  <tr>
    <td>Purpose</td>
    <td>GG::Bitmap (Generic Graphics Bitmap) is a C++ class that implements a general-purpose software bitmap for managing 32 and 16-bit ARGB bitmaps, 24-bit RGB bitmaps, and 8-bit grayscale bitmaps.  Functionality includes cropping, image resizing, bits-per-pixel conversion, blitting, solid color fills, and subset bitmaps.  It does not load or save Windows BMP files or any other formats itself (see Abe Pralle's [http://www.github.com/AbePralle/ImageIO](ImageIO) for a simple PNG and JPG I/O library).</td>
  </tr>
  <tr>
    <td>Current Version</td>
    <td>1.0.4 - April 9, 2016</td>
  </tr>
  <tr>
    <td>Language</td>
    <td>C++</td>
  </tr>
  <tr>
    <td>Dependencies</td>
    <td>None.</td>
  </tr>
</table>


## Usage
Compile `GGBitmap.h` and `GGBitmap.cpp` in with your project, `#include "GGBitmap.h"`, and either proceed `using namespace GG;` or prefix symbols with `GG::` as needed.

### Project Namespace
The GGBitmap library is defined within the nested namespace `PROJECT_NAMESPACE::GG`, where `PROJECT_NAMESPACE` is a `#define` that defaults to `Project`.  Including "ImageIO.h" automatically issues the directive `using namespace PROJECT_NAMESPACE;`.  If you're wanting to use ImageIO in a precompiled library and avoid conflicts with end users who might also use ImageIO, define `PROJECT_NAMESPACE=SomeOtherNamespace` as a compiler flag.


## Demo
Open a command prompt in `Source/Demo`, run `make` to compile and execute, and then inspect the results in `Output/` and the source code in `Demo.c`.


## Data Types
Bitmap uses the following typedefs:

Type        | Description
------------|---------------
`Pixel32`   | Unsigned 32-bit.
`Pixel16`   | Unsigned 16-bit.
`Pixel8`    | Unsigned 8-bit.
`Color`     | Interchangeable with `int`.  Stores an ARGB32 color value (0xAARRGGBB).

## API

### Bitmap Struct
The Bitmap struct has the following properties:

Name        |Type    |Description
------------|--------|----------------
`width`     |`int`   |The width of the bitmap in pixels.
`height`    |`int`   |The height of the bitmap in pixels.
`bpp`       |`int`   |Bits Per Pixel - 8, 16, 24, or 32.
`bypp`      |`int`   |BYtes Per Pixel - 1, 2, 3, or 4.
`owns_data` |`int`   |Indicates whether or not the pixel data should be freed when the bitmap is retired or deleted.
`data`      |`void*` |A pointer to 8, 16, 24, or 32-bit pixel data.
`data_size` |`int`   |The size, in bytes, of the pixel data.

### Adopt, Borrow, and Copy
`Bitmap(...)` accepts an `options` parameter which should contain a BPP (bits-per-pixel) value (32/24/16/8) and may be bitwise-or'd with one of the following pixel data management options:

Constant                |Value |Description
------------------------|------|---------------------------
`OPTION_ADOPT_DATA`  |1     |Uses the supplied `pixel_data` and becomes responsible for freeing it.
`OPTION_BORROW_DATA` |0     |Uses the supplied `pixel_data` but is not responsible for freeing it.  Take care not to free the pixel data as long as the bitmap is still using it.
`OPTION_COPY_DATA`   |2     |Makes a copy of the supplied `pixel_data`.  The original data may be freed.

### BlendFn Callbacks
Various drawing functions accept a blend function callback that has the following signature:

```C
fn_name( src:Color src, dest:Color )→Color
```

It is called whenever a `src` ARGB32 color value will be overlaid on a `dest` ARGB32 color.  The callback should blend the colors as desired and return the result.

The following BlendFn callbacks are already defined:

Callback                      | Description
------------------------------|-----------------------------------
`BlendFn_blend`               | A standard blend based on the source alpha.
`BlendFn_blend_premultiplied` | Blends using the assumption that the source R,G,B components have already been multiplied by the source alpha value.
`BlendFn_opaque`              | Replaces the destination with the source.

### FilterFn Callbacks
The `Bitmap_filter()` function accepts a callback that has the following signature:

```C
fn_name( bitmap:Bitmap*, x:int, y:int, color:Color )→Color
```

Filter function callbacks allow pixels to be modified based on their color, position, and/or surrounding pixel data.

The following FilterFn callbacks are already defined:

Callback                      | Description
------------------------------|-----------------------------------
`FilterFn_premultiply_alpha`  | Multiplies each pixel's R,G,B components by its Alpha value, e.g. `red = (red * alpha) / 255`.
`FilterFn_swap_red_and_blue`  | Swaps the Red and Blue components of each pixel.

### Creating Bitmap Objects
<table>
  <tr><td>
    <b>
      Bitmap();
    </b><br/>
    <div style="margin-left:20px">
      Creates an empty Bitmap with no data.  `adopt()`, `borrow()`, or `copy()` can be called to assign data.
    </div>
  </td></tr>
  <tr><td>
    <b>
      Bitmap( pixel_data:void*, width:int, height:int, options:int )
    </b><br/>
    <div style="margin-left:20px">
      <code>options</code> should be a BPP value 32/24/16/8 optionally bitwise-or'd with Bitmap::OPTION_ADOPT/BORROW/COPY_DATA.  If <code>pixel_data</code> is 0 (NULL) then new pixel data is allocated using the given <code>width</code> and <code>height</code>.
    </div>
  </td></tr>
  <tr><td>
    <b>
      Bitmap( existing:Bitmap* )→Bitmap*
    </b><br/>
    <div style="margin-left:20px">
      Creates a Bitmap containing a copy of an existing Bitmap's data.
    </div>
  </td></tr>
  <tr><td>
    <b>
      Bitmap( existing:Bitmap*, x:int, y:int, width:int, height:int )→Bitmap*
    </b><br/>
    <div style="margin-left:20px">
      Creates a Bitmap containing a copied subset of an existing Bitmap.
    </div>
  </td></tr>
  <tr><td>
    <b>
      init( pixel_data:void*, width:int, height:int, options:options )→Bitmap*
    </b><br/>
    <div style="margin-left:20px">
      Frees any existing data and adopts, borrows, or copies the specified pixel data.  If the pixel data does not exist
      then new pixel data is created of the given dimensions and bits per pixel.
    </div>
  </td></tr>
  <tr><td>
    <b>
      retire()→Bitmap*
    </b><br/>
    <div style="margin-left:20px">
      Frees the pixel data of a given Bitmap object if necessary.
    </div>
  </td></tr>
</table>

### Adopting, Borrowing, or Copying an Existing Bitmap's Data
<table>
  <tr><td>
    <b>
      adopt( other:Bitmap* )
    </b><br/>
    <div style="margin-left:20px">
      Frees this bitmap's existing pixel data if necessary and adopts the pixel data of the other bitmap.  If the other bitmap owned the data then the ownership transfers; otherwise neither bitmap owns the data.
    </div>
  </td></tr>
  <tr><td>
    <b>
      borrow( other:Bitmap* )
    </b><br/>
    <div style="margin-left:20px">
      Frees this bitmap's existing pixel data if necessary and borrows the pixel data of the other bitmap.
    </div>
  </td></tr>
  <tr><td>
    <b>
      copy( other:Bitmap* )
    </b><br/>
    <div style="margin-left:20px">
      Frees this bitmap's existing pixel data if necessary and copies the pixel data of the other bitmap.
    </div>
  </td></tr>
</table>

### Getting and Setting Pixel Values
<table>
  <tr><td>
    <b>
      get( x:int, y:int )→Color
    </b><br/>
    <div style="margin-left:20px">
      Returns the ARGB32 color of pixel (x,y) in this bitmap.  (0,0) is the top-left pixel and (bitmap->width-1,bitmap->height-1) is the bottom-right pixel.  No parameter validation is performed.
    </div>
  </td></tr>
  <tr><td>
    <b>
      set( x:int, y:int, color:Color )
    </b><br/>
    <div style="margin-left:20px">
      Sets the ARGB32 color of pixel (x,y) in this bitmap.  (0,0) is the top-left pixel and (bitmap->width-1,bitmap->height-1) is the bottom-right pixel.  No parameter validation is performed.
    </div>
  </td></tr>
</table>


### Data Manipulation and Conversion
<table>
  <tr><td>
    <b>
      convert_to_bpp( bpp:int )
    </b><br/>
    <div style="margin-left:20px">
      Converts this bitmap to have any other number of bits per pixel - 32, 24, 16, or 8.  The original pixel data is freed if necessary and replaced by the new pixel data.
    </div>
  </td></tr>
  <tr><td>
    <b>
      ensure_data_ownership()
    </b><br/>
    <div style="margin-left:20px">
      If this bitmap does not have ownership of its `pixel_data` (due to "borrowing" existing data) then it creates its own copy of its pixel data.
    </div>
  </td></tr>
  <tr><td>
    <b>
      expand_to_power_of_two()
    </b><br/>
    <div style="margin-left:20px">
      Ensures this bitmap's pixel data is a power of two in width and height, often necessary in hardware-accelerated graphics.  Any additional padding is set to be a copy of the rightmost or bottommost edge of the original surface to avoid rendering artifacts.
    </div>
  </td></tr>
  <tr><td>
    <b>
      filter( FilterFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Rewrites each pixel of this bitmap using the result of the given filter function callback.  For example: <code>filter( bitmap, GG::FilterFn_premultiply_alpha )</code>.
    </div>
  </td></tr>
  <tr><td>
    <b>
      filter_area( x:int, y:int, w:int, h:int, FilterFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Filters a subset of the entire bitmap - see <code>filter()</code> above.
    </div>
  </td></tr>
</table>


### Cropping and Resizing
<table>
  <tr><td>
    <b>
      crop( left:int, top:int, right:int, bottom:int )
    </b><br/>
    <div style="margin-left:20px">
      Crops this bitmap by the number of pixels specified for each edge.  The original pixel data is freed if necessary and replaced by the cropped pixel data.
    </div>
  </td></tr>
  <tr><td>
    <b>
      resize( new_width:int, new_height:int )
    </b><br/>
    <div style="margin-left:20px">
      Resizes this bitmap in place.  Pixel color values are averaged if the new size is less than the original size.  The original pixel data is freed if necessary and replaced by the resized pixel data.
    </div>
  </td></tr>
  <tr><td>
    <b>
      resize_to( dest_bitmap:Bitmap* )
    </b><br/>
    <div style="margin-left:20px">
      Overrwrites the destination bitmap with a copy of the this bitmap resized to the destination bitmap's dimensions.  If the destination bitmap is 32 BPP then its pixel data will be overrwritten; otherwise it will be freed if necessary and replaced.
    </div>
  </td></tr>
</table>



### Drawing and Filling
<table>
  <tr><td>
    <b>
      blit( dest_bitmap:Bitmap*, x:int, y:int, BlendFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Blits (draws) this bitmap onto a destination bitmap with its top-left corner at the given (x,y) location and using the given blend function (described above).
    </div>
  </td></tr>
  <tr><td>
    <b>
      blit_area( sx:int, sy:int, w:int, h:int, dest_bitmap:Bitmap*, dx:int, dy:int, BlendFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Blits (draws) a subset of this bitmap onto a destination bitmap with the subset's top-left corner at the given (x,y) location and using the given blend function (described above).
    </div>
  </td></tr>
  <tr><td>
    <b>
      blit_column( src_x:int, dest_x:int, BlendFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Copies a column of pixels from one column to another within this bitmap.
    </div>
  </td></tr>
  <tr><td>
    <b>
      blit_row( src_y:int, dest_y:int, BlendFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Copies a row of pixels from one row to another within the this bitmap.
    </div>
  </td></tr>
  <tr><td>
    <b>
      draw_border( x:int, y:int, w:int, h:int, Color color, BlendFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Draws a rectangular border on this bitmap.
    </div>
  </td></tr>
  <tr><td>
    <b>
      fill( x:int, y:int, w:int, h:int, Color color, BlendFn fn )
    </b><br/>
    <div style="margin-left:20px">
      Fills a rectangular area on this bitmap.
    </div>
  </td></tr>
</table>


## Change Log

### v1.0.4 - April 9, 2016
-  Fixed blit clipping error that was using `width` instead of `height`.

### v1.0.3 - February 2, 2016
-  Removed semicolon after namespace closing brace to prevent a C++-11 warning.

### v1.0.2 - January 15, 2016
-  Converted library to C++.

### v1.0.1 - January 3, 2016
-  Fixed bug in GGBitmap_expand_to_power_of_two().

### v1.0.0 - December 31, 2015
-  Original release.

