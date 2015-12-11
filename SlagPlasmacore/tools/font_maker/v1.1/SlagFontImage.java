//package ishottheserif;

//==============================================================================
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.Shape;
import java.awt.font.FontRenderContext;
import java.awt.font.GlyphVector;
import java.awt.geom.AffineTransform;
import java.awt.geom.Line2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.HashMap;

//import ishottheserif.MainFrame;

//==============================================================================
/**
 * Class that holds: a hashMap of chars to shapes (the glyph shapes),
 * 	the dimensions of each glyph, the dimensions of the font as a whole,
 *  and the image drawn with that info
 */
//==============================================================================
public class SlagFontImage {

	public static final boolean isDebugging = false;
//	public static final boolean isDebugging = true;
	
	public static final int DEFAULT_IMAGE_TYPE = BufferedImage.TYPE_INT_ARGB;
	
	// default set of characters that will be rendered to the font png
	public static final String DEFAULT_FONT_STRING = 
		" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ" + 
		"[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	
	public static final Color DEFAULT_FONT_COLOR = Color.white;
	public static final Color DEFAULT_STRIPE_COLOR_A = Color.green;
	public static final Color DEFAULT_STRIPE_COLOR_B = Color.red;
	
	private BufferedImage image = null;
	
	//=========================================================================
	/**
	 * A hashmap containing all the shapes of glyphs of characters in a 
	 * given string, as well as their dimensions, and the height, ascent,
	 * and descent of the font used to draw them
	 * -! isFixedWidth is pretty much broken (or not working as intended)
	 */
	//=========================================================================
	public class FontCharacterShapeMap
			extends HashMap<Character,FontCharacterShape> {		
		
		private static final long serialVersionUID = 1L;
		private int height = 0;
		private int ascent = 0;
		private int descent = 0;

		boolean isFixedWidth = true;
		
		//--------------------------------------------------------------------
		/**
		 * creates the hashmap with the given string, font, and gfxcontext
		 *///-----------------------------------------------------------------
		public FontCharacterShapeMap( 
				String fontStr, Font font, Graphics2D gfx, int leftPad, int rightPad ){

			// get the dimensions common to the font as a whole
			FontMetrics metrics = gfx.getFontMetrics( font );
			height = metrics.getHeight();
			ascent = metrics.getAscent();
			descent = metrics.getDescent();

			// no chars to place in map, so stop here
			if( fontStr.length() == 0 ){ return; }
			
			// get the character dimensions and place them in the map
			//	checking the next wid v. previous to see if font is fixed width
			char c = fontStr.charAt( 0 );
			FontCharacterShape fcd = new FontCharacterShape( c, font, gfx, leftPad, rightPad );
			put( c, fcd );
			int lastWidth = fcd.getWidth();
			
			for( int i = 1; i < fontStr.length(); i++ ){
				c = fontStr.charAt( i );
				fcd = new FontCharacterShape( c, font, gfx, leftPad, rightPad );
				put( c, fcd );
				
				int thisWidth = fcd.getWidth();
				if( thisWidth != lastWidth ) isFixedWidth = false;
				lastWidth = thisWidth;
			}
		}
		
		//--------------------------------------------------------------------
		/**
		 * gets the pixel width of a string
		 *///-----------------------------------------------------------------
		public int getStringPixelWidth( String s ){
			int width = 0;
			for( int i = 0; i < s.length(); i++ ){
				char c = s.charAt( i );
				width += get( c ).getWidth();
			}
			return width;
		}
		
		//--------------------------------------------------------------------
		public int getHeight(){ return height; }
		public int getAscent(){ return ascent; }
		public int getDescent(){ return descent; }
		public boolean isFixedWidth(){ return isFixedWidth; }
	}
	//=========================================================================
	
	//=========================================================================
	/**
	 * contains the shape of a glyph of a character, its bounds, its advanced
	 * width, and any offfset to add to x when drawing
	 */
	//=========================================================================
	public class FontCharacterShape {

