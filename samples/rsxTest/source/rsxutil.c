#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include <rsx/commands.h>

#include "rsxutil.h"

gcmContextData *context; // Context to keep track of the RSX buffer.

VideoResolution res; // Screen Resolution

u32 *buffer[2]; // The buffer we will be drawing into
u32 offset[2]; // The offset of the buffers in RSX memory
u32 *depth_buffer; // Depth buffer. We aren't using it but the ps3 crashes if we don't have it
u32 depth_offset;

int pitch;
int depth_pitch;

// Initilize and rsx
void init_screen() {
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
	
	pitch = 4 * res.width; // each pixel is 4 bytes
	depth_pitch = 2 * res.width; // And each value in the depth buffer is a 16 bit float

	// Configure the buffer format to xRGB
	VideoConfiguration vconfig;
	memset(&vconfig, 0, sizeof(VideoConfiguration));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = pitch;

	assert(videoConfigure(0, &vconfig, NULL, 0) == 0);
	assert(videoGetState(0, 0, &state) == 0); 

	s32 buffer_size = pitch * res.height; 
	s32 depth_buffer_size = depth_pitch * res.height;
	printf("buffers will be 0x%x bytes\n", buffer_size);
	
	gcmSetFlipMode(GCM_FLIP_VSYNC); // Wait for VSYNC to flip

	// Allocate two buffers for the RSX to draw to the screen (double buffering)
	buffer[0] = rsxMemAlign(16, buffer_size);
	buffer[1] = rsxMemAlign(16, buffer_size);
	assert(buffer[0] != NULL && buffer[1] != NULL);

	depth_buffer = rsxMemAlign(16, depth_buffer_size * 2);

	assert(realityAddressToOffset(buffer[0], &offset[0]) == 0);
	assert(realityAddressToOffset(buffer[1], &offset[1]) == 0);
	// Setup the display buffers
	assert(gcmSetDisplayBuffer(0, offset[0], pitch, res.width, res.height) == 0);
	assert(gcmSetDisplayBuffer(1, offset[1], pitch, res.width, res.height) == 0);

	assert(realityAddressToOffset(depth_buffer, &depth_offset) == 0);

	gcmResetFlipStatus();
	flip(1);
}

void waitFlip() { // Block the PPU thread untill the previous flip operation has finished.
	while(gcmGetFlipStatus() != 0) 
		usleep(200);
	gcmResetFlipStatus();
}

void flip(s32 buffer) {
	assert(gcmSetFlip(context, buffer) == 0);
	realityFlushBuffer(context);
	gcmSetWaitFlip(context); // Prevent the RSX from continuing until the flip has finished.
}

void setupRenderTarget(u32 currentBuffer) {
	// Set the color0 target to point at the offset of our current surface
	realitySetRenderSurface(context, REALITY_SURFACE_COLOR0, REALITY_RSX_MEMORY, 
					offset[currentBuffer], pitch);

	// Setup depth buffer
	realitySetRenderSurface(context, REALITY_SURFACE_ZETA, REALITY_RSX_MEMORY, 
					depth_offset, depth_pitch);

	// Choose color0 as the render target and tell the rsx about the surface format.
	realitySelectRenderTarget(context, REALITY_TARGET_0, 
		REALITY_TARGET_FORMAT_COLOR_X8R8G8B8 | 
		REALITY_TARGET_FORMAT_ZETA_Z16 | 
		REALITY_TARGET_FORMAT_TYPE_LINEAR,
		res.width, res.height, 0, 0);
}
