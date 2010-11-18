/* Now double buffered with animation.
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

#include <console/Console.h>

gcmContextData *context; // Context to keep track of the RSX buffer.

VideoResolution res; // Screen Resolution


int currentBuffer = 0;
s32 *buffer[2]; // The buffer we will be drawing into.


CONSOLE_VIDEO g_ConsoleVideo[2];
CONSOLE_LOGIC g_ConsoleLogic;
CONSOLE_RENDER g_ConsoleRender;


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
		for(j = 0; j < res.width; j++)
			buffer[i* res.width + j] = i*j;
	}
}

s32 main(s32 argc, const char* argv[])
{
	PadInfo padinfo;
	PadData paddata;
	int i,j;
	int cols,rows;
	int slotwidth=16,slotheight=32;
	u32 X,Y,VPX,VPY;
	ULONG RenderingFlags;
	UCHAR TextmodeBackgorundColor;
	ULONG DropShadowColor;
	UCHAR DropShadowOrientation;

	init_screen();
	ioPadInit(7);

	cols = (res.width/slotwidth)/2;
	rows = (res.height/slotheight)/2;

	CONSOLE_VIDEO_Initialize(&(g_ConsoleVideo[0]),(PUCHAR)(buffer[0]),res.width,res.height,4,res.width*4,(res.width-(cols*slotwidth))/2,cols*slotwidth,(res.height-(rows*slotheight))/2,rows*slotheight);
	CONSOLE_VIDEO_Initialize(&(g_ConsoleVideo[1]),(PUCHAR)(buffer[1]),res.width,res.height,4,res.width*4,(res.width-(cols*slotwidth))/2,cols*slotwidth,(res.height-(rows*slotheight))/2,rows*slotheight);
	CONSOLE_LOGIC_Initialize(&g_ConsoleLogic,cols,rows);
	CONSOLE_RENDER_Initialize(&g_ConsoleRender,slotwidth,slotheight,6,g_BasicColors4BPP,CharMask8x16stdX2,g_8x16X2CursorMask,g_8x16X2CursorInsertMask,g_8x16X2MousePointerMask);

	for(j = 0;j<16;j++)
	{
		for(i = 0;i<16;i++)
		{
			CONSOLE_LOGIC_PutAttrCharPairXY(&g_ConsoleLogic, i, j, (i<<4)|j, (16*j)+i);
		}
	}

	long frame = 0; // To keep track of how many frames we have rendered.
	
	// Ok, everything is setup. Now for the main loop.
	while(1)
	{
		// Check the pads.
		ioPadGetInfo(&padinfo);
		for(i=0; i<MAX_PADS; i++)
		{
			if(padinfo.status[i])
			{
				ioPadGetData(i, &paddata);
				
				if(paddata.BTN_CROSS)
				{
					return 0;
				}

				//

				if(paddata.BTN_LEFT)
				{
					if(paddata.BTN_L1)
					{
						CONSOLE_LOGIC_GetMousePointerXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutMousePointerXY(&g_ConsoleLogic,X-1,Y);
					}
					else
					{
						CONSOLE_LOGIC_GetCursorXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutCursorXY(&g_ConsoleLogic,X-1,Y);
					}

					if(paddata.BTN_L2)
					{
						CONSOLE_VIDEO_GetViewPortX(&(g_ConsoleVideo[0]),&VPX);
						CONSOLE_VIDEO_GetViewPortY(&(g_ConsoleVideo[0]),&VPY);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[0]),VPX-1);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[0]),VPY);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[1]),VPX-1);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[1]),VPY);
					}
				}

				if(paddata.BTN_RIGHT)
				{
					if(paddata.BTN_L1)
					{
						CONSOLE_LOGIC_GetMousePointerXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutMousePointerXY(&g_ConsoleLogic,X+1,Y);
					}
					else
					{
						CONSOLE_LOGIC_GetCursorXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutCursorXY(&g_ConsoleLogic,X+1,Y);
					}

					if(paddata.BTN_L2)
					{
						CONSOLE_VIDEO_GetViewPortX(&(g_ConsoleVideo[0]),&VPX);
						CONSOLE_VIDEO_GetViewPortY(&(g_ConsoleVideo[0]),&VPY);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[0]),VPX+1);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[0]),VPY);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[1]),VPX+1);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[1]),VPY);
					}
				}

				if(paddata.BTN_UP)
				{
					if(paddata.BTN_L1)
					{
						CONSOLE_LOGIC_GetMousePointerXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutMousePointerXY(&g_ConsoleLogic,X,Y-1);
					}
					else
					{
						CONSOLE_LOGIC_GetCursorXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutCursorXY(&g_ConsoleLogic,X,Y-1);
					}

					if(paddata.BTN_L2)
					{
						CONSOLE_VIDEO_GetViewPortX(&(g_ConsoleVideo[0]),&VPX);
						CONSOLE_VIDEO_GetViewPortY(&(g_ConsoleVideo[0]),&VPY);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[0]),VPX);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[0]),VPY-1);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[1]),VPX);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[1]),VPY-1);
					}
				}

				if(paddata.BTN_DOWN)
				{
					if(paddata.BTN_L1)
					{
						CONSOLE_LOGIC_GetMousePointerXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutMousePointerXY(&g_ConsoleLogic,X,Y+1);
					}
					else
					{
						CONSOLE_LOGIC_GetCursorXY(&g_ConsoleLogic,&X,&Y);
						CONSOLE_LOGIC_PutCursorXY(&g_ConsoleLogic,X,Y+1);
					}

					if(paddata.BTN_L2)
					{
						CONSOLE_VIDEO_GetViewPortX(&(g_ConsoleVideo[0]),&VPX);
						CONSOLE_VIDEO_GetViewPortY(&(g_ConsoleVideo[0]),&VPY);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[0]),VPX);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[0]),VPY+1);

						CONSOLE_VIDEO_PutViewPortX(&(g_ConsoleVideo[1]),VPX);
						CONSOLE_VIDEO_PutViewPortY(&(g_ConsoleVideo[1]),VPY+1);
					}
				}

				if(paddata.BTN_R1)
				{
					CONSOLE_RENDER_GetRenderingOptions(&g_ConsoleRender,&RenderingFlags);
					RenderingFlags ^= CONSOLE_RENDER_RENDERING_FLAG_TRANSPARENT;
					CONSOLE_RENDER_SetRenderingOptions(&g_ConsoleRender,RenderingFlags);
				}


				if(paddata.BTN_R2)
				{
					CONSOLE_RENDER_GetRenderingOptions(&g_ConsoleRender,&RenderingFlags);
					RenderingFlags ^= CONSOLE_RENDER_RENDERING_FLAG_DROPSHADOW;
					CONSOLE_RENDER_SetRenderingOptions(&g_ConsoleRender,RenderingFlags);
				}

				if(paddata.BTN_R3)
				{
					CONSOLE_RENDER_GetRenderingOptions(&g_ConsoleRender,&RenderingFlags);
					RenderingFlags ^= CONSOLE_RENDER_RENDERING_FLAG_DROPSHADOWOVERANYBACKGROUND;
					CONSOLE_RENDER_SetRenderingOptions(&g_ConsoleRender,RenderingFlags);
				}

				//
			}
		}

		for(j = 0;j<rows;j++)
		{
			for(i = 0;i<cols;i++)
			{
				CONSOLE_LOGIC_PutAttrCharPairXY(&g_ConsoleLogic, i, j, frame&0x0F, frame);
			}
		}


		waitFlip(); // Wait for the last flip to finish, so we can draw to the old buffer

		drawFrame(buffer[currentBuffer], frame++); // Draw into the unused buffer
		CONSOLE_RENDER_Render(&g_ConsoleRender,&g_ConsoleLogic,&(g_ConsoleVideo[currentBuffer]));

		flip(currentBuffer); // Flip buffer onto screen
		currentBuffer = !currentBuffer;
	}

	CONSOLE_RENDER_Finalize(&g_ConsoleRender);
	CONSOLE_LOGIC_Finalize(&g_ConsoleLogic);
	CONSOLE_VIDEO_Finalize(&(g_ConsoleVideo[0]));
	CONSOLE_VIDEO_Finalize(&(g_ConsoleVideo[1]));

	return 0;
}



