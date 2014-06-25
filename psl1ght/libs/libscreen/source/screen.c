/* Screen utils */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include <rsx/screen.h>

gcmContextData *context; // Context to keep track of the RSX buffer.
volatile s32 currentBuffer = 0;
/* Flip/Flop Frame Buffer */
t_buffer *buffers[2]; // The buffer we will be drawing into.

static VideoResolution video_res; // Screen Resolution

// Block the PPU thread until the previous flip operation has finished.
void waitFlip(void)
{ 
	while(gcmGetFlipStatus() != 0) 
		usleep(200);
	gcmResetFlipStatus();
}

void flip(s32 buffer)
{
	assert(gcmSetFlip(context, buffer) == 0);
	realityFlushBuffer(context);
	gcmSetWaitFlip(context); // Prevent the RSX from continuing until the flip has finished.
}

static void makeBuffer(int id, int size) {
	t_buffer *buf = malloc(sizeof(t_buffer));
	buf->ptr = rsxMemAlign(16, size);
	assert(buf->ptr != NULL);

	assert(realityAddressToOffset(buf->ptr, &buf->offset) == 0);
	// Register the display buffer with the RSX
	assert(gcmSetDisplayBuffer(id, buf->offset, video_res.width * 4, video_res.width, video_res.height) == 0);
	
	buf->width = video_res.width;
	buf->height = video_res.height;
	buffers[id] = buf;
}

// Initialize screen.
/* Fill Video Resolution */
void screen_init(VideoResolution* res)
{
	// Allocate a 1Mb buffer, aligned to a 1Mb boundary to be our shared IO memory with the RSX.
	void *host_addr = memalign(1024*1024, 1024*1024);
	assert(host_addr != NULL);

	// Initialise Reality, which sets up the command buffer and shared IO memory
	context = realityInit(0x10000, 1024*1024, host_addr); 
	assert(context != NULL);

	VideoState state;
	assert(videoGetState(0, 0, &state) == 0); // Get the state of the display
	assert(state.state == 0); // Make sure display is enabled

	// Get the current resolution
	assert(videoGetResolution(state.displayMode.resolution, &video_res) == 0);
	*res = video_res;
	
	// Configure the buffer format to xRGB
	VideoConfiguration vconfig;
	memset(&vconfig, 0, sizeof(VideoConfiguration));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = video_res.width * 4;

	assert(videoConfigure(0, &vconfig, NULL, 0) == 0);
	assert(videoGetState(0, 0, &state) == 0); 

	s32 buffer_size = 4 * video_res.width * video_res.height; // each pixel is 4 bytes
	
	gcmSetFlipMode(GCM_FLIP_VSYNC); // Wait for VSYNC to flip

	// Allocate two buffers for the RSX to draw to the screen (double buffering)
	makeBuffer(0, buffer_size);
	makeBuffer(1, buffer_size);

	gcmResetFlipStatus();
	flip(1);
}
