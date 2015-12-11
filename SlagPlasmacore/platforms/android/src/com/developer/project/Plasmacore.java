//=============================================================================
// Plasmacore.java
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Primary Java-side Plasmacore implementation.
//
// ----------------------------------------------------------------------------
//
// $(COPYRIGHT)
//
//   http://plasmaworks.com/plasmacore
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
// ----------------------------------------------------------------------------
//
// Adapted from the following Google sample code:
//
//   http://code.google.com/p/apps-for-android/source/browse/trunk/
//     Samples/OpenGLES/Triangle/src/com/google/android/opengles/triangle/
//     GLView.java?r=25
//
//=============================================================================
package com.$(DEVELOPER_PACKAGE_ID).$(PROJECT_PACKAGE_ID);

import java.io.*;
import java.lang.reflect.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import java.security.*;

import android.app.*;
import android.content.*;
import android.content.res.*;
import android.graphics.*;
import android.hardware.*;
import android.media.*;
import android.net.*;
import android.net.wifi.*;
import android.os.*;
import android.os.PowerManager.*;
import android.opengl.*;
import android.telephony.*;
import android.util.*;
import android.view.*;
import android.view.inputmethod.*;
import android.widget.*;
import android.webkit.*;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import javax.microedition.khronos.egl.*;
import javax.microedition.khronos.opengles.*;

public class Plasmacore implements SensorEventListener
{
  static
  {
    System.loadLibrary( "$(PROJECT_PACKAGE_ID)" );
  }

  final static public String LOGTAG = "Plasmacore";
  static Activity   activity;
  static Plasmacore singleton;

  final static public int res_general = 0;
  final static public int res_data    = 1;
  final static public int res_image   = 2;
  final static public int res_sound   = 3;

  boolean created_vm;
  public String execution_lock = new String();

  // PROPERTIES
  String project_id, key_filename;
  ArrayList<String> assets = new ArrayList<String>();

  PowerManager.WakeLock wake_lock;
  java.util.Timer       release_wake_timer;

  volatile boolean pausing;
  volatile boolean paused;

  ViewGroup layout;
  GLSurfaceView view;

  TouchManager touch_manager = new TouchManager();
  boolean showing_keyboard=false;
  boolean hard_keyboard_hidden=true;
  double acceleration_x;
  double acceleration_y;
  double acceleration_z;

  boolean fast_hardware = true;

  byte[] io_buffer;
  ResourceBank<FileInputStream>  infile_bank  = new ResourceBank<FileInputStream>();
  ResourceBank<FileOutputStream> outfile_bank = new ResourceBank<FileOutputStream>();
  ResourceBank<AndroidSound>     sound_bank = new ResourceBank<AndroidSound>();
  ResourceBank<PCWebView>        web_view_bank = new ResourceBank<PCWebView>();

  // introspection
  Method m_getX;
  Method m_getY;
  Method m_getPointerCount;
  Method m_getPointerId;

  Handler alert_handler = new Handler()
  {
    public void handleMessage( Message m ) { alert( (String) m.obj ); }
  };

  Handler new_web_view_handler = new Handler()
  {
    public void handleMessage( Message m ) 
    {
      synchronized (web_view_bank)
      {
        PCWebView view = new PCWebView(Plasmacore.activity);
        view.getSettings().setJavaScriptEnabled(true);
        web_view_bank.set( m.what, view );

        view.setWebViewClient(
          new WebViewClient()
          {
            public void onPageFinished( WebView view, String url )
            {
              PCWebView web_view = (PCWebView) view;
              web_view.loaded = true;
              web_view.set_visible( true );
            }

            public void onReceivedError( WebView view, int error_code, String descr, String url )
            {
              ((PCWebView) view).failed = true;
            }

            public boolean shouldOverrideUrlLoading( WebView view, String url )
            {
              if (url.startsWith("market:"))
              {
                Intent intent = new Intent( Intent.ACTION_VIEW, Uri.parse(url) );
                activity.startActivity(intent); 
                return true;
              }
              return false;
            }
          } );
      }
    }
  };

  Handler web_view_url_handler = new Handler()
  {
    public void handleMessage( Message m ) 
    {
      synchronized (web_view_bank)
      {
        PCWebView view = web_view_bank.get(m.what);
        if (view == null) return;
        view.loadUrl( (String) m.obj );
      }
    }
  };

  Handler web_view_html_handler = new Handler()
  {
    public void handleMessage( Message m ) 
    {
      synchronized (web_view_bank)
      {
        PCWebView view = web_view_bank.get(m.what);
        if (view == null) return;
        view.loadData( (String) m.obj , "text/html", "utf-8" );
      }
    }
  };

  Handler web_view_close_handler = new Handler()
  {
    public void handleMessage( Message m ) 
    {
      synchronized (web_view_bank)
      {
        PCWebView view = web_view_bank.get(m.what);
        if (view == null) return;
        view.set_visible(false);
        view.closed = true;
        web_view_bank.release(m.what);
      }
    }
  };

  Handler web_view_set_bounds_handler = new Handler()
  {
    public void handleMessage( Message m ) 
    {
      synchronized (web_view_bank)
      {
        PCWebView view = web_view_bank.get(m.what);
        if (view == null) return;
        view.x = (m.arg1 >>> 16);
        view.y = (m.arg1 & 0xffff);
        view.width  = (m.arg2 >>> 16);
        view.height = (m.arg2 & 0xffff);
      }
    }
  };

  Handler web_view_set_visible_handler = new Handler()
  {
    public void handleMessage( Message m ) 
    {
      synchronized (web_view_bank)
      {
        PCWebView view = web_view_bank.get(m.what);
        if (view == null) return;
        view.set_visible( m.arg1 != 0 );
      }
    }
  };

  Handler web_view_resume_handler = new Handler()
  {
    public void handleMessage( Message m ) 
    {
      ((PCWebView) m.obj).set_visible( true );
    }
  };

  // METHODS
  public Plasmacore( Activity activity )
  {
    this.activity = activity;

    if (jniAndroidMemoryClass() == 16) fast_hardware = false;
  }

  public void launch( int screen_width, int screen_height )
  {
    if (created_vm) return;
    created_vm = true;

    byte[] etc_bytes = jniLoadResource(0,"game.etc");
    boolean result;
    synchronized (execution_lock)
    {
      result = bardCreate( screen_width, screen_height, etc_bytes );
    }
    if ( !result ) activity.finish();
  }

  public void onTexturesLost()
  {
    if (created_vm) 
    {
      synchronized (execution_lock)
      {
        bardTexturesLostEvent();
      }
    }
  }

