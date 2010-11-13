/* Now double buffered with animation with one PNG dancing.
 */ 

#include <psl1ght/lv2.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>

#include <io/pad.h>

#include <sysmodule/sysmodule.h>
#include <pngdec/pngdec.h>

#include <psl1ght/lv2.h>

#include "libpng.h"
#include "psl1ght_png.bin.h" // png in memory

gcmContextData *context; // Context to keep track of the RSX buffer.

VideoResolution res; // Screen Resolution

int currentBuffer = 0;
s32 *buffer[2]; // The buffer we will be drawing into.

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

// Initilize everything. You can probally skip over this function.
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

void drawFrame(int *buffer, long frame) {
	s32 i, j;
	for(i = 0; i < res.height; i++) {
		s32 color = (i / (res.height * 1.0) * 256);
		// This should make a nice black to green graident
		color = (color << 8) | ((frame % 255) << 16);
		for(j = 0; j < res.width; j++)
			buffer[i* res.width + j] = color;
	}

}

u32 module_flag;

void unload_modules(){

	if(module_flag & 2)
		SysUnloadModule(SYSMODULE_PNGDEC);

	if(module_flag & 1)
		SysUnloadModule(SYSMODULE_FS);
}

s32 main(s32 argc, const char* argv[])
{
	PadInfo padinfo;
	PadData paddata;
	int i;
	
	atexit(unload_modules);

	if(SysLoadModule(SYSMODULE_FS)!=0) return 0; else module_flag |=1;

	if(SysLoadModule(SYSMODULE_PNGDEC)!=0) return 0; else module_flag |=2;

	init_screen();
	ioPadInit(7);

	PngDatas png1;

#ifndef USE_PNG_FROM_FILE
	
	png1.png_in= (void *) psl1ght_png_bin;
	png1.png_size= sizeof(psl1ght_png_bin);

	LoadPNG(&png1, NULL);

#else

	LoadPNG(&png1, "/dev_usb/PS3_GAME/ICON0.PNG");

#endif

	long frame = 0; // To keep track of how many frames we have rendered.
	
	// Ok, everything is setup. Now for the main loop.
	while(1){
		// Check the pads.
		ioPadGetInfo(&padinfo);
		for(i=0; i<MAX_PADS; i++){
			if(padinfo.status[i]){
				ioPadGetData(i, &paddata);
				
				if(paddata.BTN_CROSS){
					return 0;
				}
			}
			
		}

		waitFlip(); // Wait for the last flip to finish, so we can draw to the old buffer
		
		drawFrame(buffer[currentBuffer], frame++); // Draw into the unused buffer

		if(png1.bmp_out) {
		
			static int x=0,y=0,dx=2,dy=2;

			u32 *scr=  (u32 *) buffer[currentBuffer];
			u32 *png= (u32 *) png1.bmp_out;
			int n, m;
		
			// update x, y coordinates

			x+=dx; y+=dy;

			if(x < 0) {x=0; dx=1;}
			if(x > (res.width-png1.width)) {x=(res.width-png1.width); dx=-2;}

			if(y < 0) {y=0; dy=1;}
			if(y > (res.height-png1.height)) {y=(res.height-png1.height); dy=-2;}
				
			
			// update screen buffer from coordinates

			scr+=y*res.width+x;

			// draw PNG

			for(n=0;n<png1.height;n++) {

				if((y+n)>=res.height) break;
			
				for(m=0;m<png1.width;m++) {
				
					if((x+m)>=res.width) break;
					scr[m]=png[m];

				}
			
			png+=png1.wpitch>>2;
			scr+=res.width;

			}
		
		}

		flip(currentBuffer); // Flip buffer onto screen
		currentBuffer = !currentBuffer;
	}
	
	return 0;
}

