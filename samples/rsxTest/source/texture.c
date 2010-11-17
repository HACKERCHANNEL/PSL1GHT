#include <assert.h>
#include <string.h>

#include <pngdec/loadpng.h>

#include <rsx/reality.h>
#include <rsx/commands.h>
#include <rsx/nv40.h>

#include "texture.h"

#include "acid.c"

extern gcmContextData *context;

void load_acid_texture(uint8_t *fbmem, uint32_t offset)
{
  uint32_t i;
  const unsigned char *src = acid.pixel_data;
  for( i = 0; i < acid.width * acid.height * 4; i += 4 )
  {
    fbmem[i + offset + 1] = *src++;
    fbmem[i + offset + 2] = *src++;
    fbmem[i + offset + 3] = *src++;
    fbmem[i + offset + 0] = *src++;
  }
}

void load_tex(uint32_t unit, uint32_t offset, uint32_t width, uint32_t height, uint32_t stride, uint32_t fmt, int smooth )
{
  realityTexture tex;
  tex.swizzle =
    NV30_3D_TEX_SWIZZLE_S0_X_S1 | NV30_3D_TEX_SWIZZLE_S0_Y_S1 |
    NV30_3D_TEX_SWIZZLE_S0_Z_S1 | NV30_3D_TEX_SWIZZLE_S0_W_S1 |
    NV30_3D_TEX_SWIZZLE_S1_X_X | NV30_3D_TEX_SWIZZLE_S1_Y_Y |
    NV30_3D_TEX_SWIZZLE_S1_Z_Z | NV30_3D_TEX_SWIZZLE_S1_W_W ;

  tex.offset = offset;

  tex.format = fmt |
    NV40_3D_TEX_FORMAT_LINEAR  | 
    NV30_3D_TEX_FORMAT_DIMS_2D |
    NV30_3D_TEX_FORMAT_DMA0 |
    NV30_3D_TEX_FORMAT_NO_BORDER | (0x8000) |
    (1 << NV40_3D_TEX_FORMAT_MIPMAP_COUNT__SHIFT);

  tex.wrap =  NV30_3D_TEX_WRAP_S_REPEAT |
    NV30_3D_TEX_WRAP_T_REPEAT |
    NV30_3D_TEX_WRAP_R_REPEAT;

  tex.enable = NV40_3D_TEX_ENABLE_ENABLE;

  if(smooth)
    tex.filter = NV30_3D_TEX_FILTER_MIN_LINEAR |
	       NV30_3D_TEX_FILTER_MAG_LINEAR | 0x3fd6;
  else
    tex.filter = NV30_3D_TEX_FILTER_MIN_NEAREST |
	       NV30_3D_TEX_FILTER_MAG_NEAREST | 0x3fd6;

  tex.width = width;
  tex.height = height;
  tex.stride = stride;

  realitySetTexture(context, unit, &tex);
}

/* This code doesn't work for some reason. I'm way to hot to care...
realityTexture *loadTexture(const uint8_t *pngData) {
	PngDatas png;
	png.png_in= (void *) pngData;
	png.png_size= sizeof(pngData);

	LoadPNG(&png, NULL);

	realityTexture *tex = malloc(sizeof(realityTexture));

	uint32_t size = png.width * png.height * 4;
	uint32_t *textureAddress = rsxMemAlign(128, size*4);
	assert(textureAddress != NULL);
	assert(realityAddressToOffset(textureAddress, &tex->offset) == 0);
	
	// copy texture to video memory
	memcpy(textureAddress, png.bmp_out, size);

	// Setup tex struct
	tex->format = NV40_3D_TEX_FORMAT_FORMAT_A8R8G8B8 |
		      NV40_3D_TEX_FORMAT_LINEAR |
		      NV30_3D_TEX_FORMAT_DMA0 |
		      NV30_3D_TEX_FORMAT_NO_BORDER | 0x8000 |
		     (1 << NV40_3D_TEX_FORMAT_MIPMAP_COUNT__SHIFT);
	tex->wrap = NV30_3D_TEX_WRAP_S_REPEAT |
		    NV30_3D_TEX_WRAP_T_REPEAT |
		    NV30_3D_TEX_WRAP_R_REPEAT;
	tex->enable = NV40_3D_TEX_ENABLE_ENABLE;
	tex->filter = NV30_3D_TEX_FILTER_MIN_LINEAR |
	              NV30_3D_TEX_FILTER_MAG_LINEAR | 0x3fd6;
	tex->swizzle =
	    NV30_3D_TEX_SWIZZLE_S0_X_S1 | NV30_3D_TEX_SWIZZLE_S0_Y_S1 |
	    NV30_3D_TEX_SWIZZLE_S0_Z_S1 | NV30_3D_TEX_SWIZZLE_S0_W_S1 |
	    NV30_3D_TEX_SWIZZLE_S1_X_X | NV30_3D_TEX_SWIZZLE_S1_Y_Y |
	    NV30_3D_TEX_SWIZZLE_S1_Z_Z | NV30_3D_TEX_SWIZZLE_S1_W_W ;
	tex->width = png.width;
	tex->height = png.height;
	tex->stride = png.width * 4;

	free(png.bmp_out);

	printf("Texture offset: 0x%08x", tex->offset);

	return tex;
}*/
