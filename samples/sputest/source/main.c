#include <psl1ght/lv2.h>
#include <psl1ght/lv2/spu.h>
#include <lv2/spu.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spu.bin.h"

int main(int argc, const char* argv[])
{
	u32 spu = 0;
	sysSpuImage image;
	u32 entry = 0;
	u32 segmentcount = 0;
	sysSpuSegment* segments;

	printf("Initializing 6 SPUs... ");
	printf("%08x\n", lv2SpuInitialize(6, 5));
	printf("Initializing raw SPU... ");
	printf("%08x\n", lv2SpuRawCreate(&spu, NULL));

	printf("Getting ELF information... ");
	printf("%08x\n", sysSpuElfGetInformation(spu_bin, &entry, &segmentcount));
	printf("\tEntry Point: %08x\n\tSegment Count: %08x\n", entry, segmentcount);

	size_t segmentsize = sizeof(sysSpuSegment) * segmentcount;
	segments = (sysSpuSegment*)malloc(segmentsize);
	memset(segments, 0, segmentsize);

	printf("Getting ELF segments... ");
	printf("%08x\n", sysSpuElfGetSegments(spu_bin, segments, segmentcount));

	printf("Loading ELF image... ");
	printf("%08x\n", sysSpuImageImport(&image, spu_bin, 0));

/*	image.type = SYS_SPU_IMAGE_TYPE_USER;
	image.entryPoint = entry;
	image.segments = (u32)(u64)segments;
	image.segmentCount = segmentcount; */

	printf("Loading image into SPU... ");
	printf("%08x\n", sysSpuRawImageLoad(spu, &image));

	printf("Running SPU...\n");
	lv2SpuRawWriteProblemStorage(spu, SPU_RunCntl, 1);

	printf("Waiting for SPU to return...\n");
	while (!(lv2SpuRawReadProblemStorage(spu, SPU_MBox_Status) & 1)) {
		asm volatile("eieio" ::);
	}

	printf("SPU mailbox return value: %08x\n", lv2SpuRawReadProblemStorage(spu, SPU_Out_MBox));

	printf("Destroying SPU... ");
	printf("%08x\n", lv2SpuRawDestroy(spu));

	printf("Closing image... ");
	printf("%08x\n", sysSpuImageClose(&image));

	return 0;
}
