#pragma once

#include <psl1ght/types.h>
#include <stdlib.h>

EXTERN_BEGIN

#define EVENT_REQUEST_EXITAPP 0x101

typedef enum sysEventSlot{
	EVENT_SLOT0,
	EVENT_SLOT1,
	EVENT_SLOT2,
	EVENT_SLOT3,
	EVENT_SLOTMAX,
} sysEventSlot;

typedef void (*sysCallback)(u64 status,	u64 param, void * userdata);

s32 sysRegisterCallback(sysEventSlot slot, sysCallback func, void * userdata);
s32 sysRegisterCallback_ex(sysEventSlot slot, opd32* opdentry, void * userdata);
s32 sysCheckCallback();
s32 sysUnregisterCallback(sysEventSlot slot);

EXTERN_END

