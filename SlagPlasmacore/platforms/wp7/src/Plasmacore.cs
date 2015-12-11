//=============================================================================
// Plasmacore.cs
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Plasmacore Native Layer implementation for Windows Phone 7.
// 
// ----------------------------------------------------------------------------
//
// Copyright 2010 Plasmaworks LLC
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
//   http://www.apache.org/licenses/LICENSE-2.0 
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an 
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
// either express or implied. See the License for the specific 
// language governing permissions and limitations under the License.
//
//=============================================================================

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.IsolatedStorage;
using System.Linq;
using System.Net;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Input.Touch;
using Microsoft.Xna.Framework.Media;

namespace BardFramework
{
  public class Plasmacore
  {

    public const int PIXEL_FORMAT_RGB32   = 1;
    public const int PIXEL_FORMAT_RGB16   = 2;
    public const int PIXEL_FORMAT_INDEXED = 4;

    static public GraphicsDevice device;
    static public ContentManager content;

    static public int display_width, display_height;
    static public ClassString event_launch;
    static public ClassString event_update;
    static public ClassString event_draw;
    static public ClassString event_mouse_move;
    static public ClassString event_mouse_button;
    static public ClassString event_key;

    static public long   last_update_ms;
    static public double time_debt;

    static public double accel_x, accel_y, accel_z;

    static public void configure( ContentManager content, GraphicsDevice device, 
        int display_width, int display_height )
    {
      Plasmacore.content = content;
      Plasmacore.device = device;
      DrawBuffer.init(device);

      GameXC.configure();
      GameXC.singleton_ClassGlobal.property_stdout = new ClassTraceWriter(Bard.CallInit.TRUE);

      Plasmacore.display_width = display_width;
      Plasmacore.display_height = display_height;
      event_launch = GameXC.singleton_ClassSignalManager.find_signal_id(Bard.str("launch"));
      event_update = GameXC.singleton_ClassSignalManager.find_signal_id(Bard.str("update"));
      event_draw   = GameXC.singleton_ClassSignalManager.find_signal_id(Bard.str("draw"));
      event_mouse_move = 
        GameXC.singleton_ClassSignalManager.find_signal_id(Bard.str("mouse_move"));
      event_mouse_button = 
        GameXC.singleton_ClassSignalManager.find_signal_id(Bard.str("mouse_button"));
      event_key = 
        GameXC.singleton_ClassSignalManager.find_signal_id(Bard.str("key"));
    }

    static public void launch()
    {
      GameXC.init();
      GameXC.singleton_ClassSignalManager.queue_native( 
          event_launch,
          new ClassSignalObjectArg( GameXC.main_object, display_width, display_height ) 
        );
      GameXC.singleton_ClassSignalManager.raise_pending();
    }

    static public void update()
    {
      long cur_ms = (DateTime.Now.Ticks/10000);
      time_debt += (cur_ms - last_update_ms) / 1000.0;
      last_update_ms = cur_ms;

      if (time_debt < 0 || time_debt >= 8.0/60.0) time_debt = 1.0/60.0;

      ClassInput input = GameXC.singleton_ClassInput;
      input.property_acceleration_x = -accel_x;
      input.property_acceleration_y = accel_y;
      input.property_acceleration_z = accel_z;

      while (time_debt >= 1.0/60.0)
      {
        GameXC.singleton_ClassSignalManager.queue_native( event_update, null );
        GameXC.singleton_ClassSignalManager.raise_pending();
        time_debt -= 1.0/60.0;
      }
    }

    static public void on_mouse_move_absolute( int mouse_id, double x, double y )
    {
      GameXC.singleton_ClassSignalManager.queue_native( event_mouse_move,
          new ClassSignalDataArg( mouse_id, 0, true, x, y ) );
    }

    static public void on_mouse_button_press( int mouse_id, double x, double y, int button_index )
    {
      GameXC.singleton_ClassSignalManager.queue_native( event_mouse_button, 
          new ClassSignalDataArg( mouse_id, button_index, true, x, y ) );
    }

    static public void on_mouse_button_release( int mouse_id, double x, double y, int button_index )
    {
      GameXC.singleton_ClassSignalManager.queue_native( event_mouse_button, 
          new ClassSignalDataArg( mouse_id, button_index, false, x, y ) );
    }

    static public void on_key( bool is_press, int code, bool is_unicode )
    {
      GameXC.singleton_ClassSignalManager.queue_native( event_key, 
          new ClassSignalDataArg( is_unicode?1:0, code, is_press, 0.0, 0.0 ) );
    }

    static public void draw()
    {
      int c = GameXC.singleton_ClassDisplay.property_background_color.property_argb;
      if ((c & unchecked((int)0xff000000)) != 0)
      {
        Color color = new Color((c>>16)&255,(c>>8)&255,c&255,(c>>24)&255);
        device.Clear(color);
      }

      NativeTransformManager.reset();
      GameXC.singleton_ClassSignalManager.queue_native( event_draw, null );
      GameXC.singleton_ClassSignalManager.raise_pending();
      DrawBuffer.render();
    }

    static public Vector3 v3( CompoundVector2 v )
    {
      return new Vector3( (float) v.property_x, (float) v.property_y, 0f );
    }
  }

  public struct Matrix2x3
  {
    public double r1c1, r1c2, r1c3;
    public double r2c1, r2c2, r2c3;

    public Matrix2x3( double v1, double v2, double v3, double v4, double v5, double v6 ) 
    {
      r1c1 = v1;
      r1c2 = v2;
      r1c3 = v3;
      r2c1 = v4;
      r2c2 = v5;
      r2c3 = v6; 
    }

    public Matrix2x3 times( Matrix2x3 other )
    {
      /*
      a b c * g h i
      d e f   j k l
      0 0 1   0 0 1
       =
      ag+bj ah+bk ai+bl+c
      dg+ej dh+ek di+el+f
      */
      return new Matrix2x3(
        r1c1*other.r1c1+r1c2*other.r2c1,
        r1c1*other.r1c2+r1c2*other.r2c2,
        r1c1*other.r1c3+r1c2*other.r2c3 + r1c3,
        r2c1*other.r1c1+r2c2*other.r2c1,
        r2c1*other.r1c2+r2c2*other.r2c2,
        r2c1*other.r1c3+r2c2*other.r2c3 + r2c3 
      );
    }

    public double determinant()
    {
      return r1c1*r2c2 - r1c2*r2c1;
    }

    public Matrix2x3 inverse( Matrix2x3 other )
    {
      double invdet = 1.0 / (r1c1*r2c2 - r1c2*r2c1);
      Matrix2x3 result;

      result.r1c1 =  r2c2 * invdet;
      result.r1c2 = -r1c2 * invdet;
      result.r1c3 =  (r1c2*r2c3 - r1c3*r2c2) * invdet;

      result.r2c1 = -r2c1 * invdet;
      result.r2c2 =  r1c1 * invdet;
      result.r2c3 =  (r1c3*r2c1 - r1c1*r2c3) * invdet;

      return result;
    }

    public Vector3 transform( Vector3 v )
    {
      return new Vector3(
        (float)(v.X * r1c1 + v.Y * r1c2 + r1c3 - 0.5),
        (float)(v.X * r2c1 + v.Y * r2c2 + r2c3 - 0.5),
        v.Z
      );
    }
  }

  public class DrawBuffer
  {
    public const int RENDER_FLAG_POINT_FILTER = (1 << 0);
    public const int RENDER_FLAG_FIXED_COLOR  = (1 << 2);
    public const int RENDER_FLAG_TEXTURE_WRAP = (1 << 3);

    public const int BLEND_ZERO               =  0;
    public const int BLEND_ONE                =  1;
    public const int BLEND_SRC_ALPHA          =  2;
    public const int BLEND_INVERSE_SRC_ALPHA  =  3;
    public const int BLEND_DEST_ALPHA         =  4;
    public const int BLEND_INVERSE_DEST_ALPHA =  5;
    public const int BLEND_DEST_COLOR         =  6;
    public const int BLEND_INVERSE_DEST_COLOR =  7;
    public const int BLEND_OPAQUE             =  8;
    public const int BLEND_SRC_COLOR          =  9;
    public const int BLEND_INVERSE_SRC_COLOR  = 10;
    public const int MAX_BUFFERED_VERTICES   = 512*3;
    public const int DRAW_TEXTURED_TRIANGLES = 1;
    public const int DRAW_SOLID_TRIANGLES    = 2;
    public const int DRAW_LINES              = 3;
    public const int DRAW_POINTS             = 4;

