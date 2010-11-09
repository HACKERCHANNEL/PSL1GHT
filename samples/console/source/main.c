#include <psl1ght/lv2.h>
#include <psl1ght/console.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
	// Initialize the console.
	// This must always be done as it initializes the RSX framebuffer.
	con_init();
	
	// con_printf works exactly as you'd expect from printf.
	con_printf( "Hello World!\n" );
	
	// You can also get the dimensions of the console
	int w, h;
	con_get_dimensions( &w, &h );
	con_printf( "This console has %d columns and %d rows.\n", w, h );
	
	// Color changing is also supported. The RGB macro is provided
	// for convenience (see console.h)
	con_set_foreground_color( RGB( 0, 255, 0 ) );
	con_printf( "This text is green.\n" );
	
	// You can manipulate the coordinates the text will be drawn to.
	// The coordinates are specified in characters.
	con_set_xy( 10, 10 );
	con_printf( "This text is faaaaar away from the rest.\n" );
	
	// Wait 10 seconds so you can actually see something.
	sleep( 10 );
	
	// It's a good habit to shutdown the console after you're done.
	// It frees some memory used internally by the library.
	con_shutdown();
	
	return 0;
}