		private Shape shape = null;
		private Rectangle bounds = null;
		private int advancedWidth = 0;
		private int leftXOffset = 0;
		
		//--------------------------------------------------------------------
		/**
		 * what a massive bastard of an API the Java2D font/glyph/shape is
		 *///-----------------------------------------------------------------
		public FontCharacterShape( char c, Font font, Graphics2D gfx, int leftPad, int rightPad ){
			debugln( "" + c );
			
			FontMetrics metrics = gfx.getFontMetrics( font );
			advancedWidth = metrics.charWidth( c );

			FontRenderContext context = gfx.getFontRenderContext();
			GlyphVector glyph = font.createGlyphVector( context, "" + c );
			
			shape = glyph.getGlyphOutline( 0 );
			bounds = glyph.getPixelBounds( context, 0, 0 );

			// get the most accurate shape bounds we can 
			//	by rounding either up for positives or down for negatives
			//?? will width or height ever be negative??
			Rectangle2D shapeBounds = shape.getBounds2D();
			bounds = new Rectangle(
					( shapeBounds.getX() < 0 )?
							( (int)Math.floor( shapeBounds.getX() ) ):
							( (int)Math.ceil( shapeBounds.getX() ) ),
					( shapeBounds.getY() < 0 )?
							( (int)Math.floor( shapeBounds.getY() ) ):
							( (int)Math.ceil( shapeBounds.getY() ) ),
					( shapeBounds.getWidth() < 0 )?
							( (int)Math.floor( shapeBounds.getWidth() ) ):
							( (int)Math.ceil( shapeBounds.getWidth() ) ),
					( shapeBounds.getHeight() < 0 )?
							( (int)Math.floor( shapeBounds.getHeight() ) ):
							( (int)Math.ceil( shapeBounds.getHeight() ) )
			);
			debugln( "  my bounds (1): " + bounds );
			
			int boundsWidth = (int)bounds.getWidth();
			if( bounds.getX() >= 0 ){ 
				// if the x pos is positive, we need to add enough width to 
				//	the bounds to include it all (because x will push it to the right)
				boundsWidth += (int)bounds.getX();
				
			} else {
				// if x is negative, the glyph will draw to the left of the baseline
				//	(which we don't want). We'll capture a positive version of
				//	that offset and use it later in the draw method to push it
				//	to the right and make sure it's drawn inside it's width
				leftXOffset = (int)( 0 - bounds.getX() );
			}
			
			//----------------------------------------------<<<<<<<<<<<<
			if( advancedWidth >= ( boundsWidth + leftXOffset ) ){
				leftXOffset = 0;
				boundsWidth = advancedWidth;
			}

			boundsWidth = (int)Math.max( 
					boundsWidth, glyph.getPixelBounds( context, 0, 0 ).getWidth() );

      leftXOffset += leftPad;
      boundsWidth += leftPad + rightPad;

			//MAJ. CHEESY HACKS here (forwardslash is a psychopath, too)
			if( c == '/' ) boundsWidth += 1;
			
			bounds = new Rectangle( 
					(int)bounds.getX(), (int)bounds.getY(), 
					boundsWidth, (int)bounds.getHeight() );
			
			debugln( "  my bounds (2): " + bounds );
			debugln( "  pixel bounds: " + glyph.getPixelBounds( context, 0, 0 ) );
			debugln( "  shape bounds: " + shape.getBounds2D() );

			debugln( "  width: " + boundsWidth );
			debugln( "  x offset: " + bounds.getX() );
			debugln( "  adv wid: " + advancedWidth );
			debugln( "  dims: " + getWidth() + "x" + getHeight() );
		}
		
		//--------------------------------------------------------------------
		/**
		 * move the shape to a new spot using the magic of affine
		 *///-----------------------------------------------------------------
		private Shape translateTo( int x, int y ){
			int newX = x + leftXOffset;
			AffineTransform at = AffineTransform.getTranslateInstance( newX, y );
			return ( at.createTransformedShape( shape ) );
		}
		