    static public VertexPositionColorTexture[] vertices;
    static public VertexPositionColorTexture[] alpha_mask_vertices;

    static public GraphicsDevice device;
    static public BasicEffect    basic_effect;

    static public int draw_mode;
    static public int count;
    static public int render_flags;

    static public int   vertex_pos;

    static public Color constant_color;
    static public int   src_blend, dest_blend;

    static public Texture2D texture;
    static public Texture2D alpha_mask;
    //Texture2D draw_target;

    static public void init( GraphicsDevice device )
    {
      DrawBuffer.device = device;

      RasterizerState new_rasterizer_state = new RasterizerState();
      new_rasterizer_state.CullMode = CullMode.None;
      new_rasterizer_state.ScissorTestEnable = true;
      device.RasterizerState = new_rasterizer_state;
      device.BlendState = BlendState.AlphaBlend;

      draw_mode = DrawBuffer.DRAW_TEXTURED_TRIANGLES;
      src_blend = DrawBuffer.BLEND_SRC_ALPHA;
      dest_blend = DrawBuffer.BLEND_INVERSE_SRC_ALPHA;
      constant_color = Color.Black;

      vertices = new VertexPositionColorTexture[MAX_BUFFERED_VERTICES];
      alpha_mask_vertices = new VertexPositionColorTexture[MAX_BUFFERED_VERTICES];

      basic_effect = new BasicEffect(device);
      basic_effect.World = Matrix.Identity;
      basic_effect.View = Matrix.Identity;
      basic_effect.Projection = Matrix.CreateOrthographicOffCenter(
        0, PlasmacoreSettings.display_width,
        PlasmacoreSettings.display_height, 0,
        -1, 1 );
    }

    static public void reset()
    {
      count = 0;
      vertex_pos = 0;
    }

    static public void set_render_flags( int flags, int src_blend, int dest_blend )
    {
      if (render_flags != flags 
        || DrawBuffer.src_blend != src_blend || DrawBuffer.dest_blend != dest_blend)
      {
        render();
      }
      render_flags = flags;
      DrawBuffer.src_blend = src_blend;
      DrawBuffer.dest_blend = dest_blend;
    }

    static public void set_textured_triangle_mode( ClassTexture texture, ClassTexture alpha_mask )
    {
      if (alpha_mask == null)
      {
        set_textured_triangle_mode( ((NativeTextureData) texture.property_native_data).texture,
          null );
      }
      else
      {
        set_textured_triangle_mode( ((NativeTextureData) texture.property_native_data).texture,
          (alpha_mask.property_native_data as NativeTextureData).texture );
      }
    }

    static public void set_textured_triangle_mode( Texture2D texture, Texture2D alpha_mask )
    {
      if (draw_mode != DrawBuffer.DRAW_TEXTURED_TRIANGLES 
        || DrawBuffer.texture != texture || DrawBuffer.alpha_mask != alpha_mask) 
      {
        render();
      }
      draw_mode = DrawBuffer.DRAW_TEXTURED_TRIANGLES;
      DrawBuffer.texture = texture;
      DrawBuffer.alpha_mask = alpha_mask;
    }

    static public void set_solid_triangle_mode()
    {
      if (draw_mode != DrawBuffer.DRAW_SOLID_TRIANGLES) render();
      draw_mode = DrawBuffer.DRAW_SOLID_TRIANGLES;
    }

    static public void set_line_mode()
    {
      if (draw_mode != DrawBuffer.DRAW_LINES) render();
      draw_mode = DrawBuffer.DRAW_LINES;
    }

    static public void set_point_mode()
    {
      if (draw_mode != DrawBuffer.DRAW_POINTS) render();
      draw_mode = DrawBuffer.DRAW_POINTS;
    }

    static public void set_draw_target( Texture2D target )
    {
      //Plasmacore.log("TODO: set_draw_target()");

      /*
      if (draw_target == target) return;

      render();
      draw_target = target;

      if (draw_target) 
      {
        glBindFramebufferOES( GL_FRAMEBUFFER_OES, draw_target->frame_buffer );
        glBindTexture( GL_TEXTURE_2D, 0 );
        glViewport(0, 0, draw_target->texture_width, draw_target->texture_height );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
    #if defined(TARGET_OS_IPHONE) || defined(ANDROID)
        glOrthox( 0, draw_target->texture_width<<16, 0, draw_target->texture_height<<16, -1<<16, 1<<16 );
    #else
        glOrtho( 0, draw_target->texture_width, 0, draw_target->texture_height, -1, 1 );
    #endif
        glMatrixMode(GL_MODELVIEW);
      }
      else 
      {
        glBindFramebufferOES( GL_FRAMEBUFFER_OES, MAIN_BUFFER );
        glViewport(0, 0, plasmacore.display_width, plasmacore.display_height );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
    #if defined(TARGET_OS_IPHONE) || (ANDROID)
        glOrthox( 0, plasmacore.display_width<<16, plasmacore.display_height<<16, 0, -1<<16, 1<<16 );
    #else
        glOrtho( 0, plasmacore.display_width, plasmacore.display_height, 0, -1, 1 );
    #endif
        glMatrixMode(GL_MODELVIEW);
        glEnable( GL_BLEND );
      }
      */

    }

    static public void add( VertexPositionColorTexture v1, VertexPositionColorTexture v2, 
        VertexPositionColorTexture v3 )
    {
      if (count == MAX_BUFFERED_VERTICES) render();

      if ((render_flags & DrawBuffer.RENDER_FLAG_FIXED_COLOR) != 0
        && draw_mode == DRAW_TEXTURED_TRIANGLES
        && !constant_color.Equals(v1.Color))
      {
        render();
        constant_color = v1.Color;
      }

      vertices[vertex_pos++] = v1;
      vertices[vertex_pos++] = v2;
      vertices[vertex_pos++] = v3;
      count += 3;
    }

    static public void add( VertexPositionColorTexture v1, VertexPositionColorTexture v2, 
              VertexPositionColorTexture v3,
              VertexPositionColorTexture alpha_v1, VertexPositionColorTexture alpha_v2, 
              VertexPositionColorTexture alpha_v3 )
    {
      if (count == MAX_BUFFERED_VERTICES) render();

      if ((render_flags & DrawBuffer.RENDER_FLAG_FIXED_COLOR) != 0
        && !constant_color.Equals(v1.Color))
      {
        render();
        constant_color = v1.Color;
      }

      vertices[vertex_pos]              = v1;
      alpha_mask_vertices[vertex_pos++] = alpha_v1;
      vertices[vertex_pos]              = v2;
      alpha_mask_vertices[vertex_pos++] = alpha_v2;
      vertices[vertex_pos]              = v3;
      alpha_mask_vertices[vertex_pos++] = alpha_v3;
      count += 3;
    }

    static public void add( VertexPositionColorTexture v1, VertexPositionColorTexture v2 )
    {
      if (count == MAX_BUFFERED_VERTICES) render();

      vertices[vertex_pos++] = v1;
      vertices[vertex_pos++] = v2;
      count += 2;
    }

    static public void add( VertexPositionColorTexture v )
    {
      if (count == MAX_BUFFERED_VERTICES) render();

      vertices[vertex_pos++] = v;
      ++count;
    }

