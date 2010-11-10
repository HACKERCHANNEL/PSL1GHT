#include "config.h"

#include <stdint.h>

typedef struct {
	uint32_t header1;
	uint16_t header2;
	uint16_t imports;
	uint32_t zero1;
	uint32_t zero2;
	const char* name;
	const void* fnid;
	const void* fstub;
	uint32_t zero3;
	uint32_t zero4;
	uint32_t zero5;
	uint32_t zero6;
} __attribute__((__packed__)) prx_header;

extern uint32_t LIBRARY_SYMBOL __attribute__((section(".rodata.sceFNID")));
static const void* scefstub[0] __attribute__((section(".data.sceFStub")));

static const uint32_t version __attribute__((section(".rodata.sceResident"))) = 0;
static const const char name[] __attribute__((section(".rodata.sceResident"))) = LIBRARY_NAME;

static prx_header header __attribute__((section(".lib.stub"))) = {
	LIBRARY_HEADER_1,
	LIBRARY_HEADER_2,
	0,
	0,
	0,
	name,
	&LIBRARY_SYMBOL,
	scefstub,
	0,
	0,
	0,
	0
};

#define EXPORT(name, fnid) \
	extern void* __##name; \
	const void* name##_stub __attribute__((section(".data.sceFStub"))) = &__##name; \
	const uint32_t name##_fnid __attribute__((section(".rodata.sceFNID"))) = fnid;

#include "exports.h"
