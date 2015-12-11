//==============================================================================
package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JLabel;

//==============================================================================
/**
 * Simple JLabel to display a SlagFontImage
 */
//==============================================================================
public class FontImageViewer extends JLabel {

	// component id and default settings
	private static final long serialVersionUID = 2L;
	public static final Dimension PREFERRED_SIZE = new Dimension( 600, 200 );
	private static final Color DEFAULT_BG = Color.black;
	
	private BufferedImage fontImage = null;
	
	//--------------------------------------------------------------------------
	/**
	 * 
	 *///-----------------------------------------------------------------------
	public FontImageViewer(){
		super();
		
		setSize( PREFERRED_SIZE );
		super.setBackground( DEFAULT_BG );
		
	}
	
	//--------------------------------------------------------------------------
	/**
	 * 
	 *///-----------------------------------------------------------------------
	public void setBackground( Color newColor ){
		// ensures a repaint / update when bg is reset
		super.setBackground( newColor );
		repaint();
	}

	//--------------------------------------------------------------------------
	/**
	 * 
	 *///-----------------------------------------------------------------------
	public void setFontImage( BufferedImage newFontImage ){
		// ensures a repaint / update when font is reset
		fontImage = newFontImage;
		setPreferredSize( 
				new Dimension( fontImage.getWidth(), fontImage.getHeight() ) );
		repaint();
	}
	
	public BufferedImage getFontImage(){ return fontImage; }	
	
	//--------------------------------------------------------------------------
	/**
	 * 
	 *///-----------------------------------------------------------------------
	public void paintComponent( Graphics gfx ){
		super.paintComponent( gfx );
		
		// clear the background
		gfx.setColor( getBackground() );
		gfx.fillRect( 0, 0, getWidth(), getHeight() );
		
		// if there's a fontImage, center the DEFAULT_FONT_STRING in the component 
		//	based on the font's metrics and draw 
		if( fontImage != null ){
	    int x = ( getWidth() / 2 ) - ( fontImage.getWidth() / 2 );
	    int y = ( getHeight() / 2 ) - ( fontImage.getHeight() / 2 );
	    gfx.drawImage( fontImage, x, y, null );
		}
	}
	
}
//==============================================================================