    static public void render()
    {
      if (count == 0) return;

      basic_effect.VertexColorEnabled = true;

      Blend src, dest;
      switch (src_blend)
      {
        case BLEND_ZERO:
          src = Blend.Zero;
          break;
        case BLEND_ONE:
          src = Blend.One;
          break;
        case BLEND_SRC_ALPHA:
          src = Blend.SourceAlpha;
          break;
        case BLEND_INVERSE_SRC_ALPHA:
          src = Blend.InverseSourceAlpha;
          break;
        case BLEND_DEST_ALPHA:
          src = Blend.DestinationAlpha;
          break;
        case BLEND_INVERSE_DEST_ALPHA:
          src = Blend.InverseDestinationAlpha;
          break;
        case BLEND_DEST_COLOR:
          src = Blend.DestinationColor;
          break;
        case BLEND_INVERSE_DEST_COLOR:
          src = Blend.InverseDestinationColor;
          break;
        case BLEND_OPAQUE:
          src = Blend.SourceAlphaSaturation;
          break;
        default:
          src = Blend.SourceAlpha;
          break;
      }

      switch (dest_blend)
      {
        case BLEND_ZERO:
          dest = Blend.Zero;
          break;
        case BLEND_ONE:
          dest = Blend.One;
          break;
        case BLEND_SRC_ALPHA:
          dest = Blend.SourceAlpha;
          break;
        case BLEND_INVERSE_SRC_ALPHA:
          dest = Blend.InverseSourceAlpha;
          break;
        case BLEND_DEST_ALPHA:
          dest = Blend.DestinationAlpha;
          break;
        case BLEND_INVERSE_DEST_ALPHA:
          dest = Blend.InverseDestinationAlpha;
          break;
        case BLEND_DEST_COLOR:
          dest = Blend.DestinationColor;
          break;
        case BLEND_INVERSE_DEST_COLOR:
          dest = Blend.InverseDestinationColor;
          break;
        default:
          dest = Blend.DestinationAlpha;
          break;
      }

      if ((render_flags & RENDER_FLAG_FIXED_COLOR) != 0)
      {
        basic_effect.FogColor = new Vector3( 
          constant_color.R/255.0f,
          constant_color.G/255.0f,
          constant_color.B/255.0f
        );
        basic_effect.Alpha = constant_color.A / 255.0f;
        basic_effect.FogEnabled = true;
        basic_effect.FogStart = 1;
        basic_effect.FogEnd = 0;
      }
      else
      {
        basic_effect.FogEnabled = false;
        basic_effect.Alpha = 1;
      }

      BlendState blend_state = new BlendState();
      blend_state.AlphaSourceBlend = src;
      blend_state.ColorSourceBlend = src;
      blend_state.ColorDestinationBlend = dest;
      blend_state.AlphaDestinationBlend = dest;
      device.BlendState = blend_state;

      switch (draw_mode)
      {
        case DrawBuffer.DRAW_TEXTURED_TRIANGLES:
          /*
          // set texture addressing to WRAP or CLAMP
          if (render_flags & RENDER_FLAG_TEXTURE_WRAP)
          {
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
          }
          else
          {
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
          }

          if (render_flags & RENDER_FLAG_POINT_FILTER)
          {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          }
          else
          {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          }

          if (alpha_src)
          {
            glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
            glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);

            glActiveTexture( GL_TEXTURE1 );
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, alpha_src->id );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // grab color from first stage
            glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE );
            glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS );

            // use the alpha from second stage
            glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE );
            glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE );

            glActiveTexture( GL_TEXTURE0 );

          }
          else
          {
            glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE );
            glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE );
            glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
          }

          glDrawArrays(GL_TRIANGLES, 0, count);
          glDisable(GL_TEXTURE_2D);
          glDisableClientState(GL_TEXTURE_COORD_ARRAY);

          if (alpha_src)
          {
            glActiveTexture( GL_TEXTURE1 );
            glDisable( GL_TEXTURE_2D );
            glActiveTexture( GL_TEXTURE0 );
          }
          */
          basic_effect.Texture = texture;
          basic_effect.TextureEnabled = true;

          foreach (EffectPass pass in basic_effect.CurrentTechnique.Passes)
          {
            pass.Apply();

            device.DrawUserPrimitives<VertexPositionColorTexture>(
                PrimitiveType.TriangleList, vertices, 0, count/3 );
          }

          break;

        case DrawBuffer.DRAW_SOLID_TRIANGLES:
          basic_effect.TextureEnabled = false;
          foreach (EffectPass pass in basic_effect.CurrentTechnique.Passes)
          {
            pass.Apply();

            device.DrawUserPrimitives<VertexPositionColorTexture>(
                PrimitiveType.TriangleList, vertices, 0, count/3 );
          }
          break;

        case DrawBuffer.DRAW_LINES:
          basic_effect.TextureEnabled = false;
          foreach (EffectPass pass in basic_effect.CurrentTechnique.Passes)
          {
            pass.Apply();

            device.DrawUserPrimitives<VertexPositionColorTexture>(
                PrimitiveType.LineList, vertices, 0, count/2 );
          }
          break;

        case DrawBuffer.DRAW_POINTS:
          basic_effect.TextureEnabled = false;
          /*
          foreach (EffectPass pass in basic_effect.CurrentTechnique.Passes)
          {
            pass.Apply();

            device.DrawUserPrimitives<VertexPositionColorTexture>(
                PrimitiveType.PointList, vertices, 0, count );
          }
          */
          break;
      }

      /*
      glDisableClientState(GL_COLOR_ARRAY);

      if (draw_target) 
      {
        // flushes offscreen buffer drawing queue
        glBindFramebufferOES( GL_FRAMEBUFFER_OES, MAIN_BUFFER ); 
        glBindFramebufferOES( GL_FRAMEBUFFER_OES, draw_target->frame_buffer );
      }
      */

      reset();
    }
  }

  public class NativeApplication
  {
    static public void log( ClassApplication app, ClassString mesg )
    {
      Bard.log(mesg.data);
    }
  }

  public class NativeBitmap
  {
    static public bool load( ClassBitmap bitmap, String filename )
    {
      try
      {
        using (Texture2D texture = Texture2D.FromStream(
          Plasmacore.device, TitleContainer.OpenStream(filename)))
        {
          init( bitmap, texture );
          return true;
        }
      }
      catch (Exception)
      {
        return false;
      }
    }

    static public void init( ClassBitmap bitmap, ClassArrayList_of_Byte raw_data )
    {
      init( bitmap, raw_data.property_data );
    }

    static public void init( ClassBitmap bitmap, Texture2D texture )
    {
      int w = texture.Bounds.Width;
      int h = texture.Bounds.Height;
      Color[] colors = new Color[w*h];
      texture.GetData<Color>( colors );
      bitmap.init(w,h);
      int[] data = bitmap.property_data;
      int count = colors.Length;
      for (int i=0; i<count; ++i)
      {
        Color color = colors[i];
        data[i] = (color.A<<24) | (color.R<<16) | (color.G<<8) | color.B;
      }
    }

    static public void init( ClassBitmap bitmap, ClassString filename )
    {
      String name = filename.data;

      if (name.Equals("internal:font_system_17"))
      {
        init( bitmap, embedded_font_system_17 );
        return;
      }

      int slash_pos = name.LastIndexOf('/');
      int other_slash_pos = name.LastIndexOf('\\');
      if (slash_pos == -1 
        || (other_slash_pos != -1 && other_slash_pos < slash_pos))
      {
        slash_pos = other_slash_pos;
      }
      if (slash_pos != -1)
      {
        name = name.Substring( slash_pos + 1 );
      }

      bool success = true;

      if (name.EndsWith(".png") || name.EndsWith(".jpg") || name.EndsWith(".jpeg"))
      {
        success = load( bitmap, "images/"+name );
      }
      else
      {
        if ( !load( bitmap, "images/" + name + ".png" ) )
        {
          if ( !load( bitmap, "images/" + name + ".jpg" ) )
          {
            success = load( bitmap, "images/" + name + ".jpeg" );
          }
        }
      }

      if ( !success )
      {
        throw new ClassFileNotFoundError(filename);
      }
    }

    static public void init( ClassBitmap bitmap, byte[] bytes )
    {
      try
      {
        using (Texture2D texture = Texture2D.FromStream(
          Plasmacore.device, new MemoryStream(bytes)))
        {
          init( bitmap, texture );
        }
      }
      catch (Exception)
      {
        throw new ClassInvalidOperandError( Bard.str("bytes") );
      }
    }

