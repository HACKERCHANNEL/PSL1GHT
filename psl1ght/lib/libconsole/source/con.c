#include "psl1ght/console.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include "font.h"

extern const uint8_t font[];

static u32 foreground_color = 0xFFFFFFFF;
static u32 background_color = 0x00000000;

// Where the text will be drawn
static int cur_x = 0;
static int cur_y = 0;
static u32* framebuffer = NULL;

#define PRINTF_BUFFER_SIZE 4096
static char* printf_buffer = NULL;

// This is just copypasta from the videoTest sample
static gcmContextData *context; // Context to keep track of the RSX buffer.
static VideoResolution res; // Screen Resolution
static int currentBuffer = 0;
static u32* buffer[2]; // The buffer we will be drawing into.

static void waitFlip() 
{ 
	// Block the PPU thread untill the previous flip operation has finished.
	while(gcmGetFlipStatus() != 0) 
		usleep(200);
	gcmResetFlipStatus();
}

static void flip(s32 buffer) 
{
	assert(gcmSetFlip(context, buffer) == 0);
	realityFlushBuffer(context);
	gcmSetWaitFlip(context); // Prevent the RSX from continuing until the flip has finished.
}

// Initilize everything. You can probally skip over this function.
static void init_screen() 
{
	// Allocate a 1Mb buffer, alligned to a 1Mb boundary to be our shared IO memory with the RSX.
	void *host_addr = memalign(1024*1024, 1024*1024);
	assert(host_addr != NULL);

	// Initilise Reality, which sets up the command buffer and shared IO memory
	context = realityInit(0x10000, 1024*1024, host_addr); 
	assert(context != NULL);

	VideoState state;
	assert(videoGetState(0, 0, &state) == 0); // Get the state of the display
	assert(state.state == 0); // Make sure display is enabled

	// Get the current resolution
	assert(videoGetResolution(state.displayMode.resolution, &res) == 0);
	
	// Configure the buffer format to xRGB
	VideoConfiguration vconfig;
	memset(&vconfig, 0, sizeof(VideoConfiguration));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = res.width * 4;

	assert(videoConfigure(0, &vconfig, NULL, 0) == 0);
	assert(videoGetState(0, 0, &state) == 0); 

	s32 buffer_size = 4 * res.width * res.height; // each pixel is 4 bytes
	printf("buffers will be 0x%x bytes\n", buffer_size);
	
	gcmSetFlipMode(GCM_FLIP_VSYNC); // Wait for VSYNC to flip

	// Allocate two buffers for the RSX to draw to the screen (double buffering)
	buffer[0] = rsxMemAlign(16, buffer_size);
	buffer[1] = rsxMemAlign(16, buffer_size);
	assert(buffer[0] != NULL && buffer[1] != NULL);

	u32 offset[2];
	assert(realityAddressToOffset(buffer[0], &offset[0]) == 0);
	assert(realityAddressToOffset(buffer[1], &offset[1]) == 0);
	
	// Setup the display buffers
	assert(gcmSetDisplayBuffer(0, offset[0], res.width * 4, res.width, res.height) == 0);
	assert(gcmSetDisplayBuffer(1, offset[1], res.width * 4, res.width, res.height) == 0);

	gcmResetFlipStatus();
	flip(1);
}

static void con_putc( u32* buffer, int x, int y, u8 chr )
{
	u8 c = font[chr * FONT_HEIGHT];
	
	int idx;
	for( idx = 0; idx < FONT_HEIGHT; idx++ )
	{
		int shift;
		for( shift = 0; shift < 8; shift++ )
		{
			if( c & (1 << shift) )
				buffer[y * res.width + x + (7 - shift)] = foreground_color;
			else
				buffer[y * res.width + x + (7 - shift)] = background_color;
		}
		
		y++;
		
		c = font[(chr * FONT_HEIGHT) + idx];
	}	
}

static int con_write( const char* buf, int n )
{
	int idx;
	for( idx = 0; buf[idx] != 0 || idx < n; idx++ )
	{
		if( buf[idx] == '\n' )
		{
			cur_y += FONT_HEIGHT;
			cur_x = 0;
		}
		else
		{
			con_putc( framebuffer, cur_x, cur_y, buf[idx] );
			cur_x += FONT_WIDTH;
		}
		
		if( cur_x >= res.width )
		{
			cur_x = 0;
			cur_y += FONT_HEIGHT;
		}

		if( cur_y >= ( res.height - (res.height % FONT_HEIGHT) ) )
		{
			// Draw text on the last line
			cur_y -= FONT_HEIGHT;
			cur_x = 0;
				
			// Scroll the front buffer
			memcpy( framebuffer, ( (u8*)framebuffer + ( FONT_HEIGHT * res.width * 4 ) ),
					res.width * ( res.height - ( res.height % FONT_HEIGHT ) ) * 4 );
		}
	}

	// Blit the internal buffer
	memcpy( buffer[currentBuffer], framebuffer, res.width * res.height * 4 );

	flip( currentBuffer );
	currentBuffer ^= 1;
	waitFlip();
	
	return idx;
}

int con_printf( const char* format, ... )
{
	// Clear the printf buffer
	memset( printf_buffer, 0, PRINTF_BUFFER_SIZE );
	
	va_list args;
	
	va_start( args, format );
	
	int ret = vsprintf( printf_buffer,format, args );
	con_write( printf_buffer, ret );
	
	va_end( args );
	
	return ret;
}

// Note: X and Y are in CHARACTERS
void con_set_xy( int x, int y )
{
	cur_x = x * FONT_WIDTH;
	cur_y = y * FONT_HEIGHT;
}

void con_get_xy( int* x, int* y )
{
	if( x )
		*x = cur_x * FONT_WIDTH;
	
	if( y )
		*y = cur_y * FONT_HEIGHT;
}

void con_get_dimensions( int* width, int* height )
{
	if( width )
		*width = res.width / FONT_WIDTH;
	
	if( height )
		*height = res.height / FONT_HEIGHT;
}

void con_set_foreground_color( u32 color )
{
	foreground_color = color;
}

void con_set_background_color( u32 color )
{
	background_color = color;
}

void con_clear()
{
	// Reset cursor location
	cur_x = 0;
	cur_y = 0;
	
	memset( framebuffer, 0, res.width * res.height * 4 );
}

void con_shutdown()
{
	free( framebuffer );
	free( printf_buffer );
}

int con_init()
{
	// Initialize the framebuffer
	init_screen();
	
	// Allocate a buffer used for internal drawing operations
	framebuffer = (u32*)malloc( res.width * res.height * 4 );
	if( !framebuffer )
		return -1;
		
	printf_buffer = (char*)malloc( PRINTF_BUFFER_SIZE );
	if( !printf_buffer )
		return -2;

	return 0;
}