  public int tick()
  {
    if (pausing)
    {
      if ( !paused )
      {
        paused = true;
        synchronized (execution_lock)
        {
          bardOnPause();
        }
      }
      return 1;
    }

    raisePendingSignals();

    // synchronized to avoid conflict with event dispatches
    // dispatch current acceleration
    synchronized (execution_lock)
    {
      bardAccelerationEvent( -acceleration_x, acceleration_y, -acceleration_z );
System.out.println( "drawing" );
      return bardUpdateDrawEvent();
    }
  }

  public void onCreate( Bundle saved_state )
  {
    System.out.println( "onCreate()" );
    System.gc();

    singleton = this;

    AndroidSound.sound_pool = new SoundPool(15, AudioManager.STREAM_MUSIC, 0);

    // collect asset list
    collectAssets("");
    collectAssets("images");
    collectAssets("data");
    collectAssets("sounds");

    for (int i=0; i<pending_events.length; ++i)
    {
      pending_events[i] = new PendingEvent();
    }

    // Use introspection to support multitouch if enabled
    try
    {
      Class classMotionEvent = Class.forName("android.view.MotionEvent");
      m_getPointerCount = classMotionEvent.getDeclaredMethod( "getPointerCount", (Class[]) null );
      m_getPointerId = classMotionEvent.getDeclaredMethod( "getPointerId", int.class );
      m_getX = classMotionEvent.getDeclaredMethod( "getX", int.class );
      m_getY = classMotionEvent.getDeclaredMethod( "getY", int.class );
    }
    catch (Exception err)
    {
      // API 1.6 or earlier
    }

    activity.setVolumeControlStream(AudioManager.STREAM_MUSIC); //let volume control affect the music and sound

    // set up accelerometer listener
    SensorManager manager = (SensorManager)activity.getSystemService(Activity.SENSOR_SERVICE);
    if(manager != null) 
    {
      Sensor accelerometer = manager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

      if(accelerometer != null)
      {
        manager.registerListener( this, accelerometer, SensorManager.SENSOR_DELAY_GAME);
      }
    }

    configureView();
  }

  public void configureView()
  {
    view = new GLSurfaceView(activity);
    view.setEGLContextClientVersion(2);
    view.setRenderer( new PlasmacoreRenderer() );
    layout = new AbsoluteLayout(activity);
    layout.addView(view);
    activity.setContentView( layout );
    view.invalidate();
  }


  protected void collectAssets( String path )
  {
    // Getting an assets list takes a while so avoid it if we can determine that
    // we've got a single file.
    if (path.endsWith(".png") || path.endsWith(".jpg") || path.endsWith(".wav")
        || path.endsWith(".ogg") || path.endsWith(".mp3") || path.endsWith(".m4a"))
    {
      assets.add(path);
      return;
    }

    try
    {
      String[] list = activity.getAssets().list(path);
      if (list.length > 0)
      {
        for (String asset : activity.getAssets().list(path))
        {
          // don't recurse in root "" since it takes a while
          if (path.length() > 0) collectAssets( path + "/" + asset );
          else assets.add(asset);
        }
      }
      else
      {
        assets.add( path );
      }
    }
    catch (Exception err)
    {
    }
  }

  public void onResume()
  {
    pausing = false;
    paused = false;

    System.gc();

    if (view != null) view.onResume();

    for (int i=0; i<sound_bank.list.size(); ++i)
    {
      AndroidSound sound = sound_bank.list.get(i);
      if (sound != null) sound.system_resume();
    }

    // web views corrupt screen on resume if not hidden
    for (int i=0; i<web_view_bank.list.size(); ++i)
    {
      PCWebView view = web_view_bank.list.get(i);
      if (view != null) view.system_resume();
    }

  }

  public void onPause()
  {
    System.out.println( "onPause()" );

    for (int i=0; i<sound_bank.list.size(); ++i)
    {
      AndroidSound sound = sound_bank.list.get(i);
      if (sound != null) sound.system_pause();
    }

    // web views corrupt screen on resume if not hidden
    for (int i=0; i<web_view_bank.list.size(); ++i)
    {
      PCWebView view = web_view_bank.list.get(i);
      if (view != null) view.system_pause();
    }

    int timeout = 1000;
    pausing = true;
    while ( !paused )
    {
      timeout -= 5;
      if (timeout <= 0) break;

      try
      {
        Thread.sleep(5);
      }
      catch (InterruptedException err)
      {
      }
    }

    if (view != null) view.onPause();
  }

  public void onStop()
  {
    System.out.println( "onStop()" );
    releaseWakeLock();
  }

  public void onDestroy()
  {
    System.out.println( "onDestroy()" );

    synchronized (execution_lock)
    {
      bardOnShutDown();
    }

    AndroidSound.sound_pool.release();
    AndroidSound.sound_pool = null;

    releaseWakeLock();
  }

  public void releaseWakeLock()
  {
    try
    {
      if(wake_lock!=null) wake_lock.release();
      wake_lock=null;
    }catch(Exception ignore){}
  }

