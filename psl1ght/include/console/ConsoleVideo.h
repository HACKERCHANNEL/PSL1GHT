#ifndef __CONSOLEVIDEO_INCLUDED__
#define __CONSOLEVIDEO_INCLUDED__

#include <console/ConsoleBase.h>

typedef struct _CONSOLE_VIDEO
{
	PUCHAR FrameBuffer;
	ULONG ResolutionX;
	ULONG ResolutionY;
	ULONG ResolutionBPP;
	ULONG ScanLine;
	ULONG ViewPortX;
	ULONG ViewPortWidth;
	ULONG ViewPortY;
	ULONG ViewPortHeight;
//	PUCHAR ViewPortBackBuffer;
	ULONG ViewPortScanLine;
	PUCHAR ViewPortStart;
}CONSOLE_VIDEO,*PCONSOLE_VIDEO;

int CONSOLE_VIDEO_Initialize(PCONSOLE_VIDEO Video, PUCHAR FrameBuffer, ULONG ResolutionX, ULONG ResolutionY, UCHAR ResolutionBPP, ULONG ResolutionScanLine, ULONG ViewPortX, ULONG ViewPortWidth, ULONG ViewPortY, ULONG ViewPortHeight);
int CONSOLE_VIDEO_Finalize(PCONSOLE_VIDEO Video);
int CONSOLE_VIDEO_Save(PCONSOLE_VIDEO Video);
int CONSOLE_VIDEO_Restore(PCONSOLE_VIDEO Video);
int CONSOLE_VIDEO_PutViewPortX(PCONSOLE_VIDEO Video, ULONG ViewPortX);
int CONSOLE_VIDEO_PutViewPortY(PCONSOLE_VIDEO Video, ULONG ViewPortY);
int CONSOLE_VIDEO_PutViewPortWidth(PCONSOLE_VIDEO Video, ULONG ViewPortWidth);
int CONSOLE_VIDEO_PutViewPortHeight(PCONSOLE_VIDEO Video, ULONG ViewPortHeight);
int CONSOLE_VIDEO_GetViewPortX(PCONSOLE_VIDEO Video, PULONG pViewPortX);
int CONSOLE_VIDEO_GetViewPortY(PCONSOLE_VIDEO Video, PULONG pViewPortY);
int CONSOLE_VIDEO_GetViewPortWidth(PCONSOLE_VIDEO Video, PULONG pViewPortWidth);
int CONSOLE_VIDEO_GetViewPortHeight(PCONSOLE_VIDEO Video, PULONG pViewPortHeight);
int CONSOLE_VIDEO_PutPixel(PCONSOLE_VIDEO Video, ULONG X, ULONG Y, ULONG Color);

#endif//__CONSOLEVIDEO_INCLUDED__