    static public void copy_pixels_to( ClassBitmap src,
      int src_x, int src_y, int w, int h, 
      ClassBitmap dest, int dest_x, int dest_y, bool blend_alpha )
    {
      if (src == null || dest == null)
      {
        throw new ClassNullReferenceError();
      }

      int src_width = src.property_width;
      int dest_width = dest.property_width;

      int[] src_data = src.property_data;
      int[] dest_data = dest.property_data;

      int dest_skip_width = dest_width - w;
      int src_skip_width  = src_width - w;

      int src_i = src_y * src_width + src_x - 1;
      int dest_i = dest_y * dest_width + dest_x - 1;

      if (blend_alpha)
      {
        for (int j=h; j>0; --j)
        {
          for (int i=w; i>0; --i)
          {
            int bottom = dest_data[++dest_i];
            int top    = src_data[++src_i];
            int tr = (top >> 16) & 255;
            int tg = (top >> 8) & 255;
            int tb = (top & 255);
            int r = (bottom >> 16) & 255;
            int g = (bottom >> 8) & 255;
            int b = (bottom & 255);
            int inv_alpha = 255 - ((top >> 24) & 255);

            // we assume that tr, tg, and tb are premultiplied
            tr += ((r * inv_alpha) / 255);
            tg += ((g * inv_alpha) / 255);
            tb += ((b * inv_alpha) / 255);
            dest_data[dest_i] = ((255<<24) | (tr<<16) | (tg<<8) | tb);
          }
          dest_i += dest_skip_width;
          src_i  += src_skip_width;
        }
      }
      else
      {
        for (int j=h; j>0; --j)
        {
          for (int i=w; i>0; --i)
          {
            dest_data[++dest_i] = src_data[++src_i];
          }
          dest_i += dest_skip_width;
          src_i  += src_skip_width;
        }
      }
    }

    static byte[] embedded_font_system_17 =
    {
      137, 80, 78, 71, 13, 10, 26, 10,  0,  0,  0, 13, 73, 72, 68,
       82,  0,  0,  3, 87,  0,  0,  0, 17,  2,  3,  0,  0,  0, 76,
       69,190,143,  0,  0,  0,  9, 80, 76, 84, 69,255,255,255,  0,
        0,  0,255,255,255,126,239,143, 79,  0,  0,  0,  1,116, 82,
       78, 83,  0, 64,230,216,102,  0,  0,  4,171, 73, 68, 65, 84,
      120, 94,221,150,209,202,235, 40, 16,199,245, 66,223, 32,129,
      245,105,236, 69,123,109, 33, 83, 88,175,119, 23, 38, 79,147,
       15,210, 94,244, 90, 11,241, 41,247, 63,218,164, 95, 67, 15,
      203,129,133,179,172,165,196,232, 56,230, 55, 51,142,163,182,
      102,130, 25,212,175,109,132,127, 23,254, 93,157,118,234,243,
      175,165,210,236,149,114,211,135,137, 65,117,254,167,181, 13,
      134,  2, 13,253,236, 88, 19,214,227, 61,236,231, 21,105,242,
       20, 58,223,225,113,160,161,195,216,102, 97,136, 99,138, 32,
       39, 50,210, 61,202,131,  8, 50,230, 76,  3,148, 30,233, 60,
       96,  4, 18, 77,134, 58,170, 67, 68,171,176,194, 84,193,190,
      253, 44, 10, 87,223, 81,144,167, 78,202, 29,  6,121,217,181,
       61,172,  9,242,211,195, 32,253,108,111, 19,167,152, 29, 31,
       23,239,188, 73,118,126,195, 74,253,114, 96, 83,206,227,236,
      188, 43,126, 60, 20,  8,247,139,111, 22,134,159,103, 61,206,
      113, 57,102, 43, 50,144, 42,167,242, 53,206,204,118, 81,246,
       43, 39, 86,238,116,251,202,152,194,170, 83,149, 89, 92,196,
       80, 98,102,116,202, 52, 78,172,236,130,207,  0, 82,130,194,
      234, 49,134,246, 17, 61,110, 88, 73,225,101,215,220, 14,171,
      159,236,100,103,147,255, 64, 95,244,125,113,102,238, 31,167,
       59,176, 48,158, 52,133,205, 18, 38,185, 72, 34,147,147, 35,
       71,  7, 62, 80,146,207,241,228,187, 96,178,216, 69,231,196,
      241,178,156, 68,134,179,163,211,245,140, 14,155,188, 98, 93,
      110,215,156,235,170, 42,115,185,126,195, 58, 94, 38, 14,236,
       13, 27, 10, 45, 54,236, 85, 34,  3,239, 58,195,195,140,216,
      208,116,200,195,101, 18,215,250,230,245,163, 60,226,206,235,
       46,216,  9, 94, 41,105,197, 42, 11,147,137, 21,171,159,116,
      214,143,105,179,132,157,221, 24,159, 88, 12,103,102,128, 87,
      172,209,187,169,159, 59,  1, 21,172, 24, 27, 22,100,236, 36,
      160,172, 19,176, 30, 51,176,238,183,252, 13,107,176, 83,235,
      112,252,211, 69,108,200, 42,122,197,235,126,205,163,113, 65,
      144, 36, 61,166, 49,187,164, 11,182,192,170,114,193,148,120,
      189, 96, 57,166,150,166,103,245,122,221, 57,217,203, 92,177,
       12,157, 57,199,160, 37,  8, 59,239,208, 49, 35, 69,  4,104,
       71, 94, 60,235, 70, 98,115,249, 34,193, 34, 91, 66, 63,141,
       43, 86, 12,174,248,120,190,204, 60, 54,172,129, 69,230, 10,
      227,174, 88, 19,176,254,186, 45, 47, 44,246, 79,172,  7,247,
       51,176, 98,128, 68,216,176,204,102,158,120,204,213,235, 93,
      210,177, 98, 65,207, 58,117,186, 99,175, 72, 77,207,234,245,
       88, 13,106,142, 65,212,100,176,113,234, 17,147,246, 58,212,
      128,101, 91,178,120,210, 21, 96,185,  0,  4, 86,  0,152, 35,
       59, 68,218,228, 66,204,207, 32,100, 15,116,128,  7,126,  0,
      171,156,147, 98,200,216,  2,172,113,169, 88, 37, 84,172,123,
      206, 98,119,200,132, 24,190, 97,201,185,245, 12,219,237,176,
      134,230,128, 10, 81,207,214, 99,126,199,  2,113,112,254,137,
       85,191, 39,158, 89,176,216, 63, 83,  2,254,156, 77,238,175,
      167,123,170, 88,111,222,138,222,221, 61,235,118,164, 40,  2,
       11,148, 75, 95,234,234,230, 45,232,  7, 86,249,238,173, 71,
      253, 28,115, 89,189,117,  3, 22, 15, 98,229,208,207, 27,214,
      239,217,224,  7, 34, 59,111, 88, 53,192,170,121, 10,188,181,
       97,149,240,142, 37,121, 10, 88, 47,175,203, 89,144,108, 19,
      159, 88,  3,254,101, 33,118,227,229, 54, 55,172,122,182,234,
      245, 76,158, 43, 86,205, 11, 99,241,130,197,158,217,112,245,
       86, 59, 91,  8, 93, 30, 63,157, 45,139, 61,231, 87,202, 80,
      149,134,210, 19,139,179,200, 84, 34,147, 54, 44,247, 10,102,
      156,173, 31,122,107, 82,148,222,188,117,204,250,131,183,152,
      127,123,220, 42, 86,208, 92, 51,161,197,132, 26, 15,242, 49,
      158,173, 36, 55, 98, 98,193, 82, 53,203,201,217,106,153, 16,
      166, 96,138,241,149,  9, 27,150,201,182,142,144,195,204, 11,
       43, 87,172, 99,  9,220,208,175,129,149,206, 59,172, 36, 88,
      186, 97,117,217,187,131,228,207, 55, 44, 13, 44, 18, 61,103,
      206,117,228, 26,218,217, 82, 27,150,146,123,171,123, 92, 23,
      244,251,117,230,137,149, 85,191,  0, 75,174, 34,197, 46, 27,
       57, 44, 49,219, 84,177,218,189,  5,172, 88,128,181,221, 91,
      147, 30,129,101, 23, 83, 96,140,113,121,222, 91, 12, 77,232,
      141,176,123, 59, 82, 13,171, 76,192,226,111, 65,184,101, 66,
      157,170, 30, 87,128, 85,245, 92,128,213, 50, 33,176, 10, 14,
      102, 95,158,122,250,101,203,132,106, 11, 66, 37, 85,134, 30,
      207,132,190,157, 77,155,169, 15,  4,130, 50,164,200, 80,160,
      160, 72,174, 16, 79,170, 27, 16,161,146, 43,158, 85,134,148,
       31, 68,199,173,202,128,228, 64, 88,163,233, 32,157,181,202,
       80,134,107, 77, 97,130,168,172, 50,181,198,  1,209, 11,203,
       52, 61,157, 44,199, 38,  9,203,201,187, 85,143, 44,231,193,
      196, 99,208, 20, 90,209,210,246, 98,179,222, 91,187,154, 80,
      145,151,206, 86,101,224,209,168,127,216,220, 79,215,165,209,
      127, 26,229,160,200,  0, 75, 19,237,102, 52,165,138,250,182,
       14,146,110,175,167,203, 91,149,177,171,167, 94, 53, 96, 88,
       71,254,161,125, 46, 64,127,126,133,148, 69,118,169,153,107,
      143, 85,230,118, 69,255, 19,150,155, 43,131,157,192,241, 95,
      105, 20, 36,246, 62,123, 43,180,192,124, 19,255, 96,158,110,
      115,197,255,184,253, 13,230, 98,224, 33,186,187,109,174,  0,
        0,  0,  0, 73, 69, 78, 68,174, 66, 96,130
    };
  }