  public void onConfigurationChanged( Configuration new_config )
  {
    if ((new_config.hardKeyboardHidden & Configuration.HARDKEYBOARDHIDDEN_YES)>0) hard_keyboard_hidden=true;
    else if ((new_config.hardKeyboardHidden & Configuration.HARDKEYBOARDHIDDEN_NO)>0)  hard_keyboard_hidden=false;

    if (showing_keyboard)
    {
      InputMethodManager imm = (InputMethodManager)activity.getSystemService(Context.INPUT_METHOD_SERVICE);
      imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY);
    }
  }

  public boolean onKeyDown(int keyCode, final KeyEvent event) 
  {
    switch(keyCode)
    {
      case KeyEvent.KEYCODE_BACK: 
        addPendingKeyEvent( true, 27, true ); // ESC
        return true;
      case KeyEvent.KEYCODE_MENU:
        addPendingKeyEvent( true, 282, false ); // F1
        return true;
      case KeyEvent.KEYCODE_VOLUME_UP:
      case KeyEvent.KEYCODE_VOLUME_DOWN:
        return false;
    }

    int keycode = event.getKeyCode();
    int unicode = 0;
    switch (keycode)
    {
      case 66: keycode = 0; unicode = 13; break;
      case 67: keycode = 0; unicode =  8; break;
      case 59: keycode = 304; break; // left shift
      case 60: keycode = 305; break; // right shift
      default: 
       keycode = 0; 
       unicode = event.getUnicodeChar(event.getMetaState());
    }

    if (keycode != 0)
    {
      addPendingKeyEvent( true, keycode, false );
    }
    else
    {
      addPendingKeyEvent( true, unicode, true );
    }
    return true;
  }

  public boolean onKeyUp(int keyCode, final KeyEvent event) 
  {
    switch(keyCode)
    {
      case KeyEvent.KEYCODE_BACK:
        addPendingKeyEvent( false, 27, true ); // ESC
        return true; 
      case KeyEvent.KEYCODE_MENU:
        addPendingKeyEvent( false, 282, false ); // F1
        return true; 
      case KeyEvent.KEYCODE_VOLUME_UP:
      case KeyEvent.KEYCODE_VOLUME_DOWN:
        return false;
    }

    int keycode = event.getKeyCode();
    int unicode = 0;
    switch (keycode)
    {
      case 66: keycode = 0; unicode = 13; break;
      case 67: keycode = 0; unicode =  8; break;
      case 59: keycode = 304; break; // left shift
      case 60: keycode = 305; break; // right shift
      default: 
        keycode = 0;
        unicode = event.getUnicodeChar(event.getMetaState());
    }

    if (keycode != 0)
    {
      addPendingKeyEvent( false, keycode, false );
    }
    else
    {
      addPendingKeyEvent( false, unicode, true );
    }
    return true;
  }

  public boolean onTouchEvent( final MotionEvent event ) 
  {
    try
    {
      switch(event.getAction()) 
      {
        case MotionEvent.ACTION_DOWN:
        case 5:  //MotionEvent.ACTION_POINTER_1_DOWN:
          begin_touch( event.getX(), event.getY() );
          break;

        case MotionEvent.ACTION_UP:
        case 6:  //MotionEvent.ACTION_POINTER_1_UP:
          end_touch( event.getX(), event.getY() );
          break;

        case 261: //MotionEvent.ACTION_POINTER_2_DOWN:
          if (m_getX != null)
          {
            double x = (Float) m_getX.invoke( event, 1 );
            double y = (Float) m_getY.invoke( event, 1 );
            begin_touch( x, y );
          }
          break;

        case 262: //MotionEvent.ACTION_POINTER_2_UP:
          if (m_getX != null)
          {
            double x = (Float) m_getX.invoke( event, 1 );
            double y = (Float) m_getY.invoke( event, 1 );
            end_touch( x, y );
          }
          break;


        case 517: //MotionEvent.ACTION_POINTER_3_DOWN:
          if (m_getX != null)
          {
            double x = (Float) m_getX.invoke( event, 2 );
            double y = (Float) m_getY.invoke( event, 2 );
            begin_touch( x, y );
          }
          break;

        case 518: //MotionEvent.ACTION_POINTER_3_UP:
          if (m_getX != null)
          {
            double x = (Float) m_getX.invoke( event, 2 );
            double y = (Float) m_getY.invoke( event, 2 );
            end_touch( x, y );
          }
          break;

        case MotionEvent.ACTION_MOVE:
          if (m_getPointerCount == null)
          {
            update_touch( event.getX(), event.getY() );
          }
          else
          {
            int count = (Integer) m_getPointerCount.invoke( event );
            for(int i = 0; i < count; ++i) 
            {
              int id = (Integer) m_getPointerId.invoke( event, i );
              double x = (Float) m_getX.invoke( event, id );
              double y = (Float) m_getY.invoke( event, id );
              update_touch( x, y );
            }
          }
          break;
      }
    }
    catch (Exception err)
    {
      // never gonna happen
    }

    return true;
  }

  void begin_touch( double x, double y )
  {
    addPendingTouchEvent( 0, touch_manager.begin_touch(x,y), x, y );
  }

  void update_touch( double x, double y )
  {
    addPendingTouchEvent( 1, touch_manager.update_touch(x,y), x, y );
  }

  void end_touch( double x, double y )
  {
    addPendingTouchEvent( 2, touch_manager.end_touch(x,y), x, y );
  }

  public void onSensorChanged(SensorEvent event)
  {
    if(event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
    {
      acceleration_x = event.values[0] / SensorManager.STANDARD_GRAVITY;
      acceleration_y = event.values[1] / SensorManager.STANDARD_GRAVITY;
      acceleration_z = event.values[2] / SensorManager.STANDARD_GRAVITY;
    }
  }

  public void onAccuracyChanged(Sensor sensor, int accuracy) {}

  //---------------------------------------------------------------------------

  public class PendingEvent
  {
    final static public int KEY_EVENT = 0;
    final static public int TOUCH_EVENT = 1;
    final static public int SIMPLE_CUSTOM_EVENT = 2;
    final static public int COMPLEX_CUSTOM_EVENT = 3;

    int type;

    boolean is_press;
    boolean is_unicode;
    int     code;
    int     id;
    double  x,y,z;
    String  custom_id;
    String  message;

    public void KeyEvent( boolean is_press, int code, boolean is_unicode )
    {
      type = KEY_EVENT;
      this.is_press = is_press;
      this.code = code;
      this.is_unicode = is_unicode;
    }

    public void TouchEvent( int stage, int id, double x, double y )
    {
      type = TOUCH_EVENT;
      this.code = stage;
      this.id = id;
      this.x = x;
      this.y = y;
    }

    public void CustomEvent( String custom_id, String message )
    {
      type = SIMPLE_CUSTOM_EVENT;
      this.custom_id = custom_id;
      this.message = message;
    }

    public void CustomEvent( String custom_id, double value, String message )
    {
      type = COMPLEX_CUSTOM_EVENT;
      this.custom_id = custom_id;
      x = value;
      this.message = message;
    }

    public void dispatch()
    {
      switch (type)
      {
        case KEY_EVENT:
          synchronized (execution_lock)
          {
            bardKeyEvent( is_press, code, is_unicode );
          }
          break;

        case TOUCH_EVENT:
          synchronized (execution_lock)
          {
            bardTouchEvent( code, id, x, y );
          }
          break;

        case SIMPLE_CUSTOM_EVENT:
          synchronized (execution_lock)
          {
            bardCustomEvent( custom_id, message );
          }
          break;

        case COMPLEX_CUSTOM_EVENT:
          synchronized (execution_lock)
          {
            bardCustomEventWithValue( custom_id, x, message );
          }
          break;
      }
    }
  }

  public PendingEvent[] pending_events = new PendingEvent[50];
  public int num_pending_events = 0;

  public PendingEvent addPendingEvent()
  {
    if (++num_pending_events == pending_events.length) --num_pending_events;
    return pending_events[num_pending_events-1];
  }

  public void addPendingKeyEvent( boolean is_press, int code, boolean is_unicode )
  {
    synchronized (pending_events)
    {
      addPendingEvent().KeyEvent( is_press, code, is_unicode );
    }
  }

  public void addPendingTouchEvent( int stage, int id, double x, double y )
  {
    synchronized (pending_events)
    {
      addPendingEvent().TouchEvent( stage, id, x, y );
    }
  }

  public void addPendingCustomEvent( String custom_id )
  {
    synchronized (pending_events)
    {
      addPendingEvent().CustomEvent( custom_id, 0, null );
    }
  }

  public void addPendingCustomEvent( String custom_id, double value )
  {
    synchronized (pending_events)
    {
      addPendingEvent().CustomEvent( custom_id, value, null );
    }
  }

  public void addPendingCustomEvent( String custom_id, String message )
  {
    synchronized (pending_events)
    {
      addPendingEvent().CustomEvent( custom_id, message );
    }
  }

  public void addPendingCustomEvent( String custom_id, double value, String message )
  {
    synchronized (pending_events)
    {
      addPendingEvent().CustomEvent( custom_id, value, message );
    }
  }

  public void trace( String message )
  {
    addPendingCustomEvent( "trace", message );
  }

  public void queueSignal( String type, String arg )
  {
    addPendingCustomEvent( type, arg );
  }

  public void raiseSignal( String type, String arg )
  {
    addPendingCustomEvent( type, arg );
    raisePendingSignals();
  }

  public void raisePendingSignals()
  {
    synchronized (pending_events)
    {
      for (int i=0; i<num_pending_events; ++i)
      {
        pending_events[i].dispatch();
      }
      num_pending_events = 0;
    }
    synchronized (execution_lock)
    {
      bardRaisePendingSignals();
    }
  }

  //---------------------------------------------------------------------------

  public class PlasmacoreRenderer implements GLSurfaceView.Renderer
  {
    int     ms_error = 0;
    long    next_time;

    public void onSurfaceChanged( GL10 gl, int width, int height )
    {
      launch( width, height );
    }

    public void onSurfaceCreated( GL10 gl, EGLConfig config )
    {
      onTexturesLost();
    }

    public void onDrawFrame( GL10 gl )
    {
      int target_fps = tick();
      if (target_fps == 0) target_fps = 1;

      if (paused) return;

      long cur_time = System.currentTimeMillis();
      long kill_ms = next_time - cur_time;
      if (kill_ms < 0)
      {
        next_time = cur_time;
        kill_ms = 1;
      }
      next_time += 1000 / target_fps;
      ms_error  += 1000 % target_fps;
      if (ms_error >= target_fps)
      {
        ++next_time;
      }

      try
      {
        Thread.sleep(kill_ms);
      }
      catch (InterruptedException err)
      {
      }
    }
  }

  //---------------------------------------------------------------------------

  public void alert( String mesg )
  {
    if (view != null) view.onPause();

    new AlertDialog.Builder(activity)
      .setTitle("Fatal Error")
      .setMessage(mesg)
      .setCancelable(false)
      .setPositiveButton( "Okay", 
          new DialogInterface.OnClickListener()
          {
            public void onClick( DialogInterface dialog, int which )
            {
              jniExitProgram();
            }
          }
        )
    .show();
  }

  public boolean isID( int ch )
  {
    if (ch >= 'A' && ch <= 'Z') return true;
    if (ch >= 'a' && ch <= 'z') return true;
    if (ch >= '0' && ch <= '9') return true;
    if (ch == '_' || ch == '.' || ch == '-') return true;
    return false;
  }

  public boolean equalsSubstring( String st1, String st2, int st2_i, int compare_count )
  {
    if (st1.length() != compare_count) return false;
    if (st2_i + compare_count > st2.length()) return false;

    int st1_i = -1;
    --st2_i;
    ++compare_count;
    while (--compare_count != 0)
    {
      if (st1.charAt(++st1_i) != st2.charAt(++st2_i)) return false;
    }

    return true;
  }

  public boolean partialNameMatch( String partial, String full )
  {
    int full_len = full.length();
    int partial_len = partial.length();
    int partial_ch0 = partial.charAt(0);

    if (partial_len > full_len) return false;

    // look for a substring match
    int diff = full_len - partial_len;
    for (int i=0; i<=diff; i++)
    {
      if (partial_ch0 == full.charAt(i) && equalsSubstring(partial,full,i,partial_len))
      {
        int prev_char;
        if (i > 0) prev_char = full.charAt(i-1);
        else prev_char = 0;

        int next_char;
        if(i+partial_len < full.length()) next_char = full.charAt(i+partial_len);
        else next_char = 0; 
        if ( !isID(prev_char) && (next_char==0 || next_char=='.'))
        {
          return true;
        }
      }
    }

    return false;
  }

  //---------------------------------------------------------------------------

  native public boolean bardCreate( int width, int height, byte[] etc_bytes );

  native public int  bardUpdateDrawEvent();
  native public void bardTexturesLostEvent();
  native public void bardKeyEvent( boolean press, int code, boolean is_unicode );
  native public void bardTouchEvent( int stage, int id, double x, double y );
  native public void bardCustomEvent( String custom_id, String message );
  native public void bardCustomEventWithValue( String custom_id, double value, String message );
  native public void bardAccelerationEvent( double x, double y, double z );
  native public void bardRaisePendingSignals();
  native public void bardOnPause();
  native public void bardOnShutDown();

  //---------------------------------------------------------------------------

  public void jniLog( String mesg )
  {
    System.out.println( mesg );
  }

  public int jniAndroidIsTablet()
  {
    try 
    {
      // Compute screen size
      DisplayMetrics dm = activity.getResources().getDisplayMetrics();
      float screenWidth  = dm.widthPixels / dm.xdpi;
      float screenHeight = dm.heightPixels / dm.ydpi;
      double size = Math.sqrt(Math.pow(screenWidth, 2) +
                              Math.pow(screenHeight, 2));
      // Tablet devices should have a screen size greater than 6 inches
      if (size >= 6.0) return 1;
      else             return 0;
    } 
    catch( Throwable t )
    {
      Log.e( LOGTAG, "Failed to compute screen size", t );
      return 0;
    }
  }

  public int jniAndroidMemoryClass()
  {
    try
    {
      // 1.6 devices don't define getMemoryClass() so we have to check and
      // see if the method exists.
      Class classActivityManager = Class.forName( "android.app.ActivityManager" );
      ActivityManager mgr = (ActivityManager) activity.getSystemService("activity");
      Method m_getMemoryClass = classActivityManager.getDeclaredMethod( 
          "getMemoryClass", (Class[]) null );
      int mb = (Integer) m_getMemoryClass.invoke( mgr );

      return mb;
    }
    catch (Exception err)
    {
      return 16;
    }
  }

  public String jniAndroidAppVersion()
  {
    try
    {
      return activity.getPackageManager().getPackageInfo(activity.getPackageName(), 0).versionName;
    }
    catch (Exception e)
    {
      return "unknown";
    }
  }

  public void jniExitProgram()
  {
    releaseWakeLock();
    if (release_wake_timer != null) release_wake_timer.cancel();
    activity.finish();
  }

  public String find_file( int type, String filename )
  {
    // Type: res_general, res_data, res_image, or res_sound
    File file = new File(filename);
    if (file.exists()) return file.getAbsolutePath();
    return null;
  }

  public String find_asset( int type, String filename )
  {
    for (int i=0; i<assets.size(); ++i)
    {
      String asset = assets.get(i);

      if (partialNameMatch(filename,asset))
      {
        return asset;
      }
    }
    return null;
  }

  public AssetFileDescriptor open_asset_fd( String filename )
  {
    try
    {
      return activity.getAssets().openFd(filename);
    }
    catch (IOException err)
    {
    }

    return null;
  }

  public byte[] jniLoadResource( int type, String filename )
  {
    try
    {
      // Try loading from embedded assets
      for (int i=0; i<assets.size(); ++i)
      {
        String asset = assets.get(i);

        if (partialNameMatch(filename,asset))
        {
          return loadBytes( activity.getAssets().open(asset) );
        }
      }

      // Try loading the filename as it is
      return loadBytes( new FileInputStream(filename) );
    }
    catch (Exception err)
    {
      return null;
    }
  }

  public byte[] loadBytes( InputStream infile )
  {
    try
    {
      byte[] bytes = new byte[infile.available()];
      infile.read( bytes );
      infile.close();
      return bytes;
    }
    catch (Exception err)
    {
      return null;
    }
  }

  int bytes_loaded;

  int[] jniDecodeBitmapData( byte[] data )
  {
    Bitmap bmp = null;
    bmp = BitmapFactory.decodeByteArray( data, 0, data.length );
    if (bmp == null) return null;

    int w = bmp.getWidth();
    int h = bmp.getHeight();

    int[] pixels = new int[ w * h + 1 ];

    bmp.getPixels( pixels, 0, w, 0, 0, w, h );
    bmp.recycle();
    bmp = null;
    bytes_loaded += data.length;
    /*
    if (bytes_loaded >= 100000)
    {
      System.gc();
      bytes_loaded = 0;
    }
    */

    pixels[w*h] = w;
    return pixels;
  }

  byte[] jniEncodeBitmapData( int w, int h, int[] data, int encoding, int quality )
  {
    // encoding: 1=png, 2=jpg
    Bitmap bmp = Bitmap.createBitmap( data, w, h, Bitmap.Config.ARGB_8888 );

    ByteArrayOutputStream buffer = new ByteArrayOutputStream();
    Bitmap.CompressFormat format;

    if (encoding == 1) format = Bitmap.CompressFormat.PNG;
    else format = Bitmap.CompressFormat.JPEG;

    if (bmp.compress( format, quality, buffer )) return buffer.toByteArray();
    else return null;
  }

  public String jniGetCountryName()
  {
    return Locale.getDefault().getDisplayCountry();
  }

  public String jniGetDeviceID()
  {
    return Installation.id( Plasmacore.activity );
  }

  /*
  public String jniGetDeviceID()
  {
    String device_id = ((TelephonyManager)activity.getSystemService(Activity.TELEPHONY_SERVICE)).getDeviceId();
    if (device_id != null) return device_id;

    return Installation.id( Plasmacore.activity );
  }
  */

  public String jniGetDeviceType()
  {
    return android.os.Build.MODEL;
  }

  public String jniGetLanguage()
  {
    return Locale.getDefault().getLanguage();
  }


  public void jniOpenURL( String url )
  {
    try
    {
      activity.startActivity( new Intent( Intent.ACTION_VIEW, Uri.parse(url) ) );
    }
    catch (RuntimeException err)
    {
      System.err.println(err);
    }
  }

  public boolean jniIsDirectory( String filename )
  {
    return new File(filename).isDirectory();
  }

  public String[] jniDirectoryListing( String path )
  {
    return new File(path).list();
  }

  public String jniAbsoluteFilePath( String path )
  {
    return new File(path).getAbsolutePath();
  }

  public void jniFileCopy( String old_name, String new_name )
  {
    try
    {
      FileInputStream infile = new FileInputStream(old_name);
      FileOutputStream outfile = new FileOutputStream(new_name);
      for (int ch = infile.read(); ch != -1; ch = infile.read())
      {
        outfile.write(ch);
      }
      infile.close();
      outfile.close();
    }
    catch (IOException err)
    {
    }
  }

  public boolean jniFileExists( String filename )
  {
    return (new File(filename)).exists();
  }

  public void jniFileRename( String old_name, String new_name )
  {
    new File(old_name).renameTo(new File(new_name));
  }

  public void jniFileDelete( String filename )
  {
    new File(filename).delete();
  }

  public long jniFileTimestamp( String filename )
  {
    return new File(filename).lastModified();
  }

  public void jniFileTouch( String filename )
  {
    new File(filename).setLastModified( System.currentTimeMillis() );
  }

  public void jniFileMkdir( String filename )
  {
    new File(filename).mkdirs();
  }

  public int jniFileReaderOpen( String filename )
  {
    try
    {
      return infile_bank.add( new FileInputStream(filename) );
    }
    catch (IOException err)
    {
      return 0;
    }
  }

  public void jniFileReaderClose( int index )
  {
    try
    {
      FileInputStream infile = infile_bank.get(index);
      if (infile != null)
      {
         infile.close();
         infile_bank.release(index);
      }
    }
    catch (IOException err)
    {
    }
  }

  public byte[] jniFileReaderReadBytes( int index, int count )
  {
    FileInputStream infile = infile_bank.get(index);
    if (infile == null) return null;

    if (io_buffer == null || io_buffer.length < count)
    {
      io_buffer = new byte[count];
    }

    try
    {
      infile.read(io_buffer);
      return io_buffer;
    }
    catch (IOException err)
    {
      return null;
    }
  }

  public int jniFileReaderAvailable( int index )
  {
    FileInputStream infile = infile_bank.get(index);
    if (infile == null) return 0;

    try
    {
      return infile.available();
    }
    catch (IOException err)
    {
      return 0;
    }
  }

  public byte[] jniGetIOBuffer( int required_size )
  {
    if (io_buffer == null || io_buffer.length < required_size)
    {
      io_buffer = new byte[required_size];
    }
    return io_buffer;
  }

  public int jniFileWriterOpen( String filename, boolean append )
  {
    try
    {
      return outfile_bank.add( new FileOutputStream(filename,append) );
    }
    catch (IOException err)
    {
      return 0;
    }
  }

  public void jniFileWriterClose( int index )
  {
    try
    {
      FileOutputStream outfile = outfile_bank.get(index);
      if (outfile != null)
      {
        outfile.close();
        outfile_bank.release(index);
      }
    }
    catch (IOException err)
    {
    }
  }

  public void jniFileWriterWriteBytes( int index, byte[] data, int count )
  {
    FileOutputStream outfile = outfile_bank.get(index);
    if (outfile == null) return;

    try
    {
      outfile.write( data, 0, count );
    }
    catch (IOException err)
    {
    }
  }

  public boolean jniNetworkIsConnected()
  {
    ConnectivityManager manager = (ConnectivityManager) 
      activity.getSystemService( Context.CONNECTIVITY_SERVICE );
    NetworkInfo info = manager.getActiveNetworkInfo();
    if (info == null) return false;
    return (info.getType() == ConnectivityManager.TYPE_WIFI || info.getType() == ConnectivityManager.TYPE_MOBILE);
  }

  public boolean jniNetworkIsMobile()
  {
    ConnectivityManager manager = (ConnectivityManager) 
      activity.getSystemService( Context.CONNECTIVITY_SERVICE );
    NetworkInfo info = manager.getActiveNetworkInfo();
    if (info == null) return false;
    return (info.getType() == ConnectivityManager.TYPE_MOBILE);
  }

  public String  jniNetworkMacAddress()
  {
    WifiManager manager = (WifiManager) activity.getSystemService( Context.WIFI_SERVICE );
    WifiInfo info = manager.getConnectionInfo();
    return info.getMacAddress();
  }

  public int jniSoundLoad( String filename )
  {
    String filepath = find_file(res_sound,filename);
    if (filepath == null) filename = find_asset(res_sound,filename);
    if (filename == null) return 0;

    boolean bg_music;
    String ext = filename.substring( filename.lastIndexOf('.')+1 );
    if(ext.compareToIgnoreCase("wav")==0 || ext.compareToIgnoreCase("caf")==0
        || ext.compareToIgnoreCase("aif")==0 || ext.compareToIgnoreCase("aiff")==0)
    {
      bg_music = false;
    }
    else 
    {
      bg_music = true;
    }

    AndroidSound sound;
    if (filepath != null) sound = new AndroidSound(filepath, bg_music);
    else sound = new AndroidSound( open_asset_fd(filename), filename, bg_music );

    if(sound.error()) return 0;

    return sound_bank.add( sound );
  }

  public String jniSHA1HMAC( String mesg, String key ) 
  {
    byte[] key_bytes = key.getBytes();
    try
    {
      // Get an HMAC-SHA1 signing key from the raw key bytes
      SecretKeySpec sha1_key = new SecretKeySpec(key_bytes, "HmacSHA1");

      // Get an HMAC-SHA1 Mac instance and initialize it with the HMAC-SHA1 key
      Mac mac = Mac.getInstance("HmacSHA1");
      mac.init( sha1_key );

      // compute the binary signature for the message
      byte[] sig_bytes = mac.doFinal( mesg.getBytes() );

      StringBuilder buffer = new StringBuilder(sig_bytes.length*2);
      for (byte b : sig_bytes)
      {
        int left = (b >> 4) & 15;
        int right = b & 15;
        if (left < 10)  buffer.append( (char)(left + '0') );
        else            buffer.append( (char)((left-10) + 'a') );
        if (right < 10) buffer.append( (char)(right + '0') );
        else            buffer.append( (char)((right-10) + 'a') );
      }

      return buffer.toString();
    }
    catch (Exception err)
    {
      System.out.println( "Error signing URL: " + mesg );
      System.out.println( err.toString() );
      return "0000000000000000000000000000000000000000";
    }
  }

  public int jniSoundDuplicate(int sound_id) 
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound == null) return 0;

    AndroidSound new_sound;
    if (find_file(res_sound,sound.path) != null)
    {
      new_sound = new AndroidSound(sound.path, sound.is_bg_music);
    }
    else
    {
      new_sound = new AndroidSound( open_asset_fd(sound.path), sound.path, sound.is_bg_music );
    }

    if(new_sound.error()) return 0;

    return sound_bank.add( new_sound );
  }

  public void jniSoundPlay(int sound_id)
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) sound.play();
  }

  public void jniSoundPause(int sound_id) 
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) sound.pause();
  }

  public boolean jniSoundIsPlaying(int sound_id)
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) return sound.is_playing();

    return false;
  }

  public void jniSoundSetVolume(int sound_id, double volume)
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) sound.set_volume((float)volume);
  }

  public void jniSoundSetRepeats(int sound_id, boolean repeats)
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) sound.set_repeats(repeats);
  }

  public double jniSoundGetCurrentTime(int sound_id)
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) return sound.get_current_time() / 1000.0;

    return 0.0;
  }

  public void jniSoundSetCurrentTime(int sound_id, double current_time)
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) sound.set_current_time((int)(current_time*1000));
  }

  public double jniSoundDuration(int sound_id)
  {
    AndroidSound sound = sound_bank.get(sound_id);
    if(sound != null) return sound.get_duration() / 1000.0;

    return 0.0;
  }

  public void jniSoundRelease(int sound_id) 
  {
    AndroidSound sound = sound_bank.release(sound_id);
    if(sound != null) 
    {
      sound.release();
    }
  }

  public void jniShowKeyboard( boolean visible )
  {
    if (visible)
    {
      showing_keyboard = true;
      InputMethodManager imm = (InputMethodManager)activity.getSystemService(Context.INPUT_METHOD_SERVICE);
      imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY);
    }
    else
    {
      showing_keyboard = false;
      InputMethodManager imm = (InputMethodManager)activity.getSystemService(Context.INPUT_METHOD_SERVICE);
      imm.hideSoftInputFromWindow(layout.getWindowToken(), 0);
    }
  }

  public boolean jniKeyboardVisible()
  {
    return showing_keyboard;
  }

  public byte[] jniLoadGamestate( String filename )
  {
    try 
    {
      for (String cur_filename : activity.fileList()) 
      {
        if (partialNameMatch(filename,cur_filename))
        {
          // found our match
          FileInputStream fileReader = activity.openFileInput(cur_filename);

          int offset = 0;
          int count = 0;
          byte data[] = new byte[ (int) fileReader.available() ];

          int read = 0;
          int numRead = 0;
          while (read < data.length && (numRead=fileReader.read(data, read, data.length-read)) >= 0) 
          {
            read = read + numRead;
          }

          return data;
        }
      }
    }
    catch (IOException err)
    {
    }

    return null;
  } 

  public boolean jniSaveGamestate( String filename, String content )
  {
    try
    {
      FileOutputStream fileWriter = activity.openFileOutput( filename, Activity.MODE_PRIVATE );

      for (int i=0; i<content.length(); i++)
      {
        fileWriter.write( content.charAt(i) );
      }
      fileWriter.flush();
      fileWriter.close();
      return true;
    }
    catch(IOException e) 
    {
    }

    return false;
  }

  public boolean jniDeleteGamestate( String filename )
  {
    return activity.deleteFile(filename);
  }

  public int jniVideoPlay( String filename )
  {
    Intent intent = new Intent(activity, VideoPlayer.class);
    intent.putExtra("video_filename", filename);
    activity.startActivity(intent);

    return 1;
  }

  public boolean jniVideoUpdate( int player_id )
  {
    return false;
  }

  public void jniVideoStop( int player_id )
  {
  }

  public int jniWebViewGet( int index )
  {
    synchronized (web_view_bank)
    {
      if (index > 0) return index;

      index = web_view_bank.add(null);
      new_web_view_handler.obtainMessage(index,this).sendToTarget();
      return index;
    }
  }

  public void jniWebViewURL( int index, String url )
  {
    synchronized (web_view_bank)
    {
      web_view_url_handler.obtainMessage(index,url).sendToTarget();
    }
  }

  public void jniWebViewHTML( int index, String html )
  {
    synchronized (web_view_bank)
    {
      web_view_html_handler.obtainMessage(index,html).sendToTarget();
    }
  }

  public void jniWebViewClose( int index )
  {
    synchronized (web_view_bank)
    {
      web_view_close_handler.obtainMessage(index).sendToTarget();
    }
  }

  public void jniWebViewSetBounds( int index, int x, int y, int w, int h )
  {
    synchronized (web_view_bank)
    {
      web_view_set_bounds_handler.obtainMessage(index,(x<<16)|y,(w<<16)|h).sendToTarget();
    }
  }

  public void jniWebViewSetVisible( int index, boolean setting )
  {
    synchronized (web_view_bank)
    {
      web_view_set_visible_handler.obtainMessage(index,setting?1:0,0).sendToTarget();
    }
  }

  public boolean jniWebViewGetVisible( int index )
  {
    synchronized (web_view_bank)
    {
      PCWebView view = web_view_bank.get(index);
      if (view == null) return false;
      return view.visible;
    }
  }

  public boolean jniWebViewGetLoaded( int index )
  {
    synchronized (web_view_bank)
    {
      PCWebView view = web_view_bank.get(index);
      if (view == null) return false;
      return view.loaded;
    }
  }

  public boolean jniWebViewGetFailed( int index )
  {
    synchronized (web_view_bank)
    {
      PCWebView view = web_view_bank.get(index);
      if (view == null) return false;
      return view.failed;
    }
  }

  class PCWebView extends WebView
  {
    boolean loaded, failed, visible;
    int x, y, width, height;
    boolean suspended;
    boolean closed;

    public PCWebView( Context context )
    {
      super(context);
    }

    public void set_visible( boolean setting )
    {
      if (visible == setting) return;

      visible = setting;
      if (visible)
      {
        if ( !closed )
        {
          layout.addView( this, new AbsoluteLayout.LayoutParams(width,height,x,y) );
        }
      }
      else
      {
        layout.removeView( this );
      }
    }

    public void system_pause()
    {
      if (visible)
      {
        set_visible(false);
        suspended = true;
      }
    }
    
    public void system_resume()
    {
      if (suspended)
      {
        web_view_resume_handler.obtainMessage(0,this).sendToTarget();
        suspended = false;
      }
    }
  }
}

