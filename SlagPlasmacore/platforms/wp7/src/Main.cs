//=============================================================================
// Main.cs
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Plasmacore main program control.
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
using System.Linq;
using Microsoft.Devices.Sensors;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Input.Touch;
using Microsoft.Xna.Framework.Media;

using BardFramework;

namespace PlasmacoreWP7
{
  /// <summary>
  /// This is the main type for your game
  /// </summary>
  public class Main : Microsoft.Xna.Framework.Game
  {
    static public Main instance;
    bool back_pressed;

    GraphicsDeviceManager graphics;    
    Dictionary<int, int> touch_indices = new Dictionary<int, int>();

    public Main()
    {
      instance = this;

      graphics = new GraphicsDeviceManager(this);
      graphics.PreferredBackBufferWidth = PlasmacoreSettings.display_width;
      graphics.PreferredBackBufferHeight = PlasmacoreSettings.display_height;

      Content.RootDirectory = "Content";      
           
      TargetElapsedTime = TimeSpan.FromTicks(333333/2); // 60 fps target      
      
    }

    /// <summary>
    /// Allows the game to perform any initialization it needs to before starting to run.
    /// This is where it can query for any required services and load any non-graphic
    /// related content.  Calling base.Initialize will enumerate through any components
    /// and initialize them as well.
    /// </summary>
    protected override void Initialize()
    {
      // TODO: Add your initialization logic here
      Plasmacore.configure( Content, GraphicsDevice, 
        PlasmacoreSettings.display_width, PlasmacoreSettings.display_height );

      Accelerometer accelerometer = new Accelerometer();
      try
      {
        accelerometer.Start();
        accelerometer.ReadingChanged += new EventHandler<AccelerometerReadingEventArgs>(
          AccelerometerReadingChanged);
      }
      catch (Exception)
      {
      }

      base.Initialize();
    }

    public void AccelerometerReadingChanged( object sender, AccelerometerReadingEventArgs e )
    {
      Plasmacore.accel_x = e.X;
      Plasmacore.accel_y = e.Y;
      Plasmacore.accel_z = e.Z;
    }


    /// <summary>
    /// LoadContent will be called once per game and is the place to load
    /// all of your content.
    /// </summary>
    protected override void LoadContent()
    {
      Plasmacore.launch();            
    }

    /// <summary>
    /// UnloadContent will be called once per game and is the place to unload
    /// all content.
    /// </summary>
    protected override void UnloadContent()
    {
      // TODO: Unload any non ContentManager content here           
    }

    /// <summary>
    /// Allows the game to run logic such as updating the world,
    /// checking for collisions, gathering input, and playing audio.
    /// </summary>
    /// <param name="gameTime">Provides a snapshot of timing values.</param>
    protected override void Update(GameTime gameTime)
    {
      // Allows the game to exit
      if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
      {
        back_pressed = true;
        Plasmacore.on_key(true, 27, true);
      }
      else if (back_pressed)
      {
        back_pressed = false;
        Plasmacore.on_key(false, 27, true);
      }

      foreach (TouchLocation loc in TouchPanel.GetState())
      {        
        int id = loc.Id;
        int index;
        if (touch_indices.ContainsKey(id))
        {
          index = touch_indices[id];
        }
        else
        {
          index = touch_indices.Count + 1;
          touch_indices[id] = index;
        }
        
        if (loc.State == TouchLocationState.Pressed)
        {
          Plasmacore.on_mouse_button_press( index, loc.Position.X, loc.Position.Y, 1 );
        }
        else if (loc.State == TouchLocationState.Moved)
        {
          Plasmacore.on_mouse_move_absolute( index, loc.Position.X, loc.Position.Y );
        }
        else if (loc.State == TouchLocationState.Released)
        {
          Plasmacore.on_mouse_button_release( index, loc.Position.X, loc.Position.Y, 1 );
          touch_indices.Remove(id);
        }
      }

      Plasmacore.update();

      base.Update(gameTime);
    }

    /// <summary>
    /// This is called when the game should draw itself.
    /// </summary>
    /// <param name="gameTime">Provides a snapshot of timing values.</param>
    protected override void Draw(GameTime gameTime)
    {
      // TODO: Add your drawing code here
      Plasmacore.draw();

      base.Draw(gameTime);
    }

    protected override void OnExiting(object sender, System.EventArgs args)
    {
      GameXC.singleton_ClassApplication.on_exit_request();
      base.OnExiting(sender,args);
      
    }

    
  }

}
