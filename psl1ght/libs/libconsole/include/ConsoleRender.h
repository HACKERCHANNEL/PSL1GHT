#ifndef __CONSOLERENDER_INCLUDED__
#define __CONSOLERENDER_INCLUDED__

#include <ConsoleBase.h>

#include <ConsoleVideo.h> 
#include <ConsoleLogic.h>

typedef struct _CONSOLE_RENDER
{
	UCHAR CursorBlinkFrames;
	UCHAR CurrentCursorBlinkFrames;
	UCHAR CurrentCursorBlinkState;
	PUCHAR CursorMaskBuffer;
	PUCHAR CursorInsertMaskBuffer;
	PUCHAR MousePointerMaskBuffer;
	UCHAR CharacterWidth;
	UCHAR CharacterHeight;
	PUCHAR CharacterMasksBuffer;
	PULONG BasicColors;
	ULONG RenderingFlags;
	UCHAR DropShadowOrientation;
	ULONG DropShadowColor;
	UCHAR TransparentTextModeColor;
}CONSOLE_RENDER,*PCONSOLE_RENDER;

#define CONSOLE_RENDER_RENDERING_FLAG_TRANSPARENT					1
#define CONSOLE_RENDER_RENDERING_FLAG_DROPSHADOW					2
#define CONSOLE_RENDER_RENDERING_FLAG_DROPSHADOWOVERANYBACKGROUND	4

#define CONSOLE_RENDER_DROPSHADOWORIENTATION_NORTH		0
#define CONSOLE_RENDER_DROPSHADOWORIENTATION_NORTHEAST	1
#define CONSOLE_RENDER_DROPSHADOWORIENTATION_EAST		2
#define CONSOLE_RENDER_DROPSHADOWORIENTATION_SOUTHEAST	3
#define CONSOLE_RENDER_DROPSHADOWORIENTATION_SOUTH		4
#define CONSOLE_RENDER_DROPSHADOWORIENTATION_SOUTHWEST	5
#define CONSOLE_RENDER_DROPSHADOWORIENTATION_WEST		6
#define CONSOLE_RENDER_DROPSHADOWORIENTATION_NORTHWEST	7

#define CONSOLE_RENDER_DROPSHADOWORIENTATIONS	8

int CONSOLE_RENDER_Initialize(PCONSOLE_RENDER Render, UCHAR SlotWidth, UCHAR SlotHeight, ULONG CursorBlinkFrames, PULONG pBasicColors, PUCHAR pCharacterMasks, PUCHAR pCursorMask, PUCHAR pCursorInsertMask, PUCHAR pMousePointerMask);
int CONSOLE_RENDER_Finalize(PCONSOLE_RENDER Render);
int CONSOLE_RENDER_Render(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video);
int CONSOLE_RENDER_RenderChanges(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video);
int CONSOLE_RENDER_SetDropShadowOrientation(PCONSOLE_RENDER Render, UCHAR DropShadowOrientation);
int CONSOLE_RENDER_SetDropShadowColor(PCONSOLE_RENDER Render, ULONG DropShadowColor);
int CONSOLE_RENDER_SetTransparentTextModeColor(PCONSOLE_RENDER Render, UCHAR TransparentTextModeColor);
int CONSOLE_RENDER_SetRenderingOptions(PCONSOLE_RENDER Render, ULONG RenderingFlags);
int CONSOLE_RENDER_SetRenderingOptions(PCONSOLE_RENDER Render, ULONG RenderingFlags);
int CONSOLE_RENDER_GetDropShadowOrientation(PCONSOLE_RENDER Render, PUCHAR pDropShadowOrientation);
int CONSOLE_RENDER_GetDropShadowColor(PCONSOLE_RENDER Render, PULONG pDropShadowColor);
int CONSOLE_RENDER_GetTransparentTextModeColor(PCONSOLE_RENDER Render, PUCHAR pTransparentTextModeColor);
int CONSOLE_RENDER_GetRenderingOptions(PCONSOLE_RENDER Render, PULONG pRenderingFlags);

#endif//__CONSOLERENDER_INCLUDED__
