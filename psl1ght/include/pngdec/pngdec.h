#pragma once

#include <psl1ght/types.h>

EXTERN_BEGIN

#define PNGDEC_DISABLE	0
#define PNGDEC_ENABLE   1

#define PNGDEC_TOP_TO_BOTTOM 0
#define PNGDEC_BOTTOM_TO_TOP 1        


typedef enum {
    PNGDEC_FILE      = 0,
    PNGDEC_BUFFER    = 1
} PngStreamSel;

typedef enum {
    PNGDEC_GRAYSCALE       = 1,
    PNGDEC_RGB             = 2,
    PNGDEC_PALETTE         = 4,
    PNGDEC_GRAYSCALE_ALPHA = 9,
    PNGDEC_RGBA            = 10,
    PNGDEC_ARGB            = 20
} PngColorSpace;

typedef void * (*PngMalloc)(uint32_t size, void *arg);

typedef int (*PngFree)(void *ptr, void *arg);

typedef struct {
    int enable;
    uint32_t ppu_prio;
    uint32_t spu_prio;
	uint32_t addr_malloc_func;
    uint32_t addr_malloc_arg;
    uint32_t addr_free_func;
    uint32_t addr_free_arg;
} PngDecThreadInParam;

typedef struct {
    uint32_t version;
} PngDecThreadOutParam;

typedef struct {
    PngStreamSel stream_select;
    uint32_t addr_file_name;
    int64_t file_offset;
    uint32_t file_size;
    uint32_t addr_stream_ptr;
    uint32_t stream_size;
    int enable;
} PngDecSrc;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t num_components;
    PngColorSpace color_space;
    uint32_t bit_depth;
	int interlaced;
	uint32_t chunk_info;
} PngDecInfo;

typedef struct {
	uint32_t chunk_info;
	uint32_t num_text; 
	uint32_t num_unk_chunck;
    int status;
} PngDecDataInfo;

typedef struct {
    uint32_t addr_cmd_ptr;
    int mode;
    PngColorSpace color_space;
    uint32_t bit_depth;
    int pack_flag;
    int alpha_select;
    uint32_t color_alpha;
} PngDecInParam;

typedef struct {
    uint64_t width_byte;
    uint32_t width;
    uint32_t height;
    uint32_t num_components;
    uint32_t bit_depth;
    int mode;
    PngColorSpace color_space;
    uint32_t use_memory_space;
} PngDecOutParam;

int PngDecCreate(int * main_handle, PngDecThreadInParam *in, PngDecThreadOutParam *out);

int PngDecOpen(int main_handle, int * sub_handle, const PngDecSrc *src, uint32_t *space_allocated);

int PngDecReadHeader(int main_handle, int subHandle, PngDecInfo *info);

int PngDecSetParameter(int main_handle, int subHandle, const PngDecInParam *inParam, PngDecOutParam  *outParam);

int PngDecDecodeData(int main_handle, int subHandle, uint8_t *data, const uint64_t *bytes_per_line, PngDecDataInfo *dataInfo);

int PngDecClose(int main_handle, int subHandle);

int PngDecDestroy(int main_handle);


EXTERN_END
