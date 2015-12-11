//==============================================================================
package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.DisplayMode;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.GraphicsEnvironment;
import java.awt.Insets;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.AbstractAction;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JColorChooser;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.ScrollPaneConstants;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.filechooser.FileNameExtensionFilter;

import fontimage.SlagFontImage;

//==============================================================================
/**
 * Frame for controlling, viewing, and saving SlagFontImages.
 */
//==============================================================================
public class MainFrame extends JFrame 
		implements ActionListener, ItemListener, ChangeListener, KeyListener {

	//.................................................frame constants
	public static final boolean isDebugging = false;
//	public static final boolean isDebugging = true;
	
	private static final long serialVersionUID = 1L;
	private static final String APP_TITLE = "iShot the Serif";
	public static final Dimension MIN_SIZE = new Dimension( 896, 480 );
	
	public static final Font DEFAULT_FONT = 
			new Font( "Georgia", Font.PLAIN, 14 );
	
	//.................................................GUI components
	private JPanel fontControlStrip;
	
		private JButton stringButton;	
		private JButton colorButton;	
	
		private JPanel fontNameStrip;
			private JComboBox fontNameComboBox;
			private JButton prevFontNameButton;
			private JButton nextFontNameButton;
			private final Font FONT_CHOOSER_FONT = 
				new Font( "Arial", Font.PLAIN, 12 );
	
		private JSpinner fontSizeSpinner;	
		public final int FONT_MAX_SIZE = 120;
		public final int FONT_MIN_SIZE = 2;
		public final int FONT_SIZE_STEP = 2;
		
		private JPanel fontStyleStrip;
			private JCheckBox boldFontStyleCheckBox;
			private JCheckBox italicFontStyleCheckBox;

		private JButton saveButton;	
			
	private JScrollPane viewScroller;
	private FontImageViewer fontViewer;

	//.................................................font building data
	private Font currentFont = DEFAULT_FONT;
	private int imageType = SlagFontImage.DEFAULT_IMAGE_TYPE;
	private String fontString = SlagFontImage.DEFAULT_FONT_STRING;
	private Color fontColor = SlagFontImage.DEFAULT_FONT_COLOR;
	private RenderingHints renderingHints = getQualityRenderingHints();
	
	private String workingDirectory = null;
	
	
	//--------------------------------------------------------------------------
	/**
	 * Constructor for the app's main frame. Creates gui components, 
	 * centers the app on the screen, and creates a view of 
	 * a font image based on the default font
	 * 
	 *///-----------------------------------------------------------------------
	public MainFrame(){
		super( APP_TITLE );

		//set up the frame
		setFocusable( true );
		setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
		setPreferredSize( MIN_SIZE );

		//create the GUI and pack it up
		createComponents();
		pack();

		centerOnScreen();
		setVisible( true );
		
		//in order to set the viewer's font, we need to get a Graphics obj from it
		//	to do that we need to set the frame visible _FIRST_
		//	...we'll also store the rendering hints here
		Graphics2D gfx = (Graphics2D)getGraphics();
		gfx.setRenderingHints( renderingHints );
		
		fontViewer.setFontImage( new SlagFontImage( currentFont, 
				imageType, fontString, fontColor, renderingHints ).getImage() );
		
		addKeyListener( this );
	}

	//--------------------------------------------------------------------------
	/**
	 * Builds a set of RenderingHints which focus on shape drawing quality
	 * and antialiasing (instead of speed)
	 *///-----------------------------------------------------------------------
	private RenderingHints getQualityRenderingHints(){
		
		RenderingHints rh = new RenderingHints(
						RenderingHints.KEY_ANTIALIASING, 
						RenderingHints.VALUE_ANTIALIAS_ON );
		rh.put( RenderingHints.KEY_ALPHA_INTERPOLATION, 
						RenderingHints.VALUE_ALPHA_INTERPOLATION_QUALITY );
		rh.put( RenderingHints.KEY_COLOR_RENDERING, 
						RenderingHints.VALUE_COLOR_RENDER_QUALITY );
		rh.put( RenderingHints.KEY_RENDERING, 
						RenderingHints.VALUE_RENDER_QUALITY );
		rh.put( RenderingHints.KEY_DITHERING, 
						RenderingHints.VALUE_DITHER_DISABLE );
		rh.put( RenderingHints.KEY_FRACTIONALMETRICS, 
						RenderingHints.VALUE_FRACTIONALMETRICS_ON );
		rh.put( RenderingHints.KEY_INTERPOLATION, 
						RenderingHints.VALUE_INTERPOLATION_BICUBIC );
		
		return rh;
	}
	
	//--------------------------------------------------------------------------
	/**
	 * Centers the entire JFrame on the user's screen - untested on multiple
	 * monitors
	 *///-----------------------------------------------------------------------
	private void centerOnScreen(){
		//precondition: the width and height are already set properly
		
		// subtract half the frame dimensions from the screen's center 
		DisplayMode currentMode = 
			getGraphicsConfiguration().getDevice().getDisplayMode();
		setBounds( 
			( currentMode.getWidth() / 2 ) - ( getWidth() / 2 ),
			( currentMode.getHeight() / 2 ) - ( getHeight() / 2 ),
			getWidth(), getHeight() );
	}
		
	//==========================================================================
	// GUI CREATION
	//==========================================================================
	/**
	 * Creates the control strip and viewer inside the main frame
	 *///-----------------------------------------------------------------------
	private void createComponents(){

		getContentPane().setLayout( 
				new BoxLayout( getContentPane(), BoxLayout.Y_AXIS ) );
		
		getContentPane().add( createFontControlStrip() );
		
		fontViewer = new FontImageViewer();		
		debugln( "fontViewer size = " + fontViewer.getSize() );
		viewScroller = new JScrollPane( fontViewer,
			ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,
			ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED
		);
		getContentPane().add( viewScroller );
		
		debugln( "viewScroller size = " + viewScroller.getSize() );
		debugln( "contentPane size = " + getContentPane().getSize() );
		debugln( "frame size = " + getSize() );
	}
	
	//--------------------------------------------------------------------------
	/**
	 * creates the fontString and fontColor buttons, 
	 * 	the name strip, the size spinner, the style strip, and the save button
	 * 	all within the font control strip
	 * @return the JPanel containing the fontstring and color controls
	 *///-----------------------------------------------------------------------
	public JPanel createFontControlStrip(){

		fontControlStrip = new JPanel();
		fontControlStrip.setMaximumSize( new Dimension( Integer.MAX_VALUE, 32 ) );
		fontControlStrip.setLayout( 
				new BoxLayout( fontControlStrip, BoxLayout.X_AXIS ) );
		
		stringButton = new JButton( setFontString );
		stringButton.setMargin( new Insets( 2, 2, 2, 2 ) );
		fontControlStrip.add( stringButton );		
		
		colorButton = new JButton( setFontColor );
		colorButton.setMargin( new Insets( 2, 2, 2, 2 ) );
		fontControlStrip.add( colorButton );		
		
		fontControlStrip.add( createFontNameStrip() );
		fontControlStrip.add( createFontSizeSpinner() );
		fontControlStrip.add( createFontStyleStrip() );
		
		saveButton = new JButton( saveFontImage );
		saveButton.setMargin( new Insets( 2, 2, 2, 2 ) );
		fontControlStrip.add( saveButton );		
		
		return fontControlStrip;
	}

	//--------------------------------------------------------------------------
	/**
	 * creates the previous and next font name buttons and the combo box
	 * 	containing all the font name choices
	 * @return the JPanel containing the font name controls
	 *///-----------------------------------------------------------------------
	public JPanel createFontNameStrip(){
		fontNameStrip = new JPanel();
	
		fontNameStrip.setLayout( 
			new BoxLayout( fontNameStrip, BoxLayout.X_AXIS ) );
		
		prevFontNameButton = new JButton( prevFontName );
		prevFontNameButton.setMargin( new Insets( 2, 2, 2, 2 ) );
		fontNameStrip.add( prevFontNameButton );
		
		fontNameComboBox = new JComboBox( getAvailableFontFamilyNames() );
		fontNameComboBox.setSelectedItem( currentFont.getName() );
		fontNameComboBox.setFont( FONT_CHOOSER_FONT );
		fontNameComboBox.setToolTipText( 
			"Set the font name here" 
			+ " or use the previous and next font buttons" 
			+ " to the left of right of this control" );
		
		fontNameComboBox.addItemListener( this );
		fontNameStrip.add( fontNameComboBox );
		
		nextFontNameButton = new JButton( nextFontName );
		nextFontNameButton.setMargin( new Insets( 2, 2, 2, 2 ) );
		fontNameStrip.add( nextFontNameButton );
		
		return fontNameStrip;
	}
	
	//--------------------------------------------------------------------------
	/**
	 * creates the font size spinner model (which uses FONT_MIN/MAX/STEP)
	 * 	and places that inside the fontSizeSpinner
	 * @return the JSpinner control created 
	 *///-----------------------------------------------------------------------
	public JSpinner createFontSizeSpinner(){
		
		SpinnerNumberModel model = new SpinnerNumberModel( 
			new Integer( currentFont.getSize() ), 
			new Integer( FONT_MIN_SIZE ), new Integer( FONT_MAX_SIZE ), 
			new Integer( FONT_SIZE_STEP ) );
		fontSizeSpinner = new JSpinner( model );
		fontSizeSpinner.setToolTipText( 
				"Increase (Ctrl+Up) or decrease (Ctrl+Down) the font size");
		
		fontSizeSpinner.addChangeListener( this );
		fontSizeSpinner.addKeyListener( this );
			
		return fontSizeSpinner;
	}
	
	//--------------------------------------------------------------------------
	/**
	 * creates the bold and italic checkboxes to control the font's style
	 * @return the panel containing the bold italic checkboxes
	 *///-----------------------------------------------------------------------
	public JPanel createFontStyleStrip(){
		
		fontStyleStrip = new JPanel();
		fontStyleStrip.setLayout( 
			new BoxLayout( fontStyleStrip, BoxLayout.X_AXIS ) );

		boldFontStyleCheckBox = new JCheckBox( "Bold" );
		if( currentFont.isBold() ) boldFontStyleCheckBox.setSelected( true );
		boldFontStyleCheckBox.setMargin( new Insets( 2, 2, 2, 2 ) );
		boldFontStyleCheckBox.setToolTipText( "Make this font bold (Ctrl+b)" );
		
		boldFontStyleCheckBox.addItemListener( this );
		fontStyleStrip.add( boldFontStyleCheckBox );
		
		italicFontStyleCheckBox = new JCheckBox( "Italic" );
		if( currentFont.isItalic() ) italicFontStyleCheckBox.setSelected( true );
		italicFontStyleCheckBox.setMargin( new Insets( 2, 2, 2, 2 ) );
		italicFontStyleCheckBox.setToolTipText( "Make this font bold (Ctrl+i)" );
		
		italicFontStyleCheckBox.addItemListener( this );
		fontStyleStrip.add( italicFontStyleCheckBox );
			
		return fontStyleStrip;
	}
	
	//--------------------------------------------------------------------------
	/**
	 * shows a dialog to alter the fontString and return a new one (if any)
	 *///-----------------------------------------------------------------------
	public void fontStringDialog(){
		String s = (String)JOptionPane.showInputDialog( this, 
      "Enter a new string for the font image to use:"
				//an tremendous and horrible hack - that works
				+ "                                            " 
				+ "                                            ", 
      "Set the Font String",
      JOptionPane.PLAIN_MESSAGE, null, null, fontString );
		
		if( ( s != null ) && ( !s.equals( "" ) ) ){ 
			fontString = s; setNewFontFromParams();
		}
		requestFocus();
	}
	
	//--------------------------------------------------------------------------
	/**
	 * shows a color chooser to alter the fontColor and return a new one (if any)
	 *///-----------------------------------------------------------------------
	public void showFontColorChooser(){
		Color newColor = JColorChooser.showDialog( this, "Font Color", fontColor );
		if( newColor != null ){ 
			fontColor = newColor; setNewFontFromParams();
		}
		requestFocus();
	}
	
	//==========================================================================
	// RENDERING/FONT ACTIONS AND HELPER METHODS
	//==========================================================================
	/**
	 * @return a list of all font names available on the given environment
	 *///-----------------------------------------------------------------------
	public String[] getAvailableFontFamilyNames(){
		return GraphicsEnvironment.getLocalGraphicsEnvironment().
			getAvailableFontFamilyNames();
	}
		
	//--------------------------------------------------------------------------
	/**
	 * @return a filename string of the form: 
	 * 	"font_[fontName]_[pixelHeight]<_bold><_italic>.png"
	 *///-----------------------------------------------------------------------
	public String createDefaultFontFilename(){
    String defaultFilename = "font_" + currentFont.getName() 
    	+ "_" + fontViewer.getFontImage().getHeight() 
			+ ( ( currentFont.isBold() )?   ( "_bold" )  :( "" ) )
			+ ( ( currentFont.isItalic() )? ( "_italic" ):( "" ) )
    	+ ".png";
    return defaultFilename;
	}
		
	//--------------------------------------------------------------------------
	/**
	 * gets a new font image using the frame params:
	 * 	a new currentFont (gathered from the fontControlStrip),
	 *  imageType, fontString, fontColor, renderingHints (from the MainFrame)
	 *///-----------------------------------------------------------------------
	public void setNewFontFromParams(){

		String fontName = (String)fontNameComboBox.getSelectedItem();
		int fontSize = (Integer)fontSizeSpinner.getValue();
		int fontStyle = 
			( ( boldFontStyleCheckBox.isSelected() )?   ( Font.BOLD )  :( 0 ) ) | 
			( ( italicFontStyleCheckBox.isSelected() )? ( Font.ITALIC ):( 0 ) );
		
		debugln( "\nnew font: " + fontName + "," + fontSize + "," + fontStyle );
		currentFont = new Font( fontName, fontStyle, fontSize );
		
		fontViewer.setFontImage( new SlagFontImage( currentFont, 
				imageType, fontString, fontColor, renderingHints ).getImage() );
		
		viewScroller.getViewport().setView( fontViewer );
		requestFocus();
	}
	
	//--------------------------------------------------------------------------
	/**
	 * move backward in combobox font names, wrapping at beginning
	 *///-----------------------------------------------------------------------
	public void usePrevFontName(){		
		int index = fontNameComboBox.getSelectedIndex();
		debugln( "fontNameComboBox.index = " + index );
		
		if( index <= 0 ){
			fontNameComboBox.setSelectedIndex( fontNameComboBox.getItemCount() - 1 );
		} else {
			fontNameComboBox.setSelectedIndex( index - 1 );
		}
	}
	
	//--------------------------------------------------------------------------
	/**
	 * move forward in combobox font names, wrapping at end
	 *///-----------------------------------------------------------------------
	public void useNextFontName(){
		int index = fontNameComboBox.getSelectedIndex();
		debugln( "fontNameComboBox.index = " + index );
		
		if( ( index < 0 ) || 
				( index >= ( fontNameComboBox.getItemCount() - 1 ) ) ){
			fontNameComboBox.setSelectedIndex( 0 );
		} else {
			fontNameComboBox.setSelectedIndex( index + 1 );
		}
	}
	
	//--------------------------------------------------------------------------
	/**
	 * uses fontSizeSpinner's model to increase the displayed font size
	 * up to FONT_MAX_SIZE
	 *///-----------------------------------------------------------------------
	public void increaseFontSize(){
		int thisSpinnerVal = (Integer)fontSizeSpinner.getValue();
		Integer nextSpinnerVal = (Integer)fontSizeSpinner.getNextValue();
		if( nextSpinnerVal == null ){ return; }
		
		if( ( thisSpinnerVal < FONT_MAX_SIZE ) && 
				( (int)nextSpinnerVal <= FONT_MAX_SIZE ) ){
			fontSizeSpinner.setValue( (int)nextSpinnerVal );	
		}
	}
	
	//--------------------------------------------------------------------------
	/**
	 * uses fontSizeSpinner's model to decrease the displayed font size
	 * down to FONT_MIN_SIZE
	 *///-----------------------------------------------------------------------
	public void decreaseFontSize(){
		int thisSpinnerVal = (Integer)fontSizeSpinner.getValue();
		Integer prevSpinnerVal = (Integer)fontSizeSpinner.getPreviousValue();
		if( prevSpinnerVal == null ){ return; }
		
		if( ( thisSpinnerVal > FONT_MIN_SIZE ) && 
				( (int)prevSpinnerVal >= FONT_MIN_SIZE ) ){
			fontSizeSpinner.setValue( (int)prevSpinnerVal );	
		}
	}
	
	//--------------------------------------------------------------------------
	/**
	 * toggles....bold
	 *///-----------------------------------------------------------------------
	public void toggleBold(){
		boolean currentSetting = boldFontStyleCheckBox.isSelected();
		boldFontStyleCheckBox.setSelected( !currentSetting );
	}
	
	//--------------------------------------------------------------------------
	/**
	 * toggles...wait for it.........italic
	 *///-----------------------------------------------------------------------
	public void toggleItalic(){
		boolean currentSetting = italicFontStyleCheckBox.isSelected();
		italicFontStyleCheckBox.setSelected( !currentSetting );
	}
	
	//--------------------------------------------------------------------------
	/**
	 * saves the current font configuration (currentFont, imageType, 
	 * 	fontString, fontColor, renderingHints) to a file chosen by the user
	 *  from a JFileChooser. Generates overwrite dialog given an existing file
	 *  and preserves the directory saved to as a 'working directory' 
	 *///-----------------------------------------------------------------------
	public void saveCurrentFontImageFile(){
		
    JFileChooser chooser;
    // check if we've already set a working dir and use that if there
    // otherwise, use the app dir
    if( workingDirectory != null ) chooser = new JFileChooser( workingDirectory );
    else chooser = new JFileChooser( "." );

    // (NOTE: we're defaulting to PNG files)
    chooser.setApproveButtonText( "Save" );
    chooser.setDialogTitle( "Saving your Font File" );
    chooser.setFileFilter( 
    	new FileNameExtensionFilter( "PNG Images", "png" ) );

   	chooser.setSelectedFile( new File( createDefaultFontFilename() ) );
    
    if( chooser.showSaveDialog( this ) == JFileChooser.APPROVE_OPTION ){
    	File file = chooser.getSelectedFile();
    	// handle an already existing file with an overwrite prompt
    	if( file.exists() ){
    		
    		String[] options = { "Overwrite", "Cancel" };
    		int choice = JOptionPane.showOptionDialog( this,
    		    "The file " + file.getName() + "already exists.\n"
    		    + "Do you want to overwrite the file?\n",
    		    "That File Exists Already!",
    		    JOptionPane.YES_NO_OPTION, 
    		    JOptionPane.QUESTION_MESSAGE,
    		    null, options, options[ 1 ] );

    		// if the user opted out of the overwrite
    		if( ( choice == JOptionPane.CLOSED_OPTION ) || 
    				( choice == JOptionPane.NO_OPTION ) ){
    			// return focus to the frame and bail
    			requestFocus(); return;
    		}
    	}
    	
    	// attempt to write the file
    	try {
    		BufferedImage fontImage = new SlagFontImage( 
  				currentFont, imageType, fontString, fontColor, 
  				renderingHints ).getImage();
    		ImageIO.write( fontImage, "png", file );
	    	
    	} catch( IOException e ){
    		// handle IO errors with a dialog
    		JOptionPane.showMessageDialog( this,
    		    "There was an error writing the file '" 
    				+ file.getName() + "': "
    				+ e.getLocalizedMessage(), 
    		    "Error Writing Font Image", JOptionPane.ERROR_MESSAGE );
    		debugln( "error writing to file " + chooser.getSelectedFile() );
    	}
    	
    	// cache the directory chosen to be used next time
    	workingDirectory = file.getPath();
    	debugln( "workingDirectory now = " + workingDirectory );
    }
    
    requestFocus(); 
	}
	
	//==========================================================================
	// ACTION EVENTS & SUBCLASSES
	//==========================================================================
	/**
	 * parent class to all the actions below
	 *///-----------------------------------------------------------------------
	class FontViewerFrameAction extends AbstractAction {
		private static final long serialVersionUID = 1L;
		
		public FontViewerFrameAction( String text, String tip, Integer mnemonic ) {
			super( text );
			putValue( SHORT_DESCRIPTION, tip );
			putValue( MNEMONIC_KEY, mnemonic );
		}
		public void actionPerformed( ActionEvent e ){}
	}	
	
	//--------------------------------------------------------------------------
	private FontViewerFrameAction setFontString = new FontViewerFrameAction( 
			"Font String", "Set the string of characters used (Ctrl+t)", null ){
		private static final long serialVersionUID = 1L;
		public void actionPerformed( ActionEvent e ){ fontStringDialog(); }
	};
	
	//--------------------------------------------------------------------------
	private FontViewerFrameAction setFontColor = new FontViewerFrameAction( 
			"Font Color", "Set the color used for drawing (Ctrl+c)", null ){
		private static final long serialVersionUID = 1L;
		public void actionPerformed( ActionEvent e ){ showFontColorChooser(); }
	};
	
	//--------------------------------------------------------------------------
	private FontViewerFrameAction prevFontName = new FontViewerFrameAction( 
			"◄", "Previous Font (Ctrl+Left)", null ){
		private static final long serialVersionUID = 1L;
		public void actionPerformed( ActionEvent e ){ usePrevFontName(); }
	};
	
	//--------------------------------------------------------------------------
	private FontViewerFrameAction nextFontName = new FontViewerFrameAction( 
			"►", "Next Font (Ctrl+Right)", null ){
		private static final long serialVersionUID = 1L;
		public void actionPerformed( ActionEvent e ){ useNextFontName(); }
	};
	
	//--------------------------------------------------------------------------
	private FontViewerFrameAction saveFontImage = new FontViewerFrameAction( 
			"Save", "Save this font as an image (Ctrl+s)", null ){
		private static final long serialVersionUID = 1L;
		public void actionPerformed( ActionEvent e ){ saveCurrentFontImageFile(); }
	};
	
	//--------------------------------------------------------------------------
	public void actionPerformed( ActionEvent action ){}
	
	//==========================================================================
	// STATE CHANGE HANDLERS & KEY LISTENERS
	//==========================================================================
	public void itemStateChanged( ItemEvent item ){
		setNewFontFromParams();
		requestFocus();
	}
	
	//--------------------------------------------------------------------------
	public void stateChanged( ChangeEvent change ){
		setNewFontFromParams();
		requestFocus();
	}
	
	//--------------------------------------------------------------------------
	public void keyPressed( KeyEvent key ){
		// should probably use constants here...
		
		if( key.isControlDown() ){
			if( key.getKeyCode() == KeyEvent.VK_LEFT ){ 	usePrevFontName(); }
			if( key.getKeyCode() == KeyEvent.VK_RIGHT ){ 	useNextFontName(); }
			if( key.getKeyCode() == KeyEvent.VK_UP ){ 		increaseFontSize(); }
			if( key.getKeyCode() == KeyEvent.VK_DOWN ){ 	decreaseFontSize(); }
			if( key.getKeyCode() == KeyEvent.VK_B ){ 			toggleBold(); }
			if( key.getKeyCode() == KeyEvent.VK_I ){ 			toggleItalic(); }
			if( key.getKeyCode() == KeyEvent.VK_S ){ 		saveCurrentFontImageFile(); }
			if( key.getKeyCode() == KeyEvent.VK_C ){ 		showFontColorChooser(); }
			if( key.getKeyCode() == KeyEvent.VK_T ){ 		fontStringDialog(); }
		}
	}
	
	//--------------------------------------------------------------------------
	public void keyReleased( KeyEvent key ){}
	public void keyTyped( KeyEvent key ){}

	//==========================================================================
	/**
	 * returns formatted string of a set of rendering hints
	 *///-----------------------------------------------------------------------
	public static String listRenderingHints( RenderingHints hints ){
		
		String returnString = "";
		for( Object keyAsObj : hints.keySet() ){
			RenderingHints.Key key = (RenderingHints.Key)keyAsObj;
			Object valObj = hints.get( key );
			returnString += "" + key + ":" + valObj + "\n";
		}
		return returnString;
	}
	
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
	
	//==========================================================================
	/**
	 * you should already know what this is
	 *///-----------------------------------------------------------------------
	public static void main( String[] args ){
		javax.swing.SwingUtilities.invokeLater( new Runnable(){
      public void run(){
    		new MainFrame();
      }
		} );
	}

}
//==============================================================================
