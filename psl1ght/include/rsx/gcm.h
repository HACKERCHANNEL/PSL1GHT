/***
 * These are functions that are provided by libgcm_sys.sprx, which is the only
 * part of libgcm that we are leagally allowed to use.
 *
 * TODO: Fill out the rest of theses based on psl1ght/sprx/libgcm_sys/exports.h
 */

#pragma once

#include <psl1ght/types.h>
#include <stdlib.h>

EXTERN_BEGIN

// Theres are all ment to be pointers.
typedef struct{
	u32 begin;
	u32 end;
	u32 current;
	u32 callback;
} gcmContextData;

s32 gcmInitBody(u32 *contex, const u32 cmdSize, const u32 ioSize, const void* ioAddress);

#define GCM_FLIP_VSYNC 2
#define GCM_FLIP_HSYNC 1
#define GCM_FLIP_HSYNC_AND_BREAK_EVERYTHING 3
void gcmSetFlipMode(s32 mode);

typedef struct {
	u32 localAddress;
	u32 ioAddress;
	s32 localSize;
	s32 ioSize;
	s32 memoryFreq;
	s32 coreFreq;
} gcmConfiguration;

void gcmGetConfiguration(gcmConfiguration *config);

s32 gcmAddressToOffset(u32 address, u32 *offset);

s32 gcmSetDisplayBuffer(u32 bufferId, u32 offset, u32 pitch, u32 width, u32 height);
s32 gcmSetFlip(gcmContextData *context, u32 bufferId);
void gcmSetWaitFlip(gcmContextData *context);
s32 gcmGetFlipStatus();
void gcmResetFlipStatus();

typedef struct {
	u32 put;
	u32 get;
	u32 ref;
} gcmControlRegister;

gcmControlRegister *gcmGetControlRegister();
void gcmFlushBuffer(gcmContextData *context);

EXTERN_END
