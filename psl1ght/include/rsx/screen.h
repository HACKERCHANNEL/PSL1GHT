
#pragma once

#include <psl1ght/types.h>

EXTERN_BEGIN

typedef struct t_buffer
{
	u32 height;
	u32 width;
	u32 *ptr;
	// Internal stuff
	u32 offset;
} t_buffer;

extern gcmContextData *context;
extern volatile s32 currentBuffer;
extern t_buffer *buffers[2]; // The buffer we will be drawing into.

void waitFlip(void);
void flip(s32 buffer);
void screen_init(VideoResolution* res);

EXTERN_END

