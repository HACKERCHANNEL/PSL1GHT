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
	
	sleep( 15 );
	
	// It's a good habit to shutdown the console after you're done.
	con_shutdown();
	
	return 0;
}