  public class NativeChooseMusic
  {
    static public bool need_to_choose_music( Object chooser )
    {
      return !MediaPlayer.GameHasControl;
    }

    static public void use_game_music( Object chooser, bool setting )
    {
      WP7Sound.music_enabled = setting;
      if (WP7Sound.music_enabled)
      {
        MediaPlayer.Stop();
      }
    }
  }

  public class NativeDisplay
  {
    static public void fullscreen( ClassDisplay display, bool setting )
    {
    }

    static public bool fullscreen( ClassDisplay display )
    {
      return true;
    }

    static public void native_set_clipping_region( ClassDisplay display, CompoundBox bounds )
    {
      DrawBuffer.render();
      int x = (int) bounds.property_position.property_x;
      int y = (int) bounds.property_position.property_y;
      int w = (int) bounds.property_size.property_x;
      int h = (int) bounds.property_size.property_y;
      Plasmacore.device.ScissorRectangle = new Rectangle(x,y,w,h);
    }

    static public CompoundVector2 native_scale_to_fit( ClassDisplay display, int width, int height )
    {
      Bard.log( "TODO: NativeDisplay::native_scale_to_fit()" );
      return new CompoundVector2(width,height);
    }
  }

  public class NativeInput
  {
    static public bool dummy_flag;

    static public void keyboard_visible( Object context, bool setting )
    {
      dummy_flag = setting;
    }

    static public bool keyboard_visible( Object context )
    {
      return dummy_flag;
    }
  }

  public class NativeLineManager
  {
    static public void draw( Object context, CompoundLine line, CompoundColor color,
      CompoundRender render_flags )
    {
      DrawBuffer.set_render_flags( render_flags.property_flags,
        DrawBuffer.src_blend, DrawBuffer.dest_blend );
      DrawBuffer.set_line_mode();

      Vector3 pt1 = Plasmacore.v3( line.property_pt1 );
      Vector3 pt2 = Plasmacore.v3( line.property_pt2 );

      NativeTransformManager.update();
      pt1 = NativeTransformManager.transform.transform( pt1 );
      pt2 = NativeTransformManager.transform.transform( pt2 );

      int argb = color.property_argb;
      Color c = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);

      VertexPositionColorTexture v1, v2, v3, v4;
      Vector2 uv = new Vector2(0,0);
      v1 = new VertexPositionColorTexture( pt1, c, uv );
      v2 = new VertexPositionColorTexture( pt2, c, uv );