class IntList
{
  int[] data;
  int   size;

  IntList( int capacity )
  {
    data = new int[capacity];
  }

  public void ensure_capacity( int capacity )
  {
    if (data.length < capacity)
    {
      int[] new_data = new int[capacity];
      for (int i=0; i<data.length; ++i) new_data[i] = data[i];
      data = new_data;
    }
  }

  public void add( int value )
  {
    if (size == data.length) ensure_capacity(data.length*2);
    data[size++] = value;
  }

  public int get( int index )
  {
    return data[index];
  }

  public void set( int index, int value )
  {
    data[index] = value;
  }

  public boolean remove_value( int value )
  {
    if (size == 0) return false;

    for (int i=0; i<size; ++i)
    {
      if (data[i] == value)
      {
        data[i] = data[--size];
        return true;
      }
    }

    return false;
  }

  public int remove_last()
  {
    return data[--size];
  }
}

class TouchInfo
{
  boolean active;
  double x,y;

  TouchInfo() { active = false; }
}

class TouchManager
{
  final static public int MAX_TOUCHES = 4;

  TouchInfo[] touches = new TouchInfo[MAX_TOUCHES];
  int         num_active_touches = 0;

  TouchManager() 
  { 
    for (int i=0; i<MAX_TOUCHES; ++i) touches[i] = new TouchInfo();
  }

