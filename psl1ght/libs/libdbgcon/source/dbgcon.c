#include <psl1ght/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>

#include <dbgcon/dbgcon.h>
#include "font.h"

#define PRINTF_BUFFER_SIZE (4096)

typedef struct
{
	u32* framebuffer;
	t_con_dimensions con_dimensions;
	u32 font_width; /* Current font Width in pixel */
	u32 font_height; /* Current font Height in pixel */
	u32 fg_color; // Foreground color
	u32 bg_color; // Background color
	u32 x_off; // X Offset to adjust characters in console buffer
	u32 y_off; // Y Offset to adjust characters in console buffer
	u32 cur_x_default; // Current console X default position in character
	u32 cur_y_default; // Current console Y default position in character
	u32 cur_x; // Current console X position in character
	u32 cur_y; // Current console Y position in character
	char printf_buffer[PRINTF_BUFFER_SIZE]; /* Printf Buffer */
} t_con_handle;

/* Internal functions */
inline static void con_putc(t_con_handle* con_handle, const u32 x_pos, const u32 y_pos, const u8 chr)
{
	u32 idx;
	u32 shift;
	u32 buff_idx;
	u32 width = con_handle->con_dimensions.width;
	u32 bg_color = con_handle->bg_color;
	u32 fg_color = con_handle->fg_color;
	u32* buffer = con_handle->framebuffer;
	u32 font_idx = chr * con_handle->font_height;
	u32 x = x_pos;
	u32 y = y_pos;
	u8 c;

	c = font[font_idx];
	
	for(idx = 0; idx < con_handle->font_height; idx++)
	{
		buff_idx = y * width + x;
		for(shift = 0; shift < 8; shift++ )
		{
			if( c & (1 << shift) )
				buffer[buff_idx + (7 - shift)] = fg_color;
			else
				buffer[buff_idx + (7 - shift)] = bg_color;
		}
		y++;
		
		c = font[font_idx + idx];
	}
}

inline static s32 con_write(t_con_handle* handle, const u32 size)
{
	u32 idx;
	u8 buf_char;
	
	u32 con_height_max = (( handle->con_dimensions.height - (handle->con_dimensions.height % handle->font_height) ) - handle->y_off);
	u32 con_width_max = (handle->con_dimensions.width - handle->x_off);
	u32* framebuffer = handle->framebuffer;
	u8* scroll_buffer_addr = ( (u8*)framebuffer + (handle->font_height * (handle->con_dimensions.width * 4)) );
	u32 scroll_buffer_size = handle->con_dimensions.width * ( handle->con_dimensions.height - (handle->con_dimensions.height % handle->font_height) ) * 4;
	u32 cur_x = handle->cur_x;
	u32 cur_y = handle->cur_y;
	const char* buf = handle->printf_buffer;

	for(idx = 0; buf[idx] != 0 || idx < size; idx++)
	{
		buf_char = buf[idx];
		if( buf_char == '\n')
		{
			cur_x =  handle->cur_x_default;
			cur_y += handle->font_height;
		}else if( buf_char == '\r' )
		{
			cur_x =  handle->cur_x_default;
		}else
		{
			con_putc(handle, cur_x, cur_y, buf_char);
			cur_x += handle->font_width;
		}
		
		if( cur_x >= con_width_max)
		{
			cur_x = handle->cur_x_default;
			cur_y += handle->font_height;
		}

		if( cur_y >= con_height_max)
		{
			// Draw text on the last line
			cur_x = handle->cur_x_default;
			cur_y -= handle->font_height;
				
			// Scroll the front buffer
			memcpy(framebuffer, scroll_buffer_addr, scroll_buffer_size);
		}
	}
	
	/* Update handle */
	handle->cur_x = cur_x;
	handle->cur_y = cur_y;
	
	return idx;
}

s32 con_printf(void* con_handle, const char* format, ... )
{
	t_con_handle* handle = con_handle;
	
	// Clear the printf buffer
	memset(handle->printf_buffer, 0, PRINTF_BUFFER_SIZE);
	
	va_list args;
	va_start(args, format);
	
	s32 ret = vsnprintf(handle->printf_buffer, PRINTF_BUFFER_SIZE-1, format, args);
	handle->printf_buffer[PRINTF_BUFFER_SIZE-1] = 0;
	con_write(handle, ret);
	
	va_end(args);
	
	return ret;
}

