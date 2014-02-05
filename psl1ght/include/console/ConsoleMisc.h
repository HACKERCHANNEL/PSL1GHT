#ifndef __CONSOLEMISC_INCLUDED__
#define __CONSOLEMISC_INCLUDED__

#include <console/ConsoleBase.h>

#include <console/ConsoleLogic.h>


typedef struct _CONSOLE_MISC_PANEL
{
	ULONG Columns;
	ULONG Rows;
	ULONG DrawPosX;
	ULONG DrawPosY;
	UCHAR DrawTextColor;
	UCHAR DrawBackgroundColor;
	PATTR_CHAR_PAIR AttrCharPairGrid;
	int ScrollLockEnabled;
}CONSOLE_MISC_PANEL,*PCONSOLE_MISC_PANEL;

int CONSOLE_MISC_PANEL_Initialize(PCONSOLE_MISC_PANEL Panel, ULONG Columns, ULONG Rows);
int CONSOLE_MISC_PANEL_Finalize(PCONSOLE_MISC_PANEL Panel);
int CONSOLE_MISC_PANEL_BltToLogic(PCONSOLE_MISC_PANEL Panel, PCONSOLE_LOGIC DstConsole, ULONG SrcX, ULONG SrcWidth, ULONG SrcY, ULONG SrcHeight, ULONG DstX, ULONG DstWidth, ULONG DstY, ULONG DstHeight);
int CONSOLE_MISC_PANEL_BltToPanel(PCONSOLE_MISC_PANEL Panel, PCONSOLE_MISC_PANEL DstPanel, ULONG SrcX, ULONG SrcWidth, ULONG SrcY, ULONG SrcHeight, ULONG DstX, ULONG DstWidth, ULONG DstY, ULONG DstHeight);
ULONG CONSOLE_MISC_PANEL_GetDrawPosX(PCONSOLE_MISC_PANEL Panel);
ULONG CONSOLE_MISC_PANEL_GetDrawPosY(PCONSOLE_MISC_PANEL Panel);
int CONSOLE_MISC_PANEL_SetDrawPosX(PCONSOLE_MISC_PANEL Panel, ULONG PosX);
int CONSOLE_MISC_PANEL_SetDrawPosY(PCONSOLE_MISC_PANEL Panel, ULONG PosY);
UCHAR CONSOLE_MISC_PANEL_GetDrawTextColor(PCONSOLE_MISC_PANEL Panel);
UCHAR CONSOLE_MISC_PANEL_GetDrawBackgroundColor(PCONSOLE_MISC_PANEL Panel);
int CONSOLE_MISC_PANEL_SetDrawTextColor(PCONSOLE_MISC_PANEL Panel, UCHAR TextColor);
int CONSOLE_MISC_PANEL_SetDrawBackgroundColor(PCONSOLE_MISC_PANEL Panel, UCHAR BackgroundColor);
int CONSOLE_MISC_PANEL_GetTextColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, PUCHAR pTextColor);
int CONSOLE_MISC_PANEL_GetBackgroundColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, PUCHAR pBackgroundColor);
int CONSOLE_MISC_PANEL_SetTextColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, UCHAR TextColor);
int CONSOLE_MISC_PANEL_SetBackgroundColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, UCHAR BackgroundColor);
void CONSOLE_MISC_PANEL_putc(PCONSOLE_MISC_PANEL Panel, int UseDrawTextColor, int UseDrawBackgroundColor, UCHAR C);
UCHAR CONSOLE_MISC_PANEL_getc(PCONSOLE_MISC_PANEL Panel);
void CONSOLE_MISC_PANEL_puts(PCONSOLE_MISC_PANEL Panel, int UseDrawTextColor, int UseDrawBackgroundColor, char* S);
void CONSOLE_MISC_PANEL_gets(PCONSOLE_MISC_PANEL Panel, PUCHAR S, ULONG MaxLength, int CopyBeyondNullChar);
void CONSOLE_MISC_PANEL_printf(PCONSOLE_MISC_PANEL Panel, int UseDrawTextColor, int UseDrawBackgroundColor, char* FormatString, ...);
void CONSOLE_MISC_PANEL_Clear(PCONSOLE_MISC_PANEL Panel);
void CONSOLE_MISC_PANEL_SetScrollLock(PCONSOLE_MISC_PANEL Panel, int ScrollLockEnabled);

#endif//__CONSOLEMISC_INCLUDED__