  // Each method returns the index (1+) that TouchManager
  // is using to track the event.
  int begin_touch( double x, double y )
  {
    if (num_active_touches < MAX_TOUCHES)
    {
      for (int i=0; i<MAX_TOUCHES; ++i)
      {
        if ( !touches[i].active )
        {
          touches[i].active = true;
          touches[i].x = x;
          touches[i].y = y;
          ++num_active_touches;
          return i+1;
        }
      }
    }
    return 0;
  }

  int update_touch( double x, double y )
  {
    int best_i = -1;
    double dx,dy;
    double best_r2 = 0;
    for (int i=0; i<MAX_TOUCHES; i++)
    {
      if (touches[i].active)
      {
        dx = touches[i].x - x;
        dy = touches[i].y - y;
        double r2 = dx*dx + dy*dy;
        if (best_i == -1 || r2 < best_r2)
        {
          best_r2 = r2;
          best_i = i;
        }
      }
    }

    if (best_i == -1)
    {
      //System.out.println( "ERROR: update_touch with no active touches!\n" );
      //  Commented out warning since this happens on Android WebView all the time
      return 0;
    }

    touches[best_i].x = x;
    touches[best_i].y = y;
    return best_i+1;
  }

  int end_touch( double x, double y )
  {
    int i = update_touch(x,y);
    touches[i-1].active = false;
    --num_active_touches;
    return i;
  }
}

