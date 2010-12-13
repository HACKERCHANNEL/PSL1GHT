#include <console/ConsoleMisc.h>

int CONSOLE_MISC_PANEL_Initialize(PCONSOLE_MISC_PANEL Panel, ULONG Columns, ULONG Rows)
{
	Panel->AttrCharPairGrid = malloc(Columns*Rows*sizeof(ATTR_CHAR_PAIR));

	if(Panel->AttrCharPairGrid != NULL)
	{
		memset(Panel->AttrCharPairGrid,0,Columns*Rows*sizeof(ATTR_CHAR_PAIR));

		Panel->Columns = Columns;
		Panel->Rows = Rows;
		Panel->DrawBackgroundColor = CONSOLE_BASIC_COLOR_BLACK;
		Panel->DrawTextColor = CONSOLE_BASIC_COLOR_LIGHTGRAY;
		Panel->DrawPosX = 0;
		Panel->DrawPosY = 0;
		Panel->ScrollLockEnabled = FALSE;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_Finalize(PCONSOLE_MISC_PANEL Panel)
{
	free(Panel->AttrCharPairGrid);

	Panel->AttrCharPairGrid = NULL;

	return TRUE;
}

int CONSOLE_MISC_PANEL_BltToLogic(PCONSOLE_MISC_PANEL Panel, PCONSOLE_LOGIC DstConsole, ULONG SrcX, ULONG SrcWidth, ULONG SrcY, ULONG SrcHeight, ULONG DstX, ULONG DstWidth, ULONG DstY, ULONG DstHeight)
{
	int i,j;
	ULONG EffectiveWidth,EffectiveHeight;

	if(((SrcX+SrcWidth)<=Panel->Columns)&&((SrcY+SrcHeight)<=Panel->Rows)&&((DstX+DstWidth)<=DstConsole->Columns)&&((SrcY+SrcHeight)<=DstConsole->Rows))
	{
		EffectiveWidth = (SrcWidth<DstWidth)?SrcWidth:DstWidth;
		EffectiveHeight = (SrcHeight<DstHeight)?SrcHeight:DstHeight;

		for(j = 0;j<EffectiveHeight;j++)
		{	
			for(i = 0;i<EffectiveWidth;i++)
			{
				CONSOLE_LOGIC_PutAttrCharPairXY(DstConsole,DstX+i,DstY+j,Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Attr,Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Char);
			}
		}

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_BltToPanel(PCONSOLE_MISC_PANEL Panel, PCONSOLE_MISC_PANEL DstPanel, ULONG SrcX, ULONG SrcWidth, ULONG SrcY, ULONG SrcHeight, ULONG DstX, ULONG DstWidth, ULONG DstY, ULONG DstHeight)
{
	int i,j;
	ULONG EffectiveWidth,EffectiveHeight;

	if(((SrcX+SrcWidth)<=Panel->Columns)&&((SrcY+SrcHeight)<=Panel->Rows)&&((DstX+DstWidth)<=DstPanel->Columns)&&((SrcY+SrcHeight)<=DstPanel->Rows))
	{
		EffectiveWidth = (SrcWidth<DstWidth)?SrcWidth:DstWidth;
		EffectiveHeight = (SrcHeight<DstHeight)?SrcHeight:DstHeight;

		if(Panel!=DstPanel)
		{
			for(j = 0;j<EffectiveHeight;j++)
			{	
				for(i = 0;i<EffectiveWidth;i++)
				{
					DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Attr = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Attr;
					DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Char = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Char;
				}
			}
		}
		else
		{
			if(SrcY<DstY)
			{
				for(j = EffectiveHeight-1;j>=0;j--)
				{
					for(i = 0;i<EffectiveWidth;i++)
					{
						DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Attr = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Attr;
						DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Char = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Char;
					}
				}
			}
			else
			{
				if(SrcY>DstY)
				{
					for(j = 0;j<EffectiveHeight;j++)
					{	
						for(i = 0;i<EffectiveWidth;i++)
						{
							DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Attr = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Attr;
							DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Char = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Char;
						}
					}
				}
				else
				{
					if(SrcX<DstX)
					{
						for(j = 0;j<EffectiveHeight;j++)
						{
							for(i = EffectiveWidth;i>=0;i--)
							{
								DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Attr = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Attr;
								DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Char = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Char;
							}
						}
					}
					else
					{
						if(SrcX>DstX)
						{
							for(j = 0;j<EffectiveHeight;j++)
							{	
								for(i = 0;i<EffectiveWidth;i++)
								{
									DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Attr = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Attr;
									DstPanel->AttrCharPairGrid[(DstPanel->Columns*(DstY+j))+(DstX+i)].Char = Panel->AttrCharPairGrid[(Panel->Columns*(SrcY+j))+(SrcX+i)].Char;
								}
							}
						}
					}
				}
			}
		}

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

ULONG CONSOLE_MISC_PANEL_GetDrawPosX(PCONSOLE_MISC_PANEL Panel)
{
	return Panel->DrawPosX;
}

ULONG CONSOLE_MISC_PANEL_GetDrawPosY(PCONSOLE_MISC_PANEL Panel)
{
	return Panel->DrawPosY;
}

int CONSOLE_MISC_PANEL_SetDrawPosX(PCONSOLE_MISC_PANEL Panel, ULONG PosX)
{
	if(PosX<Panel->Columns)
	{
		Panel->DrawPosX = PosX;
		
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_SetDrawPosY(PCONSOLE_MISC_PANEL Panel, ULONG PosY)
{
	if(PosY<Panel->Rows)
	{
		Panel->DrawPosY = PosY;

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

UCHAR CONSOLE_MISC_PANEL_GetDrawTextColor(PCONSOLE_MISC_PANEL Panel)
{
	return Panel->DrawTextColor;
}

UCHAR CONSOLE_MISC_PANEL_GetDrawBackgroundColor(PCONSOLE_MISC_PANEL Panel)
{
	return Panel->DrawBackgroundColor;
}

int CONSOLE_MISC_PANEL_SetDrawTextColor(PCONSOLE_MISC_PANEL Panel, UCHAR TextColor)
{
	if(TextColor<16)
	{
		Panel->DrawTextColor = TextColor;

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_SetDrawBackgroundColor(PCONSOLE_MISC_PANEL Panel, UCHAR BackgroundColor)
{
	if(BackgroundColor<16)
	{
		Panel->DrawBackgroundColor = BackgroundColor;

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_GetTextColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, PUCHAR pTextColor)
{
	if((X<Panel->Columns)&&(Y<Panel->Rows))
	{
		*pTextColor = (Panel->AttrCharPairGrid[(Y*Panel->Columns)+X].Attr)&0xF;

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_GetBackgroundColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, PUCHAR pBackgroundColor)
{
	if((X<Panel->Columns)&&(Y<Panel->Rows))
	{
		*pBackgroundColor = ((Panel->AttrCharPairGrid[(Y*Panel->Columns)+X].Attr)>>4)&0xF;

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_SetTextColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, UCHAR TextColor)
{
	if((X<Panel->Columns)&&(Y<Panel->Rows)&&(TextColor<16))
	{
		Panel->AttrCharPairGrid[(Y*Panel->Columns)+X].Attr &= 0xF0;
		Panel->AttrCharPairGrid[(Y*Panel->Columns)+X].Attr |= TextColor;

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int CONSOLE_MISC_PANEL_SetBackgroundColorAtXY(PCONSOLE_MISC_PANEL Panel, ULONG X, ULONG Y, UCHAR BackgroundColor)
{
	if((X<Panel->Columns)&&(Y<Panel->Rows)&&(BackgroundColor<16))
	{
		Panel->AttrCharPairGrid[(Y*Panel->Columns)+X].Attr &= 0x0F;
		Panel->AttrCharPairGrid[(Y*Panel->Columns)+X].Attr |= ((BackgroundColor<<4)&0xF0);

		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

int AdvanceDrawPos(PCONSOLE_MISC_PANEL Panel)
{
	Panel->DrawPosX++;

	if(Panel->DrawPosX==Panel->Columns)
	{
		Panel->DrawPosX = 0;
		Panel->DrawPosY++;

		if(Panel->DrawPosY==Panel->Rows)
		{
			Panel->DrawPosY--;

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

void CONSOLE_MISC_PANEL_putc(PCONSOLE_MISC_PANEL Panel, int UseDrawTextColor, int UseDrawBackgroundColor, UCHAR C)
{
	int i;

	Panel->AttrCharPairGrid[(Panel->DrawPosY*Panel->Columns)+Panel->DrawPosX].Char = C;

	if(UseDrawBackgroundColor)
	{
		Panel->AttrCharPairGrid[(Panel->DrawPosY*Panel->Columns)+Panel->DrawPosX].Attr &= 0x0F;
		Panel->AttrCharPairGrid[(Panel->DrawPosY*Panel->Columns)+Panel->DrawPosX].Attr |= ((Panel->DrawBackgroundColor<<4)&0xF0);
	}

	if(UseDrawTextColor)
	{
		Panel->AttrCharPairGrid[(Panel->DrawPosY*Panel->Columns)+Panel->DrawPosX].Attr &= 0xF0;
		Panel->AttrCharPairGrid[(Panel->DrawPosY*Panel->Columns)+Panel->DrawPosX].Attr |= Panel->DrawTextColor;
	}

	if((AdvanceDrawPos(Panel))&&(!Panel->ScrollLockEnabled))
	{
		CONSOLE_MISC_PANEL_BltToPanel(Panel,Panel,0,Panel->Columns,1,Panel->Rows-1,0,Panel->Columns,0,Panel->Rows-1);

		for(i = 0;i<Panel->Columns;i++)
		{
			Panel->AttrCharPairGrid[((Panel->Rows-1)*Panel->Columns)+i].Char = 0;	
		}
	}
}

UCHAR CONSOLE_MISC_PANEL_getc(PCONSOLE_MISC_PANEL Panel)
{
	return 	Panel->AttrCharPairGrid[(Panel->DrawPosY*Panel->Columns)+Panel->DrawPosX].Char;
}

void CONSOLE_MISC_PANEL_puts(PCONSOLE_MISC_PANEL Panel, int UseDrawTextColor, int UseDrawBackgroundColor, char* S)
{
	while(*S)
	{
		CONSOLE_MISC_PANEL_putc(Panel,UseDrawTextColor,UseDrawBackgroundColor,*S);

		S++;
	}
}

void CONSOLE_MISC_PANEL_gets(PCONSOLE_MISC_PANEL Panel, PUCHAR S, ULONG MaxLength, int CopyBeyondNullChar)
{
	while((MaxLength)&&((*S = CONSOLE_MISC_PANEL_getc(Panel))||(CopyBeyondNullChar)))
	{
		S++;
		MaxLength--;
	}
}

void CONSOLE_MISC_PANEL_printf(PCONSOLE_MISC_PANEL Panel, int UseDrawTextColor, int UseDrawBackgroundColor, char* FormatString, ...)
{
	int TempSize;
	va_list pTempArguments;
	char TempStringBuffer[1024];

	va_start(pTempArguments,FormatString);
	TempSize = vsnprintf(TempStringBuffer,sizeof(TempStringBuffer),FormatString,pTempArguments);
	va_end(pTempArguments);

	CONSOLE_MISC_PANEL_puts(Panel,UseDrawTextColor,UseDrawBackgroundColor,TempStringBuffer);
}

void CONSOLE_MISC_PANEL_Clear(PCONSOLE_MISC_PANEL Panel)
{
	int i;

	Panel->DrawPosX = 0;
	Panel->DrawPosY = 0;

	for(i = 0;i<(Panel->Columns*Panel->Rows);i++)
	{
		CONSOLE_MISC_PANEL_putc(Panel,TRUE,TRUE,0);
	}

	Panel->DrawPosX = 0;
	Panel->DrawPosY = 0;
}

void CONSOLE_MISC_PANEL_SetScrollLock(PCONSOLE_MISC_PANEL Panel, int ScrollLockEnabled)
{
	Panel->ScrollLockEnabled = ScrollLockEnabled;
}
