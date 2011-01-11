#pragma once

#include <psl1ght/lv2.h>
#include <psl1ght/lv2/filesystem.h>
#include <lv2/spu.h>

#define LV2_SPU_THREAD_ATTRIBUTE_NONE				0x00
#define LV2_SPU_THREAD_ATTRIBUTE_ASYNC_INTR_ENABLE	0x01
#define LV2_SPU_THREAD_ATTRIBUTE_DEC_SYNC_TB_ENABLE	0x02

typedef struct {
	u64 argument1;
	u64 argument2;
	u64 argument3;
	u64 argument4;
} Lv2SpuThreadArguments;

typedef struct {
	u32 nameAddress;
	u32 nameSize;
	u32 attribute;
} Lv2SpuThreadAttributes;

typedef struct {
	u32 nameSize;
	u32 nameAddress;
	u32 groupType;
	u32 memContainer;
} Lv2SpuThreadGroupAttributes;

LV2_SYSCALL lv2SpuInitialize(u32 spus, u32 rawspus) { return Lv2Syscall2(169, spus, rawspus); }

LV2_SYSCALL lv2SpuRawCreate(u32* spu, u32* attributes) { return Lv2Syscall2(160, (u64)spu, (u64)attributes); }
LV2_SYSCALL lv2SpuRawDestroy(u32 spu) { return Lv2Syscall1(161, spu); }
LV2_SYSCALL lv2SpuRawCreateInterruptTag(u32 spu, u32 classid, u32 hardwarethread, u32* tag) { return Lv2Syscall4(150, spu, classid, hardwarethread, (u64)tag); }
LV2_SYSCALL lv2SpuRawSetIntMask(u32 spu, u32 classid, u64 mask) { return Lv2Syscall3(151, spu, classid, mask); }
LV2_SYSCALL lv2SpuRawGetIntMask(u32 spu, u32 classid, u64* mask) { return Lv2Syscall3(152, spu, classid, (u64)mask); }
LV2_SYSCALL lv2SpuRawSetIntStat(u32 spu, u32 classid, u64 stat) { return Lv2Syscall3(153, spu, classid, stat); }
LV2_SYSCALL lv2SpuRawGetIntStat(u32 spu, u32 classid, u64* stat) { return Lv2Syscall3(154, spu, classid, (u64)stat); }
LV2_SYSCALL lv2SpuRawReadPuintMb(u32 spu, u32* value) { return Lv2Syscall2(163, spu, (u64)value); }
LV2_SYSCALL lv2SpuRawSetConfiguration(u32 spu, u32 value) { return Lv2Syscall2(196, spu, value); }
LV2_SYSCALL lv2SpuRawGetConfirugation(u32 spu, u32* value) { return Lv2Syscall2(197, spu, (u64)value); }
LV2_SYSCALL lv2SpuRawRecoverPageFault(u32 spu) { return Lv2Syscall1(199, spu); }

LV2_SYSCALL lv2SpuImageOpen(sysSpuImage* image, const char* path) { return Lv2Syscall2(156, (u64)image, (u64)path); }
LV2_SYSCALL lv2SpuImageOpenFd(sysSpuImage* image, Lv2FsFile fd, u64 offset) { return Lv2Syscall3(260, (u64)image, fd, offset); }

