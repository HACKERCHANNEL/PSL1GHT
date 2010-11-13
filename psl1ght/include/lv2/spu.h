#pragma once

#include <psl1ght/types.h>

#define SYS_SPU_SEGMENT_TYPE_COPY	0x01
#define SYS_SPU_SEGMENT_TYPE_FILL	0x02
#define SYS_SPU_SEGMENT_TYPE_INFO	0x04

#define SYS_SPU_IMAGE_TYPE_USER		0x00
#define SYS_SPU_IMAGE_TYPE_KERNEL	0x01

typedef struct {
	u32 type;
	u32 lsStart;
	u32 size;
	union {
		u32 paStart;	// When type == COPY
		u32 value;		// When type == FILL
	};
} sysSpuSegment;

typedef struct {
	u32 type;
	u32 entryPoint;
	u32 segments; // Actually sysSpuSegment*
	u32 segmentCount;
} sysSpuImage;

EXTERN_BEGIN

int sysSpuElfGetInformation(const void* elf, u32* entryPoint, u32* segmentCount);
int sysSpuElfGetSegments(const void* elf, sysSpuSegment* segments, u32 segmentCount);

int sysSpuRawLoad(u32 spu, const char* path, u32* entryPoint);
int sysSpuRawImageLoad(u32 spu, sysSpuImage* image);

int sysSpuImageClose(sysSpuImage* image);
int sysSpuImageImport(sysSpuImage* image, const void* elf, u32 type);

EXTERN_END

