
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

EXTERN_BEGIN

typedef struct t_con_dimensions
{
	u32 height;
	u32 width;
} t_con_dimensions;

// Macro for specifying colors easily
#define RGB( r, g, b )	(u32)( (r << 16) | (g << 8) | (b)  )

/* Default Foreground & Background Color */
#define DEFAULT_FG_COLOR (0xFFFFFFFF)
#define DEFAULT_BG_COLOR (0x00000000)

// Console printf, behaves exactly the same as regular printf,
// only outputting to the framebuffer
s32 con_printf(void* con_handle, const char* format, ... );

// Console printf_xy, printf at specified X & Y character position,
// only outputting to the framebuffer
s32 con_printf_xy(void* con_handle, const u32 x, const u32 y, const char* format, ... );

// Set cursor coordinates
// NOTE: Coordinates are in characters
void con_cursor_xy_set(void* con_handle, const u32 x, const u32 y);

// Get cursor coordinates
// NOTE: Coordinates are in characters
void con_cursor_xy_get(void* con_handle, u32* x, u32* y);

// Gets the console dimensions in characters
void con_dimensions_get(void* con_handle, u32* width, u32* height);

// Gets the current used font size (width & height are in pixels)
void con_fontsize_get(void* con_handle, u32* width, u32* height);

// Set foreground color, in xRGB format
void con_fgcolor_set(void* con_handle, const u32 color);

// Set background color, in xRGB format
void con_bgcolor_set(void* con_handle, const u32 color);

// Clear the console framebuffer & reset the cursor position to default position (on top left)
void con_clear(void* con_handle);

/* Get console FrameBuffer address
Can be used to manage yourself and replace the con_draw() to blit the console frame buffer to screen 
*/
u32* con_framebuffer_get(void* con_handle);

/* Basicly draw to screen the console frame buffer 
screen_width : Destination screen_buffer width
screen_height : Destination screen_buffer height
*/
void con_draw(void* con_handle, u32* screen_buffer, const u32 screen_width, const u32 screen_height);

// Initializes the console
// Returns NULL on error or con_handle pointer on success.
void* con_init(const t_con_dimensions* con_dimensions);

// Shuts down the console, freeing memory
void con_exit(void* con_handle);

EXTERN_END
