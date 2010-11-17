/* libjpg.c

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
#include <psl1ght/types.h>

#include <jpgdec/loadjpg.h>


static void *jpg_malloc(u32 size, void * a) {

	return malloc(size);
}


static int jpg_free(void *ptr, void * a) {
	
	free(ptr);
	
	return 0;
} 

int LoadJPG(JpgDatas *jpg, char *filename)
{
	int ret=-1;
	
	int mHandle;
	int sHandle;

	JpgDecThreadInParam InThdParam;
	JpgDecThreadOutParam OutThdParam;

	JpgDecInParam inParam;
	JpgDecOutParam outParam;
	
	JpgDecSrc src; 
	uint32_t space_allocated;

	JpgDecInfo DecInfo;
	
	uint64_t bytes_per_line;
	JpgDecDataInfo DecDataInfo;

	InThdParam.enable   = 0;
	InThdParam.ppu_prio = 512;
	InThdParam.spu_prio = 200;
	InThdParam.addr_malloc_func  = (u32)(u64) OPD32(jpg_malloc);
	InThdParam.addr_malloc_arg   = 0; // no args
	InThdParam.addr_free_func    = (u32)(u64) OPD32(jpg_free);
	InThdParam.addr_free_arg    =  0; // no args


	ret= JpgDecCreate(&mHandle, &InThdParam, &OutThdParam);

	jpg->bmp_out= NULL;

	if(ret == 0) {
		
		memset(&src, 0, sizeof(JpgDecSrc));
			
		if(filename) {
			src.stream_select = JPGDEC_FILE;
			src.addr_file_name  = (u32)(u64) filename;
		} else {
			src.stream_select = JPGDEC_BUFFER;
			src.addr_stream_ptr  = (u32)(u64) jpg->jpg_in;
			src.stream_size    = jpg->jpg_size;
		}

		src.enable  = JPGDEC_DISABLE;
			
		ret= JpgDecOpen(mHandle, &sHandle, &src, &space_allocated);
			
		if(ret == 0) {
			
			ret = JpgDecReadHeader(mHandle, sHandle, &DecInfo);
			
			if(ret==0 && DecInfo.color_space==0) ret=-1; // unsupported color

			if(ret == 0) {	
		
				inParam.addr_cmd_ptr = 0;
				inParam.downscale	 = 1;
				inParam.quality		 = JPGDEC_LOW_QUALITY; // fast
				inParam.mode         = JPGDEC_TOP_TO_BOTTOM;
				inParam.color_space  = JPGDEC_ARGB;
				inParam.color_alpha  = 0xFF;

				ret = JpgDecSetParameter(mHandle, sHandle, &inParam, &outParam);
				}
				
			if(ret == 0) {
					jpg->wpitch= outParam.width* 4;
					bytes_per_line = (uint64_t)  jpg->wpitch;

					jpg->bmp_out= malloc(jpg->wpitch *  outParam.height);

					if(!jpg->bmp_out) {

						ret=-1; // out of memory

					} else {

						memset(jpg->bmp_out, 0, jpg->wpitch *  outParam.height);
						
						ret = JpgDecDecodeData(mHandle, sHandle, jpg->bmp_out, &bytes_per_line, &DecDataInfo);

						if((ret == 0) && (DecDataInfo.status == 0)){

							jpg->width  = outParam.width;
							jpg->height = outParam.height;
							
							ret=0; // ok :)

							}
					}
				}
				
			JpgDecClose(mHandle, sHandle);
			}

			if(ret && jpg->bmp_out) {
				
				free(jpg->bmp_out); jpg->bmp_out= NULL;	

			}

			JpgDecDestroy(mHandle);
			
		}

return ret;
}

