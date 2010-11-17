#pragma once

#include <psl1ght/types.h>

EXTERN_BEGIN

#define JPGDEC_DISABLE	0
#define JPGDEC_ENABLE   1

#define JPGDEC_TOP_TO_BOTTOM 0
#define JPGDEC_BOTTOM_TO_TOP 1        

 #define JPGDEC_HIGH_QUALITY 0  
 #define JPGDEC_LOW_QUALITY	 5  

typedef enum {
    JPGDEC_FILE      = 0,
    JPGDEC_BUFFER    = 1
} JpgStreamSel;

typedef enum {
    JPGDEC_GRAYSCALE       = 1,
    JPGDEC_RGB             = 2,
    JPGDEC_YCBCR           = 3,
    JPGDEC_RGBA            = 10,
	JPGDEC_UPSTREAM        = 11,
    JPGDEC_ARGB            = 20
} JpgColorSpace;

typedef void * (*JpgMalloc)(uint32_t size, void *arg);

typedef int (*JpgFree)(void *ptr, void *arg);

typedef struct {
    int enable;
    uint32_t ppu_prio;
    uint32_t spu_prio;
	uint32_t addr_malloc_func;
    uint32_t addr_malloc_arg;
    uint32_t addr_free_func;
    uint32_t addr_free_arg;
} JpgDecThreadInParam;

typedef struct {
    uint32_t version;
} JpgDecThreadOutParam;

typedef struct {
    JpgStreamSel stream_select;
    uint32_t addr_file_name;
    int64_t file_offset;
    uint32_t file_size;
    uint32_t addr_stream_ptr;
    uint32_t stream_size;
    int enable;
} JpgDecSrc;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t num_components;
    JpgColorSpace color_space;
} JpgDecInfo;

typedef struct {
	float value;
	uint32_t output_lines; 
    int status;
} JpgDecDataInfo;

typedef struct {
    uint32_t addr_cmd_ptr;
	uint32_t downscale;
	int quality;
    int mode;
    JpgColorSpace color_space;
    uint8_t color_alpha;
    uint8_t pad[3];
} JpgDecInParam;

typedef struct {
    uint64_t width_byte;
    uint32_t width;
    uint32_t height;
    uint32_t num_components;
    int mode;
    JpgColorSpace color_space;
	uint32_t downscale;
    uint32_t use_memory_space;
} JpgDecOutParam;

int JpgDecCreate(int * main_handle, JpgDecThreadInParam *in, JpgDecThreadOutParam *out);

int JpgDecOpen(int main_handle, int * sub_handle, const JpgDecSrc *src, uint32_t *space_allocated);

int JpgDecReadHeader(int main_handle, int sub_handle, JpgDecInfo *info);

int JpgDecSetParameter(int main_handle, int sub_handle, const JpgDecInParam *inParam, JpgDecOutParam  *outParam);

int JpgDecDecodeData(int main_handle, int sub_handle, uint8_t *data, const uint64_t *bytes_per_line, JpgDecDataInfo *dataInfo);

int JpgDecClose(int main_handle, int sub_handle);

int JpgDecDestroy(int main_handle);


EXTERN_END
