#include "sysutil/events.h"
opd32 newopdcallback[EVENT_SLOTMAX];
s32 sysRegisterCallback(sysEventSlot slot, sysCallback func, void * userdata)
{
	opd64 * opd = (opd64*)func;
	newopdcallback[slot].func = (u64)opd->func;
	newopdcallback[slot].rtoc = (u64)opd->rtoc;
	
	return sysRegisterCallback_ex(slot, &newopdcallback[slot], userdata);
}

