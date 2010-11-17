
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#include <rsx/commands.h>
#include <rsx/nv40.h>
#include <rsx/reality.h>

#include <io/pad.h>

#include <sysmodule/sysmodule.h>

//#include "ball.bin.h"
#include "texture.h"
#include "rsxutil.h"
#include "nv_shaders.h"

int currentBuffer = 0;
//realityTexture *ball; // Texture.

u32 *tx_mem;
u32 tx_offset;

void drawFrame(int buffer, long frame) {

	realityViewportTranslate(context, 0.0, 0.0, 0.0, 0.0);
	realityViewportScale(context, 1.0, 1.0, 1.0, 0.0); 

	realityZControl(context, 0, 1, 1); // disable viewport culling

	// Enable alpha blending.
	realityBlendFunc(context,
		NV30_3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA |
		NV30_3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
		NV30_3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA |
		NV30_3D_BLEND_FUNC_DST_ALPHA_ZERO);
	realityBlendEquation(context, NV40_3D_BLEND_EQUATION_RGB_FUNC_ADD |
		NV40_3D_BLEND_EQUATION_ALPHA_FUNC_ADD);
	realityBlendEnable(context, 1);

	realityViewport(context, res.width, res.height);

	// Just because we can only set the clear color, dosen't mean it has to be boring
	double cycle = (frame % 200)/100.0;
	uint8_t color = (sin(cycle*M_PI) + 1.0) * 127 ;

	setupRenderTarget(buffer);

	// set the clear color
	realitySetClearColor(context, color | color << 8 | color << 16); 
	// and the depth clear value
	realitySetClearDepthValue(context, 0xffff);
	// Clear the buffers
	realityClearBuffers(context, REALITY_CLEAR_BUFFERS_COLOR_R |
				     REALITY_CLEAR_BUFFERS_COLOR_G |
				     REALITY_CLEAR_BUFFERS_COLOR_B |
				     NV30_3D_CLEAR_BUFFERS_COLOR_A |
				     NV30_3D_CLEAR_BUFFERS_STENCIL |
				     REALITY_CLEAR_BUFFERS_DEPTH);

	// Load shaders, because the rsx won't do anything without them.
	realityLoadVertexProgram(context, &nv40_vp);
	realityLoadFragmentProgram(context, &nv30_fp); 

	// Load texture
	load_tex(0, tx_offset, 128, 128, 128*4,  NV40_3D_TEX_FORMAT_FORMAT_A8R8G8B8, 1);
	
	// Generate quad
	realityVertexBegin(context, REALITY_QUADS);
	{
		realityTexCoord2f(context, 0.0, 0.0);
		realityVertex4f(context, 600.0, 300.0, 0.0, 1.0); 

		realityTexCoord2f(context, 1.0, 0.0);
		realityVertex4f(context, 1112.0, 300.0, 0.0, 1.0); 

		realityTexCoord2f(context, 1.0, 1.0);
		realityVertex4f(context, 1112.0, 812.0, 0.0, 1.0); 

		realityTexCoord2f(context, 0.0, 1.0);
		realityVertex4f(context, 600.0, 812.0, 0.0, 1.0); 
	}
	realityVertexEnd(context);
}

s32 main(s32 argc, const char* argv[])
{
	PadInfo padinfo;
	PadData paddata;
	int i;
	
	init_screen();
	ioPadInit(7);

	// Load texture
	tx_mem = rsxMemAlign(16, 2*1024*1024);
	assert(realityAddressToOffset(tx_mem, &tx_offset) == 0);

	load_acid_texture((uint8_t *)tx_mem, 0);

	// install fragment shader in rsx memory
	u32 *frag_mem = rsxMemAlign(256, 256);
	printf("frag_mem = 0x%08lx\n", (u64) frag_mem);
	realityInstallFragmentProgram(context, &nv30_fp, frag_mem);

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
		drawFrame(currentBuffer, frame++); // Draw into the unused buffer
		flip(currentBuffer); // Flip buffer onto screen
		currentBuffer = !currentBuffer;

	}
	
	return 0;
}