LV2_SYSCALL lv2SpuThreadInitialize(u32* thread, u32 group, u32 spu, sysSpuImage* image, Lv2SpuThreadAttributes* attributes, Lv2SpuThreadArguments* arguments) { return Lv2Syscall6(172, (u64)thread, (u64)group, spu, (u64)image, (u64)attributes, (u64)arguments); }
LV2_SYSCALL lv2SpuThreadSetArguments(u32 thread, Lv2SpuThreadArguments* arguments) { return Lv2Syscall2(166, thread, (u64)arguments); }
LV2_SYSCALL lv2SpuThreadGetExitStatus(u32 thread, int* status) { return Lv2Syscall2(165, thread, (u64)status); }
LV2_SYSCALL lv2SpuThreadConnectEvent(u32 thread, u32 queue, u32 type, u8 spup) { return Lv2Syscall4(191, thread, queue, type, spup); }
LV2_SYSCALL lv2SpuThreadDisconnectEvent(u32 thread, u32 type, u8 spup) { return Lv2Syscall3(192, thread, type, spup); }
LV2_SYSCALL lv2SpuThreadBindQueue(u32 thread, u32 queue, u32 spuq_num) { return Lv2Syscall3(193, thread, queue, spuq_num); }
LV2_SYSCALL lv2SpuThreadUnbindQueue(u32 thread, u32 spuq_num) { return Lv2Syscall2(194, thread, spuq_num); }
LV2_SYSCALL lv2SpuThreadWriteLocalStorage(u32 thread, u32 address, u64 value, u32 type) { return Lv2Syscall4(181, thread, address, value, type); }
LV2_SYSCALL lv2SpuThreadReadLocalStorage(u32 thread, u32 address, u64* value, u32 type) { return Lv2Syscall4(182, thread, address, (u64)value, type); }
LV2_SYSCALL lv2SpuThreadSetConfiguration(u32 thread, u64 value) { return Lv2Syscall2(187, thread, value); }
LV2_SYSCALL lv2SpuThreadGetConfiguration(u32 thread, u64* value) { return Lv2Syscall2(188, thread, (u64)value); }
LV2_SYSCALL lv2SpuThreadWriteMb(u32 thread, u32 value) { return Lv2Syscall2(190, thread, value); }
LV2_SYSCALL lv2SpuThreadRecoverPageFault(u32 thread) { return Lv2Syscall1(198, thread); }

LV2_SYSCALL lv2SpuThreadGroupCreate(u32* group, u32 num, u32 pri, Lv2SpuThreadGroupAttributes * attr) { return Lv2Syscall4(170, (u64)group, num, pri, (u64)attr); }
LV2_SYSCALL lv2SpuThreadGroupDestroy(u32 group) { return Lv2Syscall1(171, group); }
LV2_SYSCALL lv2SpuThreadGroupStart(u32 group) { return Lv2Syscall1(173, group); }
LV2_SYSCALL lv2SpuThreadGroupSuspend(u32 group) { return Lv2Syscall1(174, group); }
LV2_SYSCALL lv2SpuThreadGroupResume(u32 group) { return Lv2Syscall1(175, group); }
LV2_SYSCALL lv2SpuThreadGroupYield(u32 group) { return Lv2Syscall1(176, group); }
LV2_SYSCALL lv2SpuThreadGroupTerminate(u32 group, u32 value) { return Lv2Syscall2(177, group, value); }
LV2_SYSCALL lv2SpuThreadGroupJoin(u32 gid, u32 *cause, u32 *status) { return Lv2Syscall3(178, gid, (u64) cause, (u64)status); }
LV2_SYSCALL lv2SpuThreadGroupSetPriority(u32 group, u32 pri) { return Lv2Syscall2(179, group, pri); }
LV2_SYSCALL lv2SpuThreadGroupGetPriority(u32 group, u32 *pri) { return Lv2Syscall2(180, group, (u64)pri); }
LV2_SYSCALL lv2SpuThreadGroupConnectEvent(u32 group, u32 eq, u32 et) { return Lv2Syscall3(185, group, eq, et); }
LV2_SYSCALL lv2SpuThreadGroupDisconnectEvent(u32 group, u32 et) { return Lv2Syscall2(186, group, et); }
LV2_SYSCALL lv2SpuThreadGroupConnectEventAllThreads(u32 group, u32 eq, u64 req, u8 *spup) { return Lv2Syscall4(251, group, eq, req, (u64)spup); }
LV2_SYSCALL lv2SpuThreadGroupDisconnectEventAllThreads(u32 group, u8 spup) { return Lv2Syscall2(252, group, spup); }

#define LV2_SPU_RAW_BASE			0xE0000000ULL
#define LV2_SPU_RAW_OFFSET			0x00100000ULL
#define LV2_SPU_RAW_LOCAL_OFFSET	0x00000000ULL
#define LV2_SPU_RAW_PROBLEM_OFFSET	0x00040000ULL

