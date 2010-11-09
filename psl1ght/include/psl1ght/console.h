// fbcon.h - header file for the framebuffer console
#ifndef FBCON_H
#define FBCON_H

// Font dimensions
#define FONT_WIDTH 	8
#define FONT_HEIGHT 16

// Macro for specifying colors easily
#define RGB( r, g, b )	(u32)( (r << 16) | (g << 8) | (b)  )

// Console printf, behaves exactly the same as regular printf,
// only outputting to the framebuffer
int con_printf( const char* format, ... );

// Set cursor coordinates
// NOTE: Coordinates are in characters!
void con_set_xy( int x, int y );

// Get cursor coordinates
// NOTE: Coordinates are in characters!
void con_get_xy( int* x, int* y );

// Gets the console dimensions
void con_get_dimensions( int* width, int* height );

// Set foreground color, in xRGB format
void con_set_foreground_color( unsigned int color );

// Set background color, in xRGB format
void con_set_background_color( unsigned int color );

// Clear the console
void con_clear();

// Initializes the console
// Returns < 0 on error.
int con_init();

// Shuts down the console, freeing some memory
// Not mandatory but good behaviour anyway.
void con_shutdown();

#endif
