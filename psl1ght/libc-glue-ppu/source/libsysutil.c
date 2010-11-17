#include "sysutil/events.h"

s32 sysRegisterCallback(sysEventSlot slot, sysCallback func, void * userdata)
{
	return sysRegisterCallback_ex(slot, OPD32(func), userdata);
}

