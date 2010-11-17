/* libpng.c

Copyright (c) 2010 Hermes <www.elotrolado.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are 
permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this list of 
  conditions and the following disclaimer. 
- Redistributions in binary form must reproduce the above copyright notice, this list 
  of conditions and the following disclaimer in the documentation and/or other 
  materials provided with the distribution. 
- The names of the contributors may not be used to endorse or promote products derived 
  from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <malloc.h>
#include <string.h>

#include <pngdec/loadpng.h>


static void *png_malloc(u32 size, void * a) {

	return malloc(size);
}


static int png_free(void *ptr, void * a) {
	
	free(ptr);
	
	return 0;
} 

int LoadPNG(PngDatas *png, char *filename)
{
	int ret=-1;
	
	int mHandle;
	int sHandle;

	PngDecThreadInParam InThdParam;
	PngDecThreadOutParam OutThdParam;

	PngDecInParam inParam;
	PngDecOutParam outParam;
	
	PngDecSrc src; 
	uint32_t space_allocated;

	PngDecInfo DecInfo;
	
	uint64_t bytes_per_line;
	PngDecDataInfo DecDataInfo;

	InThdParam.enable   = 0;
	InThdParam.ppu_prio = 512;
	InThdParam.spu_prio = 200;
	InThdParam.addr_malloc_func  = (u32)(u64) OPD32(png_malloc);
	InThdParam.addr_malloc_arg   = 0; // no args
	InThdParam.addr_free_func    = (u32)(u64) OPD32(png_free);
	InThdParam.addr_free_arg    =  0; // no args


	ret= PngDecCreate(&mHandle, &InThdParam, &OutThdParam);

	png->bmp_out= NULL;

	if(ret == 0) {
		
		memset(&src, 0, sizeof(PngDecSrc));
			
		if(filename) {
			src.stream_select = PNGDEC_FILE;
			src.addr_file_name  = (u32)(u64) filename;
		} else {
			src.stream_select = PNGDEC_BUFFER;
			src.addr_stream_ptr  = (u32)(u64) png->png_in;
			src.stream_size    = png->png_size;
		}

		src.enable  = PNGDEC_DISABLE;
			
		ret= PngDecOpen(mHandle, &sHandle, &src, &space_allocated);
			
		if(ret == 0) {
			
			ret = PngDecReadHeader(mHandle, sHandle, &DecInfo);
				
			if(ret == 0) {	
				
				inParam.addr_cmd_ptr      = 0;
				inParam.mode         = PNGDEC_TOP_TO_BOTTOM;
				inParam.color_space  = PNGDEC_ARGB;
				inParam.bit_depth    = 8;
				inParam.pack_flag    = 1;
				
				if((DecInfo.color_space == PNGDEC_GRAYSCALE_ALPHA) || (DecInfo.color_space == PNGDEC_RGBA) || (DecInfo.chunk_info & 0x10))
					inParam.alpha_select = 0;
				else
					inParam.alpha_select = 1;
				
				inParam.color_alpha  = 0xff;

				ret = PngDecSetParameter(mHandle, sHandle, &inParam, &outParam);
				}
				
			if(ret == 0) {
					png->wpitch= outParam.width* 4;
					bytes_per_line = (uint64_t)  png->wpitch;

					png->bmp_out= malloc(png->wpitch *  outParam.height);

					if(!png->bmp_out) {

						ret=-1; // out of memory

					} else {

						memset(png->bmp_out, 0, png->wpitch *  outParam.height);
						
						ret = PngDecDecodeData(mHandle, sHandle, png->bmp_out, &bytes_per_line, &DecDataInfo);

						if((ret == 0) && (DecDataInfo.status == 0)){

							png->width  = outParam.width;
							png->height = outParam.height;
							
							ret=0; // ok :)

							}
					}
				}
				
			PngDecClose(mHandle, sHandle);
			}

			if(ret && png->bmp_out) {
				
				free(png->bmp_out); png->bmp_out= NULL;	

			}

			PngDecDestroy(mHandle);
			
		}

return ret;
}