// Note: X and Y are in CHARACTERS
s32 con_printf_xy(void* con_handle, const u32 x, const u32 y, const char* format, ... )
{
	t_con_handle* handle = con_handle;
	
	/* Save cursor positions */
	u32 old_cur_x = handle->cur_x; // Current console X position
	u32 old_cur_y = handle->cur_y; // Current console Y position	
	/* Modify cursor position */
	con_cursor_xy_set(con_handle,x, y);
	
	// Clear the printf buffer
	memset(handle->printf_buffer, 0, PRINTF_BUFFER_SIZE);
	
	va_list args;
	va_start(args, format);
	
	s32 ret = vsnprintf(handle->printf_buffer, PRINTF_BUFFER_SIZE-1, format, args);
	handle->printf_buffer[PRINTF_BUFFER_SIZE-1] = 0;
	con_write(handle, ret);
	
	va_end(args);
	
	/* Restore cursor positions */
	handle->cur_x = old_cur_x; // Current console X position
	handle->cur_y = old_cur_y; // Current console Y position	
	
	return ret;
}

// Note: X and Y are in CHARACTERS
void con_cursor_xy_set(void* con_handle, const u32 x, const u32 y)
{
	t_con_handle* handle = con_handle;
	handle->cur_x = (x * handle->font_width) + handle->x_off;
	handle->cur_y = (y * handle->font_height) + handle->y_off;
}

// Note: X and Y are in CHARACTERS
void con_cursor_xy_get(void* con_handle, u32* x, u32* y)
{
	t_con_handle* handle = con_handle;
	if(x)
		*x = handle->cur_x;
	
	if(y)
		*y = handle->cur_y;
}

// Note: width and height are in CHARACTERS
void con_dimensions_get(void* con_handle, u32* width, u32* height)
{
	t_con_handle* handle = con_handle;
	if( width )
		*width = handle->con_dimensions.width / handle->font_width;
	
	if( height )
		*height = handle->con_dimensions.height / handle->font_height;
}

// Note: width and height are in pixels
void con_fontsize_get(void* con_handle, u32* width, u32* height)
{
	t_con_handle* handle = con_handle;
	if( width )
		*width = handle->font_width;
	
	if( height )
		*height = handle->font_height;
}

/* Get console FrameBuffer address */
u32* con_framebuffer_get(void* con_handle)
{
	t_con_handle* handle = con_handle;
	return handle->framebuffer;
}

/* Set console foreground color for future write */
void con_fgcolor_set(void* con_handle, u32 color)
{
	t_con_handle* handle = con_handle;
	handle->fg_color = color;
}

/* Set console background color for future write */
void con_bgcolor_set(void* con_handle, u32 color)
{
	t_con_handle* handle = con_handle;
	handle->bg_color = color;
}

/* Clear console framebuffer */
void con_clear(void* con_handle)
{
	t_con_handle* handle = con_handle;
	// Reset cursor location
	handle->cur_x = handle->cur_x_default;
	handle->cur_y = handle->cur_y_default;
	
	memset(handle->framebuffer, 0, handle->con_dimensions.width * handle->con_dimensions.height * 4 );
}

void con_draw(void* con_handle, u32* screen_buffer, const u32 screen_width, const u32 screen_height)
{
	u32 screen_size;
	u32 console_size;
	
	t_con_handle* handle = con_handle;
	screen_size = screen_width*screen_height*4;
	console_size = handle->con_dimensions.width * handle->con_dimensions.height * 4;
	
	// Blit the internal buffer only if console size 
	if(screen_size < console_size)
	{
		/* If console size is > screen just blit the maximum size to don't overflow the screen buffer */
		console_size = screen_size;
	}
	memcpy(screen_buffer, handle->framebuffer, console_size);
}

/* Return NULL if error or con_handle pointer */
void* con_init(const t_con_dimensions* con_dimensions)
{
	t_con_handle* handle = NULL;
	
	// Allocate Console Handle
	handle = malloc(sizeof(t_con_handle));
	if( !handle )
		return NULL;
	
	// Allocate a buffer used for internal framebuffer operations
	handle->framebuffer = malloc( con_dimensions->width * con_dimensions->height * 4);
	if(!handle->framebuffer)
	{
		free(handle);
		return NULL;
	}

	/* Init Con Handle */
	handle->con_dimensions = *con_dimensions;
	handle->font_width = FONT_WIDTH;
	handle->font_height = FONT_HEIGHT;
	handle->fg_color = DEFAULT_FG_COLOR;
	handle->bg_color = DEFAULT_BG_COLOR;
	handle->x_off = (6*handle->font_width);
	handle->y_off = (2*handle->font_height);
	handle->cur_x_default = handle->x_off;
	handle->cur_y_default = handle->y_off;
	handle->cur_x = handle->cur_x_default;
	handle->cur_y = handle->cur_y_default;
	
	return handle;
}

void con_exit(void* con_handle)
{
	t_con_handle* handle = con_handle;
	if(handle)
	{
		if(handle->framebuffer)
			free(handle->framebuffer);
			
		free(handle);
	}
}