		//--------------------------------------------------------------------
		/**
		 * fills the glyph shape
		 *///-----------------------------------------------------------------
		public void fill( Graphics2D gfx, int x, int y, Color clr ){
			Shape translatedShape = translateTo( x, y );
			gfx.setColor( clr );
			gfx.fill( translatedShape );
		}
		
		//--------------------------------------------------------------------
		/**
		 * only draws the outline of the glyph shape
		 *///-----------------------------------------------------------------
		public void outline( Graphics2D gfx, int x, int y, Color clr ){
			Shape translatedShape = translateTo( x, y );
			gfx.setColor( clr );
			gfx.draw( translatedShape );
		}

		//--------------------------------------------------------------------
		/**
		 * wrapper for filling at a spot
		 *///-----------------------------------------------------------------
		public void draw( Graphics2D gfx, int x, int y, Color clr ){
			fill( gfx, x, y, clr );
		}
		
		//--------------------------------------------------------------------
		/**
		 * draws an outline in oClr and the fill over it
		 *///-----------------------------------------------------------------
		public void draw( Graphics2D gfx, int x, int y, Color clr, Color oClr ){
			outline( gfx, x, y, oClr );
			fill( gfx, x, y, clr );
		}
		
		//--------------------------------------------------------------------
		public int getWidth(){ return (int)bounds.getWidth(); }
		public int getHeight(){ return (int)bounds.getHeight(); }
		
	}
	//=========================================================================
	
	
	//--------------------------------------------------------------------------
	public SlagFontImage( Font font, Graphics2D gfx ){
		this( font, DEFAULT_IMAGE_TYPE, 
			DEFAULT_FONT_STRING, DEFAULT_FONT_COLOR, gfx.getRenderingHints() );
	}
	public SlagFontImage( Font font, RenderingHints hints ){
		this( font, 
			DEFAULT_IMAGE_TYPE, DEFAULT_FONT_STRING, DEFAULT_FONT_COLOR, hints );
	}
	public SlagFontImage(
			Font font, int imageType,
			String fontString, Color fontColor,
			RenderingHints renderingHints )
  {
    this(font, imageType, fontString, fontColor, renderingHints, 0, 0);
  }
		
	//--------------------------------------------------------------------------
	/**
	 * creates a slag font image for your family's enjoyment
	 *///-----------------------------------------------------------------------
	public SlagFontImage(
			Font font, int imageType,
			String fontString, Color fontColor,
			RenderingHints renderingHints, int leftPad, int rightPad ){

		//in order to properly size the image for the font, we need FontMetrics
		//	in order to get FontMetrics, we need another, draft image
		//chicken.egg || egg.chicken
		BufferedImage draftImage = new BufferedImage( 8, 8, imageType );
		Graphics2D gfx = draftImage.createGraphics();
		gfx.setRenderingHints( renderingHints );
		gfx.setFont( font );

		//next, we'll gather the character dimensions 
		//	and use those to get the image size that actually holds the font
		FontCharacterShapeMap dimMap = 
			new FontCharacterShapeMap( fontString, font, gfx, leftPad, rightPad );
		
		// handle 0 height or 0 width
		int stringPixelWidth = dimMap.getStringPixelWidth( fontString );
		int fontImageHeight = dimMap.getHeight();
		if( ( stringPixelWidth <= 0 ) || ( fontImageHeight <= 0 ) ){
			image = draftImage; return;
			//TODO: lame
		}
		
		image = new BufferedImage( stringPixelWidth, fontImageHeight, imageType );
		
		//draw the basic font to the image
		image = drawFontToImage( 
				font, image, dimMap, fontString, fontColor, renderingHints );

		//if proportional, SLAG will need a proportional stripe in the image
		if( !dimMap.isFixedWidth() ){
			image = drawSLAGProportionalWidthStripe(
					image, dimMap, 
					fontString, DEFAULT_STRIPE_COLOR_A, DEFAULT_STRIPE_COLOR_B );
		}
		//otherwise, the font is fixed width and no stripe is needed
		
	}