#define LV2_SPU_THREAD_BASE			0xF0000000ULL
#define LV2_SPU_THREAD_OFFSET		0x00100000ULL

// Problem State Registers
#define MFC_LSA						0x00003004
#define MFC_EAH						0x00003008
#define MFC_EAL						0x0000300C
#define MFC_Size_Tag				0x00003010
#define MFC_Class_CMD				0x00003014
#define MFC_CMDStatus				0x00003014
#define MFC_QStatus					0x00003104
#define Prxy_QueryType				0x00003204
#define Prxy_QueryMask				0x0000321C
#define Prxy_TagStatus				0x0000322C
#define SPU_Out_Intr_MBox			0x00004000
#define SPU_Out_MBox				0x00004004
#define SPU_In_MBox					0x0000400C
#define SPU_MBox_Status				0x00004014
#define SPU_RunCntl					0x0000401C
#define SPU_Status					0x00004024
#define SPU_NPC						0x00004034
#define SPU_Sig_Notify_1			0x0001400C
#define SPU_Sig_Notify_2			0x0001C00C

#define LV2_SPU_RAW_GET_BASE_OFFSET(spu) \
	(LV2_SPU_RAW_BASE + LV2_SPU_RAW_OFFSET * (spu))

#define LV2_SPU_RAW_GET_PROBLEM_STORAGE(spu, reg) \
	(vu32*)(LV2_SPU_RAW_GET_BASE_OFFSET(spu) + LV2_SPU_RAW_PROBLEM_OFFSET + reg)

static inline void lv2SpuRawWriteProblemStorage(u32 spu, u32 reg, u32 value) {
	*LV2_SPU_RAW_GET_PROBLEM_STORAGE(spu, reg) = value;
}

static inline u32 lv2SpuRawReadProblemStorage(u32 spu, u32 reg) {
	return *LV2_SPU_RAW_GET_PROBLEM_STORAGE(spu, reg);
}

#define LV2_SPU_RAW_GET_LOCAL_STORAGE(spu, reg) \
	(vu32*)(LV2_SPU_RAW_GET_BASE_OFFSET(spu) + LV2_SPU_RAW_LOCAL_OFFSET + reg)
static inline void lv2SpuRawWriteLocalStorage(u32 spu, u32 reg, u32 value) {
	*LV2_SPU_RAW_GET_LOCAL_STORAGE(spu, reg) = value;
}

static inline u32 lv2SpuRawReadLocalStorage(u32 spu, u32 reg) {
	return *LV2_SPU_RAW_GET_LOCAL_STORAGE(spu, reg);
}

#define LV2_SPU_THREAD_GET_BASE_OFFSET(spu) \
	(LV2_SPU_THREAD_BASE + LV2_SPU_THREAD_OFFSET * (spu))

#define LV2_SPU_THREAD_GET_PROBLEM_STORAGE(spu, reg) \
	(vu32*)(LV2_SPU_THREAD_GET_BASE_OFFSET(spu) + LV2_SPU_RAW_PROBLEM_OFFSET + reg)

static inline void lv2SpuThreadWriteProblemStorage(u32 spu, u32 reg, u32 value) {
	*LV2_SPU_THREAD_GET_PROBLEM_STORAGE(spu, reg) = value;
}

static inline u32 lv2SpuThreadReadProblemStorage(u32 spu, u32 reg) {
	return *LV2_SPU_THREAD_GET_PROBLEM_STORAGE(spu, reg);
}

#define LV2_SPU_THREAD_GET_LOCAL_STORAGE(spu, reg) \
	(vu32*)(LV2_SPU_THREAD_GET_BASE_OFFSET(spu) + LV2_SPU_RAW_LOCAL_OFFSET + reg)

/*
static inline void lv2SpuThreadWriteLocalStorage(u32 spu, u32 reg, u32 value) {
	*LV2_SPU_THREAD_GET_LOCAL_STORAGE(spu, reg) = value;
}

static inline u32 lv2SpuThreadReadLocalStorage(u32 spu, u32 reg) {
	return *LV2_SPU_THREAD_GET_LOCAL_STORAGE(spu, reg);
}
*/