//=============================================================================
//  AndroidSound
//=============================================================================
class AndroidSound implements MediaPlayer.OnErrorListener
{
  static SoundPool sound_pool;

  String path;
  AssetFileDescriptor fd;
  boolean error;

  int sound_pool_id;  // for sound effects
  MediaPlayer media_player; // for music

  int stream_id;

  boolean system_sound_repeats;
  boolean is_bg_music;
  boolean system_paused;


  AndroidSound( String path, boolean is_bg_music )
  {
    this.path = path;
    this.is_bg_music = is_bg_music;
    initialize();
  }

  AndroidSound( AssetFileDescriptor fd, String path, boolean is_bg_music )
  {
    this.fd = fd;
    this.path = path;
    this.is_bg_music = is_bg_music;
    initialize();
  }


  void initialize()
  {
    for (int i=0; i<3; ++i)
    {
      try
      {
        sound_pool_id = -1;
        if (media_player != null) media_player.release();
        media_player = null;
        system_sound_repeats = false;

        if (is_bg_music)
        {
          media_player = new MediaPlayer();
          media_player.setOnErrorListener(this);
          try 
          {
            media_player.reset();
            if (fd != null)
            {
              media_player.setDataSource(
                  fd.getFileDescriptor(),
                  fd.getStartOffset(),
                  fd.getLength()
                );
            }
            else
            {
              media_player.setDataSource(path);
            }
            media_player.prepare();
            media_player.setAudioStreamType(AudioManager.STREAM_MUSIC); //Added by Ty so the volume control has an effect on this.
          }
          catch(Exception e) 
          {
            System.out.println( "Error initializing media_player: " + e.toString() );
          }
        }
        else
        {
          if (fd != null) sound_pool_id = sound_pool.load(fd, 1);
          else            sound_pool_id = sound_pool.load(path, 1);

          long timeout = System.currentTimeMillis() + 1000;
          int stream;
          while((stream = sound_pool.play(sound_pool_id, 0.0f, 0.0f, 1, 0, 1.0f)) == 0)
          {
            // Briefly play the stream at zero volume to get it decoded and prepped.
            try
            {
              Thread.sleep(8);
            }
            catch(InterruptedException e) {}

            if(System.currentTimeMillis() >= timeout) break;
          }

          sound_pool.stop(stream);
        }
        error = false;
        return;
      }
      catch (RuntimeException this_sometimes_happens)
      {
        try
        {
          Thread.sleep(2000);
        }
        catch (InterruptedException err)
        {
        }
      }
    }

    System.out.println(
        "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    System.out.println( "Failed to load sound " + path );
    System.out.println(
        "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    sound_pool_id = -1;
    media_player = null;
  }

  public boolean onError( MediaPlayer player, int what, int extra )
  {
    error = true;
    return false;
  }

  void release()
  {
    if(is_bg_music) 
    {
      if (media_player != null) media_player.release();
    }
    else if (sound_pool_id >= 0)
    {
      sound_pool.unload(sound_pool_id);
    }
  }

  boolean error() 
  { 
    return media_player == null && sound_pool_id == -1; 
  }

  void play()
  {
    if (media_player != null)
    {
      media_player.start();
      if ( !media_player.isPlaying() )
      {
        error = true;
        initialize();
        if (media_player != null) media_player.start();
      }
    }
    else if (sound_pool_id >= 0)
    {
      if(system_sound_repeats) stream_id = sound_pool.play(sound_pool_id, 1.0f, 1.0f, 0, -1, 1.0f);
      else stream_id = sound_pool.play(sound_pool_id, 1.0f, 1.0f, 0, 0, 1.0f);
    }
  }

  void system_pause()
  {
    try
    {
      if (error) initialize();
      if (media_player != null && media_player.isPlaying())
      {
        media_player.pause();
        system_paused = true;
      }
    }
    catch (IllegalStateException err)
    {
      // bleh
    }
  }

  void system_resume()
  {
    if (media_player != null && system_paused)
    {
      if (error) initialize();
      media_player.start();
      system_paused = false;
    }
  }

  void pause()
  {
    if (media_player != null ) 
    {
      if(media_player.isPlaying()) 
      {
        media_player.pause();
      }
    }
    else 
    {
      sound_pool.pause(stream_id);
    }
  }

  boolean is_playing()
  {
    if (media_player != null) 
    {
      return media_player.isPlaying();
    }
    return false;
  }

  void set_volume( float new_volume )
  {
    if (media_player != null) 
    {
      media_player.setVolume( new_volume, new_volume );
    }
    //if (audio_player) audio_player.volume = new_volume;
  }

  void set_repeats( boolean setting )
  {
    if (media_player != null)
    {
      media_player.setLooping(setting);
    }
    else
    {
      system_sound_repeats = setting;
    }
  }

  double get_current_time()
  {
    if (media_player != null) 
    {
      return media_player.getCurrentPosition();
    }
    return 0.0;
  }

  void set_current_time( int new_time )
  {
    if (media_player != null) 
    {
      media_player.seekTo(new_time);
    }
    else if (sound_pool_id >= 0)
    {
      sound_pool.pause(stream_id);
    }
  }

  double get_duration()
  {
    if (media_player != null) 
    {
      return media_player.getDuration();
    }
    return 1.0;
  }
}

//=============================================================================
//  ResourceBank
//=============================================================================
class ResourceBank<ResType>
{
  IntList available = new IntList(10);
  ArrayList<ResType> list = new ArrayList<ResType>();

  public ResourceBank()
  {
    list.add(null);  // use up spot #0
  }

  public ResType get( int index )
  {
    if (index <= 0 || index >= list.size()) return null;
    return list.get(index);
  }

  public void set( int index, ResType res )
  {
    // requires 'index' to already be reserved via add(null).
    list.set( index, res );
  }

  public int add( ResType res )
  {
    if (available.size > 0)
    {
      int index = available.remove_last();
      list.set( index, res );
      return index;
    }
    else
    {
      int index = list.size();
      list.add(res);
      return index;
    }
  }

  public ResType release( int index )
  {
    ResType res = get(index);
    if (res != null)
    {
      available.add(index);
      list.set(index,null);
    }
    return res;
  }
}


class Installation 
{
    private static String sID = null;
    private static final String INSTALLATION = "INSTALLATION";

    public synchronized static String id(Context context) 
    {
      if (sID == null)
      {
          File installation = new File(context.getFilesDir(), INSTALLATION);
          try 
          {
            if (!installation.exists())
            {
              writeInstallationFile(installation);
            }
            sID = readInstallationFile(installation);
          } 
          catch (Exception e) 
          {
            throw new RuntimeException(e);
          }
      }
      return sID;
    }

    private static String readInstallationFile(File installation) throws IOException 
    {
        RandomAccessFile f = new RandomAccessFile(installation, "r");
        byte[] bytes = new byte[(int) f.length()];
        f.readFully(bytes);
        f.close();
        return new String(bytes);
    }

    private static void writeInstallationFile(File installation) throws IOException 
    {
        FileOutputStream out = new FileOutputStream(installation);
        String id = UUID.randomUUID().toString();
        out.write(id.getBytes());
        out.close();
    }
}