	//--------------------------------------------------------------------------
	/**
	 * 
	 *///-----------------------------------------------------------------------
	private BufferedImage drawFontToImage( 
			Font font, BufferedImage image, FontCharacterShapeMap dimMap,
			String fontString, Color fontColor, RenderingHints renderingHints ){
		
		Graphics2D gfx = image.createGraphics();
		gfx.setRenderingHints( renderingHints );
		
		//draw a bg color for debugging porpoises
		if( MainFrame.isDebugging ){
			gfx.setColor( Color.red );
			Rectangle2D.Double rect = new Rectangle2D.Double(
					0, 0, image.getWidth(), image.getHeight() ); 
			gfx.fill(rect);
		}
		
		int x = 0;
		boolean everyOther = false;
		for( int i = 0; i < fontString.length(); i++ ){
			FontCharacterShape shape = dimMap.get( fontString.charAt( i ) );
			
			if( MainFrame.isDebugging ){
				Color bgColor = new Color( 0xFF664444 );
				if( everyOther ) bgColor = new Color( 0xFF444466 );
				
				gfx.setColor( bgColor );
				Rectangle2D.Double rect = new Rectangle2D.Double(
						x, 0, 
						shape.getWidth(), image.getHeight() ); 
				gfx.fill( rect );
			}
			everyOther = !everyOther;
			
			shape.draw( gfx, x, dimMap.getAscent(), fontColor );
			x += shape.getWidth();
		}
		
		return image;
	}
	
	//--------------------------------------------------------------------------
	/**
	 * draws the single pixel height strip of alternating colors SLAG uses
	 * 	to get the individual widths of chars in a proportional font
	 *///-----------------------------------------------------------------------
	private BufferedImage drawSLAGProportionalWidthStripe(
			BufferedImage image, FontCharacterShapeMap dimensionMap,
			String fontString, Color colorA, Color colorB ){
			
		//proportional fonts in SLAG use a single pixel stripe of two alternating
		//	colors beginning at 0,0 and ending at the top right of the font
		//	strip image in order to record the widths of each character
		
		//set up the stroke of the graphics context for a single pixel line
		//	with CAP_BUTT (line segments with square ending )
		BasicStroke stroke = new BasicStroke( 
				1.0f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER );
		Graphics2D gfx = image.createGraphics();
		gfx.setStroke( stroke );
		
		float startX = 0, endX = 0, y = 0;
		Line2D.Float line = new Line2D.Float( startX, y, endX, y );
		//cycle through the fontString getting a char
		//cycle through the charWidthMap to draw successive width recording 
		//	line segments using colorA and colorB
		for( int c = 0; c < fontString.length(); c++ ){
			
			//get the next char width - use it to get the next line endpoint
			int thisCharsWidth = dimensionMap.get( fontString.charAt( c ) ).getWidth();
			endX = startX + thisCharsWidth;
			line.setLine( startX, y, endX, y );
			
			MainFrame.debug( "thisCharsWidth: " + thisCharsWidth );
			debugln( ", line start: " + line.getX1() 
					+ ", line end: " + line.getX2() );
			
			//set up the alternating color and draw the line
			if( gfx.getColor() == DEFAULT_STRIPE_COLOR_A ){
				gfx.setColor( DEFAULT_STRIPE_COLOR_B );
			} else {
				gfx.setColor( DEFAULT_STRIPE_COLOR_A );
			}
			debugln( ", gfx.getColor() " + gfx.getColor() );
			gfx.draw( line );
			
			//set up the startX to the last endX for the next cycle
			startX = endX;
		}
		
		return image;
	}

	//--------------------------------------------------------------------------
	public BufferedImage getImage(){ return image; }
	
	//==========================================================================
	/**
	 * console debug/print statement stuff
	 *///-----------------------------------------------------------------------
	public static void debug( String msg ){
		if( isDebugging ) System.out.print( msg ); }
	
	public static void debugln( String msg ){
		if( isDebugging ) System.out.println( msg ); }
	
	public static void debugFocus( String msg ){
		System.out.println( msg ); }
	
}


