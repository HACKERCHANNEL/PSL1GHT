#include <rsx/buffer.h>
#include <assert.h>

// Thanks to shagkur for working out how to get gcc to compile this callback
// I highly recomend you don't try and "improve" this unless you are a gcc ninja.
s32 __attribute__((noinline)) rsxContextCallback(gcmContextData *context,u32 count)
{
  register s32 result asm("3");
  asm volatile (
    "stdu  1,-128(1)\n" // Make space on stack for clobbered registers
    "mr    31,2\n"	// Save rtoc
    "lwz  0,0(%0)\n"	// load callback funcion pointer
    "lwz  2,4(%0)\n"	// load callback's rtoc
    "mtctr  0\n"	
    "bctrl\n"		// Branch to callback function
    "mr    2,31\n"	// restore rtoc
    "addi  1,1,128\n"	// restore stack pointer
    : : "b"(context->callback)
    : "r31", "r0", "r1", "r2", "lr"
  );
  return result;
}

void commandBufferPut(gcmContextData* context, uint32_t value) {
	uint32_t* buffer = (uint32_t *)(uint64_t) context->current;
	 *buffer++ = value;
	context->current = (uint32_t)(uint64_t) buffer;
}

