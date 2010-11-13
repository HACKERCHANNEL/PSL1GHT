#pragma once

#include <psl1ght/lv2.h>

LV2_SYSCALL lv2MemContinerCreate(mem_container_t* container, u32 size) { return Lv2Syscall2(324,(u64)container, size); }
LV2_SYSCALL lv2MemContinerDestroy(mem_container_t container) { return Lv2Syscall1(325,container); }
