#pragma once

#include <sysutil/video.h>
#include <rsx/gcm.h>

extern u32 *buffer[2];
extern gcmContextData *context; 
extern VideoResolution res;

// Initilize the screen and rsx
void init_screen();

// Block the PPU thread untill the previous flip operation has finished.
void waitFlip();

void flip(s32 buffer);

// Needs to be called each frame to map the buffers and setup render target
void setupRenderTarget(u32 currentBuffer);
