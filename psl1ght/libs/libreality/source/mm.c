#include "rsx/reality.h"

s8 initialized = 0;
gcmConfiguration config;
void *heap_pointer;

// Really dumb allocater, It will do until we start dealing with textures and vertex buffers.
void *rsxMemAlign(s32 alignment, s32 size) {
	if (!initialized) {
		gcmGetConfiguration(&config);
		initialized = 1;
		heap_pointer = (void *)(u64) config.localAddress;
	}
	void *pointer = heap_pointer;
	pointer = (void *)((((u64) pointer) + (alignment-1)) & (-alignment)); // Align
	if ((u64) pointer + size > config.localAddress + config.localSize) // Out of memory?
		return NULL;
	heap_pointer = (void *)((u64) pointer + size);
	return pointer;
}

void *rsxMem(s32 size) {
	return rsxMemAlign(size, 0);
}


