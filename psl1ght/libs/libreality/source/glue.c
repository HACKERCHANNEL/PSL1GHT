#include "rsx/reality.h"
#include "rsx/gcm.h"

void realityFlushBuffer(gcmContextData *context) {
	gcmControlRegister *control = gcmGetControlRegister(context);
	__asm __volatile__("sync"); // Sync, to make sure the command was written;
	u32 offset;
	gcmAddressToOffset(context->current, &offset);
	control->put = offset;
}

/* Maps the memory at ioAddress into the RSX's memory space so the userspace thread
 * and the RSX can comunicate.
 * This shared memory must be 1mb aligned, and at least 1mb long.
 * Also Initilizes a RSX command buffer of cmdSize inside the shared memory. 
 * 
 * Returns a context structure.
 */
gcmContextData *realityInit(const u32 cmdSize, const u32 ioSize, const void* ioAddress) {
	u32 contextPointer;
	s32 ret = gcmInitBody(&contextPointer, cmdSize, ioSize, ioAddress);
	if (ret == 0) {
		// Double cast fixes warning about diffrent pointer sizes.
		gcmContextData *context = (gcmContextData *) (u64) contextPointer;
		return context;
	}
	return NULL;
}

int realityAddressToOffset(void* ptr, u32 *offset) {
	// Double cast for warnings.
	return gcmAddressToOffset((u32) (u64) ptr, offset);
}
