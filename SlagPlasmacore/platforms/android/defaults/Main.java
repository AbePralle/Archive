package com.$(DEVELOPER_PACKAGE_ID).$(PROJECT_PACKAGE_ID);

import android.app.*;
import android.content.*;
import android.content.res.*;
import android.os.*;
import android.view.*;
import android.view.inputmethod.*;

public class Main extends Activity
{
  CustomPlasmacore plasmacore;

  // Override methods as desired.
  public void onCreate( Bundle saved_state )
  {
    super.onCreate( saved_state );

    requestWindowFeature( Window.FEATURE_NO_TITLE );
    getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN );

    plasmacore = new CustomPlasmacore( this );
    plasmacore.onCreate( saved_state );
  }

  public void onPause()
  {
    plasmacore.onPause();
    super.onPause();
  }

  public void onResume()
  {
    super.onResume();
    plasmacore.onResume();
  }

  public void onStop()
  {
    plasmacore.onStop();
    super.onStop();
  }

  public void onDestroy()
  {
    plasmacore.onDestroy();
    super.onDestroy();
  }

  public void onConfigurationChanged( Configuration new_config )
  {
    super.onConfigurationChanged( new_config );
    plasmacore.onConfigurationChanged( new_config );
  }

  public boolean onKeyDown( int key_code, final KeyEvent event )
  {
    if (plasmacore.onKeyDown(key_code,event)) return true;
    return super.onKeyDown(key_code, event);
  }

  public boolean onKeyUp( int key_code, final KeyEvent event )
  {
    if (plasmacore.onKeyUp(key_code,event)) return true;
    return super.onKeyUp(key_code, event);
  }

  public boolean onTouchEvent( final MotionEvent event ) 
  {
    return plasmacore.onTouchEvent( event );
  }
}