      DrawBuffer.add( v1, v2 );
    }
  }

  public class NativeNativeData
  {
    static public void clean_up( NativeData context )
    {
      context.release();
    }
  }

  public class NativeProcess
  {
    static public void init( Object context, ClassString cmd )
    {
    }

    static public void update( Object context )
    {
    }

    static public void release( Object context )
    {
    }
  }

  public class NativeQuadManager
  {
    static public void fill( ClassQuadManager obj, CompoundQuad quad, 
      CompoundColorGradient colors, CompoundRender render_flags )
    {
      DrawBuffer.set_render_flags( render_flags.property_flags,
        DrawBuffer.BLEND_SRC_ALPHA, DrawBuffer.BLEND_INVERSE_SRC_ALPHA );
      DrawBuffer.set_solid_triangle_mode();

      Vector3 pt1 = Plasmacore.v3( quad.property_top_left );
      Vector3 pt2 = Plasmacore.v3( quad.property_top_right );
      Vector3 pt3 = Plasmacore.v3( quad.property_bottom_right );
      Vector3 pt4 = Plasmacore.v3( quad.property_bottom_left );

      NativeTransformManager.update();
      pt1 = NativeTransformManager.transform.transform( pt1 );
      pt2 = NativeTransformManager.transform.transform( pt2 );
      pt3 = NativeTransformManager.transform.transform( pt3 );
      pt4 = NativeTransformManager.transform.transform( pt4 );

      int argb = colors.property_top_left.property_argb;
      Color c1 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);
      argb = colors.property_top_right.property_argb;
      Color c2 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);
      argb = colors.property_bottom_right.property_argb;
      Color c3 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);
      argb = colors.property_bottom_left.property_argb;
      Color c4 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);

      Vector2 zero_uv = new Vector2(0,0);
      VertexPositionColorTexture v1, v2, v3, v4;
      v1 = new VertexPositionColorTexture( pt1, c1, zero_uv );
      v2 = new VertexPositionColorTexture( pt2, c2, zero_uv );
      v3 = new VertexPositionColorTexture( pt3, c3, zero_uv );
      v4 = new VertexPositionColorTexture( pt4, c4, zero_uv );

      DrawBuffer.add( v1, v2, v4 );
      DrawBuffer.add( v4, v2, v3 );
    }
  }

  public class NativeResourceManager
  {
    static public ClassString load_data_file( ClassResourceManager manager, ClassString filename )
    {
      try
      {
        String cs_filename = filename.data;

        int slash_pos = cs_filename.LastIndexOf('/');
        int other_slash_pos = cs_filename.LastIndexOf('\\');
        if (slash_pos == -1 
          || (other_slash_pos != -1 && other_slash_pos < slash_pos))
        {
          slash_pos = other_slash_pos;
        }
        if (slash_pos != -1)
        {
          cs_filename = cs_filename.Substring( slash_pos + 1 );
        }
        cs_filename = "data/" + cs_filename;

        if (!cs_filename.StartsWith("data/")) cs_filename = "data/" + cs_filename;
        Stream infile = TitleContainer.OpenStream(cs_filename);

        int count = (int) infile.Length;
        StringBuilder buffer = new StringBuilder(count);

        for (int i=0; i<count; ++i)
        {
          buffer.Append( (char) infile.ReadByte() );
        }

        infile.Close();

        return new ClassString( buffer.ToString() );
      }
      catch (Exception)
      {
        throw new ClassFileNotFoundError(filename);
      }
    }

    static public ClassString load_gamestate( ClassResourceManager manager, ClassString filename )
    {
#if WINDOWS
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForDomain();
#else
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForApplication();
#endif          
      try
      {
        IsolatedStorageFileStream infile;
        infile = storage.OpenFile(filename.data, System.IO.FileMode.Open);

        int count = (int)infile.Length;
        StringBuilder buffer = new StringBuilder(count);

        for (int i = 0; i < count; ++i)
        {
          buffer.Append( (char)infile.ReadByte() );
        }

        infile.Close();

        return new ClassString( buffer.ToString() );
      }
      catch (Exception)
      {
        throw new ClassFileNotFoundError(filename);
      }
    }

    static public void save_gamestate( ClassResourceManager manager, ClassString filename,
      ClassString content )
    {
#if WINDOWS
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForDomain();
#else
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForApplication();
#endif    

      IsolatedStorageFileStream outfile = null;
      outfile = storage.OpenFile( filename.data, System.IO.FileMode.Create );

      String data = content.data;
      int count = data.Length;
      for (int i=0; i<count; ++i)
      {
        outfile.WriteByte( (byte) data[i] );
      }
      outfile.Close();
    }

    static public void delete_gamestate( ClassResourceManager manager, ClassString filename )
    {
#if WINDOWS
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForDomain();
#else
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForApplication();
#endif    
      try
      {
        storage.DeleteFile(filename.data);
      }
      catch (Exception err)
      {
      }
    }
  }

  abstract public class WP7Sound : NativeData
  {
    static public bool music_enabled = true;

    abstract public bool   is_playing();
    abstract public double get_duration();
    abstract public double get_current_time();

    abstract public void set_current_time( double new_time );
    abstract public void set_repeats( bool setting );
    abstract public void set_volume( double v );
    abstract public void set_pitch( double p );
    abstract public void set_pan( double p );

    abstract public void play();
    abstract public void pause();
    //abstract public void stop();
  }

  public class WP7Song : WP7Sound
  {
    static WP7Song currently_playing;

    public Song song;
    public bool repeats;

    public WP7Song( Song song )
    {
      this.song = song;
    }

    override public void release()
    {
      if (song != null)
      {
        if (currently_playing == this)
        {
          if (MediaPlayer.State != MediaState.Playing) return;
          MediaPlayer.Stop();
        }
        song = null;
      }
    }

    public override bool is_playing()
    {
      return (currently_playing == this && MediaPlayer.State == MediaState.Playing);
    }

    public bool is_paused()
    {
      return (currently_playing == this && MediaPlayer.State == MediaState.Paused);
    }

    public override double get_duration()
    {
      return song.Duration.TotalSeconds;
    }

    public override double get_current_time()
    {
      return MediaPlayer.PlayPosition.TotalSeconds;
    }

    override public void set_current_time( double new_time )
    {
      if (new_time == 0.0 && is_paused()) MediaPlayer.Stop();
    }

    public override void set_repeats( bool setting )
    { 
      repeats = setting;
      if (is_playing())
      {
        MediaPlayer.IsRepeating = repeats;
      }
    }

    public override void set_volume( double v ) { }
    public override void set_pitch( double p ) { }
    public override void set_pan( double p ) { }

    public override void play()
    {
      if ( !music_enabled ) return;

      if (currently_playing == this)
      {
        if (MediaPlayer.State == MediaState.Playing) return;
        if (MediaPlayer.State == MediaState.Paused)
        {
          MediaPlayer.Resume();
          return;
        }
      }

      try
      {
        MediaPlayer.Play(song);
        currently_playing = this;
        if (is_playing())
        {
          MediaPlayer.IsRepeating = repeats;
        }
      }
      catch (Exception)
      {
        // is probably plugged in on USB
      }
    }

    public override void pause()
    {
      if (currently_playing == this)
      {
        if (MediaPlayer.State != MediaState.Playing) return;
        MediaPlayer.Pause();
      }
    }
  }

  public class WP7SoundEffect : WP7Sound
  {
    public SoundEffect effect;
    public float volume=1.0f, pitch=0.0f, pan=0.0f;

    public WP7SoundEffect( SoundEffect effect )
    {
      this.effect = effect;
    }

    override public void release()
    {
      effect = null;
    }

    public override bool is_playing()
    {
      return false;  // no way to tell
    }

    public override double get_duration()
    {
      return effect.Duration.TotalSeconds;
    }

    public override double get_current_time()
    {
      return 0.0;
    }

    public override void set_current_time( double new_time )
    {
    }

    public override void set_repeats( bool setting ) { }
    public override void set_volume( double v ) { volume = (float) v; }
    public override void set_pitch( double p ) { pitch = (float) p; }
    public override void set_pan( double p ) { pan = (float) p; }

    public override void play()
    {
      effect.Play(volume,pitch,pan);
    }

    public override void pause()
    {
      // can't pause
    }
  }

  public class NativeNativeSound
  {
    static public void init( ClassNativeSound sound, ClassString filename )
    {
      String fname = filename.data;
      if (fname.EndsWith(".wav") || fname.EndsWith(".mp3") || fname.EndsWith(".m4a"))
      {
        fname = fname.Substring( 0, fname.Length - 4);
      }
      

      WP7Sound sound_data = null;
      try
      {
        sound_data = new WP7SoundEffect( 
          Plasmacore.content.Load<SoundEffect>(fname) );
      }
      catch (Exception)
      {
        try
        {
          sound_data = new WP7Song( Plasmacore.content.Load<Song>(fname) );
        }
        catch (Exception)
        {
          Bard.log( "Sound not found: " + filename.data );
          return;
        }
      }
      sound.property_native_data = sound_data;
    }

    static public void init( ClassNativeSound sound, ClassArrayList_of_Byte raw_bytes )
    {
      Bard.log("TODO: NativeNativeSound::init(raw_bytes)");
    }

    static public ClassNativeSound create_duplicate( ClassNativeSound sound )
    {
      return sound;
    }

    static public void play( ClassNativeSound sound )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) sound_data.play();
    }

    static public void pause( ClassNativeSound sound )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) sound_data.pause();
    }

    static public bool is_playing( ClassNativeSound sound )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) return sound_data.is_playing();
      return false;
    }

    static public void volume( ClassNativeSound sound, double new_volume )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) sound_data.set_volume( new_volume );
    }

    static public void pan( ClassNativeSound sound, double new_pan )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) sound_data.set_pan( new_pan );
    }

    static public void pitch( ClassNativeSound sound, double new_pitch )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) sound_data.set_pitch( new_pitch - 1.0 );
    }

    static public void repeats( ClassNativeSound sound, bool setting )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) sound_data.set_repeats( setting );
    }

    static public void current_time( ClassNativeSound sound, double new_time )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) sound_data.set_current_time( new_time );
    }

    static public double current_time( ClassNativeSound sound )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) return sound_data.get_current_time();
      return 0.0;
    }

    static public double duration( ClassNativeSound sound )
    {
      WP7Sound sound_data = sound.property_native_data as WP7Sound;
      if (sound_data != null) return sound_data.get_duration();
      return 0.0;
    }
  }

  public class NativeTextureData : NativeData
  {
    public Texture2D texture;

    public NativeTextureData( Texture2D texture )
    {
      this.texture = texture;
    }

    override public void release()
    {
      texture.Dispose();
      texture = null;
    }
  }

  public class NativeTexture
  {
    static public void init( ClassTexture texture, ClassBitmap bitmap, int pixel_format )
    {
      int[] data = bitmap.property_data;
      int count = data.Length;

      int w = bitmap.property_width;
      int h = bitmap.property_height;
      int wp2 = 1;
      while (wp2 < w) wp2 <<= 1;
      int hp2 = 1;
      while (hp2 < h) hp2 <<= 1;

      int dest_skip = wp2 - w;

      Color[] colors = new Color[wp2*hp2];
      int src_pos = -1;
      int dest_pos = -1;
      for (int j=h; j>0; --j)
      {
        for (int i=w; i>0; --i)
        {
          // premultiply color components by alpha component
          int c = data[++src_pos];
          int a = (c >> 24) & 255;
          int r = (c >> 16) & 255;
          int g = (c >>  8) & 255;
          int b = c & 255;
          colors[++dest_pos] = new Color( (r*a)/255, (g*a)/255, (b*a)/255, a );
        }
        dest_pos += dest_skip;
      }

      Texture2D tex = new Texture2D( Plasmacore.device, wp2, hp2, false, SurfaceFormat.Color );
      tex.SetData<Color>( colors );

      texture.property_native_data = new NativeTextureData(tex);
      texture.property_image_size.property_x = w;
      texture.property_image_size.property_y = h;
      texture.property_texture_size.property_x = wp2;
      texture.property_texture_size.property_y = hp2;
    }

    static public void init( ClassTexture texture, CompoundVector2 size )
    {
      // TODO: implement for offscreen buffers
      /*
      int w = (int) size.property_x;
      int h = (int) size.property_y;
      int wp2 = 1;
      while (wp2 < w) wp2 <<= 1;
      int hp2 = 1;
      while (hp2 < h) hp2 <<= 1;

      Texture2D tex = new Texture2D( Plasmacore.device, wp2, hp2, false, SurfaceFormat.Color );

      texture.property_native_data = new NativeTextureData(tex);
      texture.property_image_size.property_x = w;
      texture.property_image_size.property_y = h;
      texture.property_texture_size.property_x = wp2;
      texture.property_texture_size.property_y = hp2;
      */
    }


    static public void init( ClassTexture texture, CompoundVector2 size, int pixel_format )
    {
      int w = (int) size.property_x;
      int h = (int) size.property_y;
      int wp2 = 1;
      while (wp2 < w) wp2 <<= 1;
      int hp2 = 1;
      while (hp2 < h) hp2 <<= 1;

      Texture2D tex = new Texture2D( Plasmacore.device, wp2, hp2, false, SurfaceFormat.Color );

      texture.property_native_data = new NativeTextureData(tex);
      texture.property_image_size.property_x = w;
      texture.property_image_size.property_y = h;
      texture.property_texture_size.property_x = wp2;
      texture.property_texture_size.property_y = hp2;
    }

    static public void native_release( ClassTexture texture )
    {
      if (texture.property_native_data != null)
      {
        texture.property_native_data.release();
        texture.property_native_data = null;
      }
    }

    static public void set( ClassTexture texture, ClassBitmap bitmap, CompoundVector2 pos )
    {
      int[] data = bitmap.property_data;
      int count = data.Length;

      int w = bitmap.property_width;
      int h = bitmap.property_height;

      Color[] colors = new Color[w*h];
      int src_pos = -1;
      int dest_pos = -1;
      for (int j=h; j>0; --j)
      {
        for (int i=w; i>0; --i)
        {
          // premultiply color components by alpha component
          int c = data[++src_pos];
          int a = (c >> 24) & 255;
          int r = (c >> 16) & 255;
          int g = (c >>  8) & 255;
          int b = c & 255;
          colors[++dest_pos] = new Color( (r*a)/255, (g*a)/255, (b*a)/255, a );
        }
      }

      Texture2D tex = ((NativeTextureData) texture.property_native_data).texture;
      tex.SetData<Color>( 0, new Rectangle((int)pos.property_x,(int)pos.property_y,w,h), colors, 0, w*h );      
    }

    static public void draw( ClassTexture texture, CompoundCorners uv, CompoundVector2 size,
      CompoundColor color, CompoundRender render_flags, CompoundBlend blend_fn )
    {
      DrawBuffer.set_render_flags( render_flags.property_flags,
        blend_fn.property_src_blend, blend_fn.property_dest_blend );
      DrawBuffer.set_textured_triangle_mode( texture, null );

      Vector3 pt1 = new Vector3(                       0, 0, 0 );
      Vector3 pt2 = new Vector3( (float) size.property_x, 0, 0 );
      Vector3 pt3 = new Vector3( (float) size.property_x, (float) size.property_y, 0 );
      Vector3 pt4 = new Vector3(                       0, (float) size.property_y, 0 );

      NativeTransformManager.update();
      pt1 = NativeTransformManager.transform.transform( pt1 );
      pt2 = NativeTransformManager.transform.transform( pt2 );
      pt3 = NativeTransformManager.transform.transform( pt3 );
      pt4 = NativeTransformManager.transform.transform( pt4 );

      Color c;
      //if ((render_flags.property_flags & DrawBuffer.RENDER_FLAG_FIXED_COLOR) != 0)
      //{
        //c = Color.White;
      //}
      //else
      //{
      int argb = color.property_argb;
      c = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);
      //}

      float uv_a_x = (float) uv.property_top_left.property_x;
      float uv_a_y = (float) uv.property_top_left.property_y;
      float uv_b_x = (float) uv.property_bottom_right.property_x;
      float uv_b_y = (float) uv.property_bottom_right.property_y;

      VertexPositionColorTexture v1, v2, v3, v4;
      v1 = new VertexPositionColorTexture( pt1, c, new Vector2( uv_a_x, uv_a_y ) );
      v2 = new VertexPositionColorTexture( pt2, c, new Vector2( uv_b_x, uv_a_y ) );
      v3 = new VertexPositionColorTexture( pt3, c, new Vector2( uv_b_x, uv_b_y ) );
      v4 = new VertexPositionColorTexture( pt4, c, new Vector2( uv_a_x, uv_b_y ) );

      DrawBuffer.add( v1, v2, v4 );
      DrawBuffer.add( v4, v2, v3 );
    }

    static public void draw( ClassTexture texture, CompoundCorners uv, CompoundQuad vertices,
      CompoundColorGradient colors, CompoundRender render_flags, CompoundBlend blend_fn )
    {
      DrawBuffer.set_render_flags( render_flags.property_flags,
        blend_fn.property_src_blend, blend_fn.property_dest_blend );
      DrawBuffer.set_textured_triangle_mode( texture, null );

      Vector3 pt1 = Plasmacore.v3( vertices.property_top_left );
      Vector3 pt2 = Plasmacore.v3( vertices.property_top_right );
      Vector3 pt3 = Plasmacore.v3( vertices.property_bottom_right );
      Vector3 pt4 = Plasmacore.v3( vertices.property_bottom_left );

      NativeTransformManager.update();
      pt1 = NativeTransformManager.transform.transform( pt1 );
      pt2 = NativeTransformManager.transform.transform( pt2 );
      pt3 = NativeTransformManager.transform.transform( pt3 );
      pt4 = NativeTransformManager.transform.transform( pt4 );

      int argb = colors.property_top_left.property_argb;
      Color c1 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);
      argb = colors.property_top_right.property_argb;
      Color c2 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);
      argb = colors.property_bottom_right.property_argb;
      Color c3 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);
      argb = colors.property_bottom_left.property_argb;
      Color c4 = new Color((argb>>16)&255,(argb>>8)&255,argb&255,(argb>>24)&255);

      float uv_a_x = (float) uv.property_top_left.property_x;
      float uv_a_y = (float) uv.property_top_left.property_y;
      float uv_b_x = (float) uv.property_bottom_right.property_x;
      float uv_b_y = (float) uv.property_bottom_right.property_y;

      VertexPositionColorTexture v1, v2, v3, v4;
      v1 = new VertexPositionColorTexture( pt1, c1, new Vector2( uv_a_x, uv_a_y ) );
      v2 = new VertexPositionColorTexture( pt2, c2, new Vector2( uv_b_x, uv_a_y ) );
      v3 = new VertexPositionColorTexture( pt3, c3, new Vector2( uv_b_x, uv_b_y ) );
      v4 = new VertexPositionColorTexture( pt4, c4, new Vector2( uv_a_x, uv_b_y ) );

      DrawBuffer.add( v1, v2, v4 );
      DrawBuffer.add( v4, v2, v3 );
    }

    static public void draw( ClassTexture texture, CompoundCorners uv, CompoundVector2 size,
      CompoundColor color, CompoundRender render_flags, CompoundBlend blend_fn,
      ClassTexture alpha_texture, CompoundCorners alpha_uv )
    {
      Bard.log("TODO: NativeTexture::draw()");
    }

    static public void draw_tile( ClassTexture texture, CompoundCorners uv, 
      CompoundVector2 pos, CompoundVector2 size, CompoundRender render_flags )
    {
      draw_tile( texture, uv, pos, size, render_flags.property_flags );
    }

    static public void draw_tile( ClassTexture texture, CompoundCorners uv, 
      CompoundVector2 pos, CompoundVector2 size, int render_flags )
    {
      Bard.log("TODO: NativeTexture::draw_tile()");
    }

  }

  public class NativeTransformManager
  {
    public const int TRANSFORM_STACK_SIZE = 32;

    static public Matrix2x3 transform;
    static public Matrix2x3 camera_transform;
    static public Matrix2x3 object_transform;

    static public Matrix2x3[] camera_transform_stack = new Matrix2x3[TRANSFORM_STACK_SIZE];
    static public Matrix2x3[] object_transform_stack = new Matrix2x3[TRANSFORM_STACK_SIZE];
    static public int  camera_transform_stack_count;
    static public int  object_transform_stack_count;
    static public bool camera_transform_stack_modified;
    static public bool object_transform_stack_modified;

    static public void reset()
    {
      camera_transform_stack_count = 0;
      object_transform_stack_count = 0;
      camera_transform_stack_modified = true;
      object_transform_stack_modified = true;
    }

    static public void update()
    {
      bool modified = false;

      int camera_count = camera_transform_stack_count;
      if (camera_transform_stack_modified)
      {
        modified = true;
        camera_transform_stack_modified = false;

        if (camera_count > 0)
        {
          int mpos = 0;
          camera_transform = camera_transform_stack[0];

          while (--camera_count > 0)
          {
            camera_transform = camera_transform.times(camera_transform_stack[++mpos]);
          }
        }
        else
        {
          camera_transform = new Matrix2x3(1,0,0,0,1,0);
        }
      }

      int  object_count = object_transform_stack_count;
      if (object_transform_stack_modified)
      {
        modified = true;
        object_transform_stack_modified = false;

        if (object_count > 0)
        {
          int mpos = object_count;
          object_transform = object_transform_stack[--mpos];

          while (--object_count > 0)
          {
            object_transform = object_transform.times( object_transform_stack[--mpos] );
          }
        }
        else
        {
          object_transform = new Matrix2x3(1,0,0,0,1,0);
        }
      }

      if (modified) transform = camera_transform.times( object_transform );
    }

    static public CompoundTransform current( ClassTransformManager obj )
    {
      update();
      return new CompoundTransform(
        new CompoundTransformRow(transform.r1c1,transform.r1c2,transform.r1c3),
        new CompoundTransformRow(transform.r2c1,transform.r2c2,transform.r2c3)
      );
    }

    static public void push_camera_transform( ClassTransformManager obj,
      CompoundTransform transform )
    {
      if (camera_transform_stack_count < TRANSFORM_STACK_SIZE)
      {
        CompoundTransformRow r0 = transform.property_r0;
        CompoundTransformRow r1 = transform.property_r1;
        camera_transform_stack[camera_transform_stack_count++] = 
          new Matrix2x3( r0.property_c0, r0.property_c1, r0.property_c2,
                         r1.property_c0, r1.property_c1, r1.property_c2 );
        camera_transform_stack_modified = true;
      }
    }

    static public void pop_camera_transform( ClassTransformManager obj )
    {
      if (--camera_transform_stack_count < 0) camera_transform_stack_count = 0;
      camera_transform_stack_modified = true;
    }

    static public void push_object_transform( ClassTransformManager obj,
      CompoundTransform transform )
    {
      if (object_transform_stack_count < TRANSFORM_STACK_SIZE)
      {
        CompoundTransformRow r0 = transform.property_r0;
        CompoundTransformRow r1 = transform.property_r1;
        object_transform_stack[object_transform_stack_count++] = 
          new Matrix2x3( r0.property_c0, r0.property_c1, r0.property_c2,
                         r1.property_c0, r1.property_c1, r1.property_c2 );
        object_transform_stack_modified = true;
      }
    }

    static public void pop_object_transform( ClassTransformManager obj )
    {
      if (--object_transform_stack_count < 0) object_transform_stack_count = 0;
      object_transform_stack_modified = true;
    }

    static public CompoundTransform create_from( ClassTransformManager obj,
      CompoundVector2 size, CompoundVector2 handle,
      CompoundRadians angle, CompoundVector2 scale, 
      CompoundVector2 position,
      bool hflip, bool vflip )
    {
      double handle_x = handle.property_x;
      double handle_y = handle.property_y;
      double size_x = size.property_x;
      double size_y = size.property_y;
      double scale_x = scale.property_x;
      double scale_y = scale.property_y;

      if (hflip || vflip)
      {
        handle_x -= size_x / 2.0;
        handle_y -= size_y / 2.0;
      }

      double cost = Math.Cos(angle.property_value);
      double sint = Math.Sin(angle.property_value);

      double r1c1 = cost*scale_x;
      double r1c2 = -sint*scale_y;
      double r1c3 = position.property_x - scale_x*handle_x*cost + sint*scale_y*handle_y;

      double r2c1 = sint*scale_x;
      double r2c2 = cost*scale_y;
      double r2c3 = position.property_y - scale_x*handle_x*sint - cost*scale_y*handle_y;

      Matrix2x3 m = new Matrix2x3(r1c1,r1c2,r1c3,r2c1,r2c2,r2c3);
      if (hflip || vflip)
      {
        if (hflip)
        {
          if (vflip)
          {
            m = m.times( new Matrix2x3(-1,0,0,0,-1,0) );
          }
          else
          {
            m = m.times( new Matrix2x3(-1,0,0,0,1,0) );
          }
        }
        else
        {
          m = m.times( new Matrix2x3(1,0,0,0,-1,0) );
        }

        // translate by -size/2
        m = m.times( new Matrix2x3(1,0,-size_x/2.0,0,1,-size_y/2.0) );
      }

      return new CompoundTransform(
        new CompoundTransformRow(m.r1c1,m.r1c2,m.r1c3),
        new CompoundTransformRow(m.r2c1,m.r2c2,m.r2c3)
      );
    }
  }

  public class NativeWindowsPhone7
  {
    static public String default_value;

    static public bool native_is_trial( ClassObject obj )
    {
      return (new Microsoft.Phone.Marketplace.LicenseInformation().IsTrial());
    }

    static public void prompt( ClassObject context, ClassString title, ClassString description,
        ClassString default_value )
    {
      NativeWindowsPhone7.default_value = default_value.data;
      Guide.BeginShowKeyboardInput( PlayerIndex.One, title.data, description.data,
        (default_value.data == null) ? "" : default_value.data, on_keyboard_finished, null );
    }

    static public void on_keyboard_finished( IAsyncResult result )
    {
      String st = Guide.EndShowKeyboardInput( result );
      if (st != null)
      {
        // Calculate the difference between the original default string
        // and the resulting string; e.g. "ABCD" -> "ABXY" results in
        // (delete) (delete) X Y
        if (default_value != null)
        {
          while (default_value.Length > 0)
          {
            if (st.Length > 0 && st[0] == default_value[0])
            {
              // Remove unchanged character from both strings.
              st = st.Substring(1);
              default_value = default_value.Substring(1);
            }
            else
            {
              // Send a delete for all remaining characters
              for (int i=0; i<default_value.Length; ++i)
              {
                Plasmacore.on_key( true, 8, true );
                Plasmacore.on_key( false, 8, true );
              }
              break;
            }
          }
        }

        for (int i=0; i<st.Length; ++i)
        {
          char ch = st[i];
          Plasmacore.on_key( true, ch, true );
          Plasmacore.on_key( false, ch, true );
        }
      }
      else
      {
        // Canceled; send a delete for every character in the original result.
        if (default_value != null)
        {
          for (int i=0; i<default_value.Length; ++i)
          {
            Plasmacore.on_key( true, 8, true );
            Plasmacore.on_key( false, 8, true );
          }
        }
      }
      Plasmacore.on_key( true, 10, true );
      Plasmacore.on_key( false, 10, true );
    }

    static public void web_request( Object context, ClassString url, 
      AspectDataListener listener )
    {
      WebRequest request = WebRequest.Create(url.data); 
      request.BeginGetResponse( web_response_handler, new WebRequestInfo(request,listener) );
    }

    static public void web_response_handler( IAsyncResult result )
    {
      WebRequestInfo info = (WebRequestInfo) result.AsyncState;
      try
      {
        if (info.listener != null)
        {
          Stream stream = (info.request.EndGetResponse(result)).GetResponseStream();
          int count = (int) stream.Length;
          byte[] data = new byte[ count ];
          stream.Read( data, 0, count );
          stream.Close();
          info.listener.on( new ClassArrayList_of_Byte( data, false ) );
        }
      }
      catch (Exception)
      {
        if (info.listener != null)
        {
          info.listener.on( null );
        }
      }
    }
  }

  public class WebRequestInfo
  {
    public WebRequest   request;
    public AspectDataListener listener;

    public WebRequestInfo( WebRequest request, AspectDataListener listener )
    {
      this.request = request;
      this.listener = listener;
    }
  }
}

