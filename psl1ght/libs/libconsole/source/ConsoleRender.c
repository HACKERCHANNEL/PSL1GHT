#include <console/ConsoleRender.h>

int CONSOLE_RENDER_Initialize(PCONSOLE_RENDER Render, UCHAR SlotWidth, UCHAR SlotHeight, ULONG CursorBlinkFrames, PULONG pBasicColors, PUCHAR pCharacterMasks, PUCHAR pCursorMask, PUCHAR pCursorInsertMask, PUCHAR pMousePointerMask)
{
	Render->CursorBlinkFrames = CursorBlinkFrames;
	Render->CurrentCursorBlinkFrames = 0;
	Render->CurrentCursorBlinkState = FALSE;
	Render->CursorMaskBuffer = pCursorMask;
	Render->CursorInsertMaskBuffer = pCursorInsertMask;
	Render->MousePointerMaskBuffer = pMousePointerMask;
	Render->CharacterWidth = SlotWidth;
	Render->CharacterHeight = SlotHeight;
	Render->CharacterMasksBuffer = pCharacterMasks;
	Render->BasicColors = pBasicColors;
	Render->RenderingFlags = 0;
	Render->DropShadowColor = 0x00000000;
	Render->DropShadowOrientation = CONSOLE_RENDER_DROPSHADOWORIENTATION_SOUTHEAST;
	Render->TransparentTextModeColor = 0;

	return TRUE;
}

int CONSOLE_RENDER_Finalize(PCONSOLE_RENDER Render)
{
	return TRUE;
}

int GetBitLeftToRight(PUCHAR pByteArray, UCHAR LeftToRightIndex)
{
	PUCHAR pTargetByte = pByteArray+(LeftToRightIndex/8);
	UCHAR TestMask = 0x80 >> (LeftToRightIndex%8);

	return ((*pTargetByte)&TestMask);
}

int GetBitFromLogicConsoleBitmap(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, ULONG CharCol, ULONG CharRow, UCHAR XOffsetIntoChar, UCHAR YOffsetIntoChar)
{
	UCHAR Char = 0;
	PUCHAR pCurrentCharMask;
	UCHAR BytesPerCharWidth;

	if(CONSOLE_LOGIC_GetCharXY(Console,CharCol,CharRow,&Char))
	{
		BytesPerCharWidth = (Render->CharacterWidth+7)/8;
		pCurrentCharMask = &(Render->CharacterMasksBuffer[Char*BytesPerCharWidth*Render->CharacterHeight]);
		pCurrentCharMask += BytesPerCharWidth*YOffsetIntoChar;

		return GetBitLeftToRight(pCurrentCharMask,XOffsetIntoChar);
	}
	else
	{
		return FALSE;
	}
}

void FromSlotAndOffsetToCombined(PCONSOLE_RENDER Render, ULONG SlotX, ULONG SlotY, UCHAR OffsetX, UCHAR OffsetY, PULONG pCombinedX, PULONG pCombinedY)
{
	*pCombinedX = (SlotX*Render->CharacterWidth)+OffsetX;
	*pCombinedY = (SlotY*Render->CharacterHeight)+OffsetY;
}

void FromCombinedToSlotAndOffset(PCONSOLE_RENDER Render, ULONG CombinedX, ULONG CombinedY, PULONG pSlotX, PULONG pSlotY, PUCHAR pOffsetX, PUCHAR pOffsetY)
{
	*pSlotX = CombinedX/Render->CharacterWidth;
	*pOffsetX = CombinedX%Render->CharacterWidth;
	*pSlotY = CombinedY/Render->CharacterHeight;
	*pOffsetY = CombinedY%Render->CharacterHeight;
}

int IsCharPixelDropShadow(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, ULONG CharCol, ULONG CharRow, UCHAR XOffsetIntoChar, UCHAR YOffsetIntoChar)
{
	ULONG CombinedX,CombinedY;

	FromSlotAndOffsetToCombined(Render,CharCol,CharRow,XOffsetIntoChar,YOffsetIntoChar,&CombinedX,&CombinedY);

	switch(Render->DropShadowOrientation)
	{
	case CONSOLE_RENDER_DROPSHADOWORIENTATION_NORTH:

		CombinedY++;

		break;

	case CONSOLE_RENDER_DROPSHADOWORIENTATION_NORTHEAST:

		CombinedY++;
		CombinedX--;

		break;

	case CONSOLE_RENDER_DROPSHADOWORIENTATION_EAST:

		CombinedX--;

		break;

	case CONSOLE_RENDER_DROPSHADOWORIENTATION_SOUTHEAST:

		CombinedX--;
		CombinedY--;

		break;

	case CONSOLE_RENDER_DROPSHADOWORIENTATION_SOUTH:

		CombinedY--;

		break;

	case CONSOLE_RENDER_DROPSHADOWORIENTATION_SOUTHWEST:

		CombinedX++;
		CombinedY--;

		break;

	case CONSOLE_RENDER_DROPSHADOWORIENTATION_WEST:

		CombinedX++;

		break;

	case CONSOLE_RENDER_DROPSHADOWORIENTATION_NORTHWEST:

		CombinedX++;
		CombinedY++;

		break;

	default:
		return FALSE;
	}

	FromCombinedToSlotAndOffset(Render,CombinedX,CombinedY,&CharCol,&CharRow,&XOffsetIntoChar,&YOffsetIntoChar);

	return GetBitFromLogicConsoleBitmap(Render,Console,CharCol,CharRow,XOffsetIntoChar,YOffsetIntoChar);
}

void HandleBackgroundPixelDraw(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video, ULONG X, ULONG Y, UCHAR i, UCHAR j, UCHAR BackAttr, int Masked)
{
	switch(Render->RenderingFlags)
	{
	case CONSOLE_RENDER_RENDERING_FLAG_TRANSPARENT|CONSOLE_RENDER_RENDERING_FLAG_DROPSHADOW|CONSOLE_RENDER_RENDERING_FLAG_DROPSHADOWOVERANYBACKGROUND:

		if(IsCharPixelDropShadow(Render,Console,X,Y,i,j))
		{
			CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->DropShadowColor);
		}
		else
		{
			if((BackAttr!=Render->TransparentTextModeColor)||(Masked))
			{
				CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[BackAttr]);
			}
		}

		break;

	case CONSOLE_RENDER_RENDERING_FLAG_TRANSPARENT|CONSOLE_RENDER_RENDERING_FLAG_DROPSHADOW:

		if(BackAttr==Render->TransparentTextModeColor)
		{
			if(IsCharPixelDropShadow(Render,Console,X,Y,i,j))
			{
				CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->DropShadowColor);
			}
		}
		else
		{
			CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[BackAttr]);
		}

		break;

	case CONSOLE_RENDER_RENDERING_FLAG_TRANSPARENT:

		if((BackAttr!=Render->TransparentTextModeColor)||(Masked))
		{
			CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[BackAttr]);
		}

		break;

	default:
		CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[BackAttr]);
	}
}

void DrawAttrCharPairXY(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video, int X, int Y, UCHAR Attr, UCHAR Char)
{
	PUCHAR pCurrentCharMask;
	UCHAR BytesPerCharWidth = (Render->CharacterWidth+7)/8;
	UCHAR i, j;
	UCHAR BackAttr, TextAttr;

	BackAttr = Attr>>4;
	TextAttr = Attr&0x0F;

	pCurrentCharMask = &(Render->CharacterMasksBuffer[Char*BytesPerCharWidth*Render->CharacterHeight]);

	for(j = 0;j<Render->CharacterHeight;j++)
	{
		for(i = 0;i<Render->CharacterWidth;i++)
		{
			if(GetBitLeftToRight(pCurrentCharMask, i))
			{
				CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[TextAttr]);
			}
			else
			{
				HandleBackgroundPixelDraw(Render,Console,Video,X,Y,i,j,BackAttr,FALSE);
			}
		}

		pCurrentCharMask += BytesPerCharWidth;
	}
}

void DrawAttrCharPairUnderCursorXY(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video, int X, int Y, UCHAR Attr, UCHAR Char, int CursorIsInsert)
{
	PUCHAR pCurrentCursorMask;
	PUCHAR pCurrentCharMask;
	UCHAR BytesPerCharWidth = (Render->CharacterWidth+7)/8;
	UCHAR i, j;
	UCHAR BackAttr, TextAttr;
	ULONG MaskedBackAttr, MaskedTextAttr;
	int Masked;

	BackAttr = Attr>>4;
	TextAttr = Attr&0x0F;

	if(CursorIsInsert)
	{
		pCurrentCursorMask = Render->CursorInsertMaskBuffer;
	}
	else
	{
		pCurrentCursorMask = Render->CursorMaskBuffer;
	}

	pCurrentCharMask = &(Render->CharacterMasksBuffer[Char*BytesPerCharWidth*Render->CharacterHeight]);

	for(j = 0;j<Render->CharacterHeight;j++)
	{
		for(i = 0;i<Render->CharacterWidth;i++)
		{
			Masked = GetBitLeftToRight(pCurrentCursorMask, i);

			if(Masked)
			{
				MaskedBackAttr = (15-BackAttr)&0x07;
				MaskedTextAttr = (15-TextAttr)&0x07;
			}
			else
			{
				MaskedBackAttr = BackAttr;
				MaskedTextAttr = TextAttr;
			}

			if(GetBitLeftToRight(pCurrentCharMask, i))
			{
				CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[MaskedTextAttr]);
			}
			else
			{
				HandleBackgroundPixelDraw(Render,Console,Video,X,Y,i,j,MaskedBackAttr,Masked);
			}
		}

		pCurrentCharMask += BytesPerCharWidth;
		pCurrentCursorMask += BytesPerCharWidth;
	}
}

void DrawAttrCharPairUnderMousePointerXY(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video, int X, int Y, UCHAR Attr, UCHAR Char)
{
	PUCHAR pMousePointerMask;
	PUCHAR pCurrentCharMask;
	UCHAR BytesPerCharWidth = (Render->CharacterWidth+7)/8;
	UCHAR i, j;
	UCHAR BackAttr, TextAttr;
	ULONG MaskedBackAttr, MaskedTextAttr;
	int Masked;

	BackAttr = Attr>>4;
	TextAttr = Attr&0x0F;

	pMousePointerMask = Render->MousePointerMaskBuffer;

	pCurrentCharMask = &(Render->CharacterMasksBuffer[Char*BytesPerCharWidth*Render->CharacterHeight]);

	for(j = 0;j<Render->CharacterHeight;j++)
	{
		for(i = 0;i<Render->CharacterWidth;i++)
		{
			Masked = GetBitLeftToRight(pMousePointerMask, i);

			if(Masked)
			{
				MaskedBackAttr = (15-BackAttr)&0x07;
				MaskedTextAttr = (15-TextAttr)&0x07;
			}
			else
			{
				MaskedBackAttr = BackAttr;
				MaskedTextAttr = TextAttr;
			}

			if(GetBitLeftToRight(pCurrentCharMask, i))
			{
				CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[MaskedTextAttr]);
			}
			else
			{
				HandleBackgroundPixelDraw(Render,Console,Video,X,Y,i,j,MaskedBackAttr,Masked);
			}
		}

		pCurrentCharMask += BytesPerCharWidth;
		pMousePointerMask += BytesPerCharWidth;
	}
}

void DrawAttrCharPairUnderCursorAndMousePointerXY(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video, int X, int Y, UCHAR Attr, UCHAR Char, int CursorIsInsert)
{
	PUCHAR pMousePointerMask;
	PUCHAR pCurrentCursorMask;
	PUCHAR pCurrentCharMask;
	UCHAR BytesPerCharWidth = (Render->CharacterWidth+7)/8;
	UCHAR i, j;
	UCHAR BackAttr, TextAttr;
	ULONG MaskedBackAttr, MaskedTextAttr;
	int Masked;

	BackAttr = Attr>>4;
	TextAttr = Attr&0x0F;

	if(CursorIsInsert)
	{
		pCurrentCursorMask = Render->CursorInsertMaskBuffer;
	}
	else
	{
		pCurrentCursorMask = Render->CursorMaskBuffer;
	}

	pMousePointerMask = Render->MousePointerMaskBuffer;

	pCurrentCharMask = &(Render->CharacterMasksBuffer[Char*BytesPerCharWidth*Render->CharacterHeight]);

	for(j = 0;j<Render->CharacterHeight;j++)
	{
		for(i = 0;i<Render->CharacterWidth;i++)
		{
			Masked = GetBitLeftToRight(pCurrentCursorMask, i)^GetBitLeftToRight(pMousePointerMask, i);

			if(Masked)
			{
				MaskedBackAttr = (15-BackAttr)&0x07;
				MaskedTextAttr = (15-TextAttr)&0x07;
			}
			else
			{
				MaskedBackAttr = BackAttr;
				MaskedTextAttr = TextAttr;
			}

			if(GetBitLeftToRight(pCurrentCharMask, i))
			{
				CONSOLE_VIDEO_PutPixel(Video, (X*Render->CharacterWidth)+i, (Y*Render->CharacterHeight)+j, Render->BasicColors[MaskedTextAttr]);
			}
			else
			{
				HandleBackgroundPixelDraw(Render,Console,Video,X,Y,i,j,MaskedBackAttr,Masked);
			}
		}

		pCurrentCharMask += BytesPerCharWidth;
		pMousePointerMask += BytesPerCharWidth;
		pCurrentCursorMask += BytesPerCharWidth;
	}
}

int CONSOLE_RENDER_RenderChanges(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video)
{
	ULONG ChangedSlotsSearchHandle = 0;
	ULONG X, Y, CX, CY, MPX, MPY;
	UCHAR Attr, Char;
	int MustDrawCursor;
	int MustDrawMousePointer;
	int BlinkStateChanged = FALSE;
	int CharIsUnderCursor, CharIsUnderMousePointer;
	int CharUnderCursorChanged = FALSE, CharUnderMousePointerChanged = FALSE;

	CONSOLE_LOGIC_GetCursorXY(Console, &CX, &CY);
	CONSOLE_LOGIC_GetMousePointerXY(Console, &MPX, &MPY);

	while(CONSOLE_LOGIC_GetNextChangedSlot(Console, &ChangedSlotsSearchHandle, &X, &Y))
	{
		CONSOLE_LOGIC_GetAttrCharPairXY(Console, X, Y, &Attr, &Char);

		if((X==CX)&&(Y==CY))
		{
			CharIsUnderCursor = TRUE;
			CharUnderCursorChanged = TRUE;
		}
		else
		{
			CharIsUnderCursor = FALSE;
		}

		if((X==MPX)&&(Y==MPY))
		{
			CharIsUnderMousePointer = TRUE;
			CharUnderMousePointerChanged = TRUE;
		}
		else
		{
			CharIsUnderMousePointer = FALSE;
		}

		if((!CharIsUnderCursor)&&(!CharIsUnderMousePointer))
		{
			DrawAttrCharPairXY(Render, Console, Video, X, Y, Attr, Char);
		}

	}

	if((Render->CurrentCursorBlinkFrames++)>Render->CursorBlinkFrames)
	{
		Render->CurrentCursorBlinkFrames = 0;

		Render->CurrentCursorBlinkState = !(Render->CurrentCursorBlinkState);

		BlinkStateChanged = TRUE;
	}

	MustDrawCursor = (Render->CurrentCursorBlinkState)&&(CONSOLE_LOGIC_IsCursorVisible(Console));
	MustDrawMousePointer = CONSOLE_LOGIC_IsMousePointerVisible(Console);

	if((CX==MPX)&&(CY==MPY))
	{
		if((CONSOLE_LOGIC_CursorChanged(Console))||(BlinkStateChanged)||(CONSOLE_LOGIC_MousePointerChanged(Console))||(CharUnderCursorChanged)||(CharUnderMousePointerChanged))
		{
			CONSOLE_LOGIC_GetAttrCharPairXY(Console, CX, CY, &Attr, &Char);

			if((MustDrawCursor)&&(MustDrawMousePointer))
			{
				DrawAttrCharPairUnderCursorAndMousePointerXY(Render, Console, Video, CX, CY, Attr, Char, CONSOLE_LOGIC_IsCursorInsert(Console));
			}
			else
			{
				if(MustDrawCursor)
				{
					DrawAttrCharPairUnderCursorXY(Render, Console, Video, CX, CY, Attr, Char, CONSOLE_LOGIC_IsCursorInsert(Console));
				}
				else
				{
					if(MustDrawMousePointer)
					{
						DrawAttrCharPairUnderMousePointerXY(Render, Console, Video, MPX, MPY, Attr, Char);
					}
					else
					{
						DrawAttrCharPairXY(Render, Console, Video, MPX, MPY, Attr, Char);
					}
				}
			}
		}
	}
	else
	{
		if((CONSOLE_LOGIC_CursorChanged(Console))||(BlinkStateChanged)||(CharUnderCursorChanged))
		{
			CONSOLE_LOGIC_GetAttrCharPairXY(Console, CX, CY, &Attr, &Char);

			if(MustDrawCursor)
			{
				DrawAttrCharPairUnderCursorXY(Render, Console, Video, CX, CY, Attr, Char, CONSOLE_LOGIC_IsCursorInsert(Console));
			}
			else
			{
				DrawAttrCharPairXY(Render, Console, Video, CX, CY, Attr, Char);
			}
		}

		if(CONSOLE_LOGIC_MousePointerChanged(Console)||(CharUnderMousePointerChanged))
		{
			CONSOLE_LOGIC_GetAttrCharPairXY(Console, MPX, MPY, &Attr, &Char);

			if(MustDrawMousePointer)
			{
				DrawAttrCharPairUnderMousePointerXY(Render, Console, Video, MPX, MPY, Attr, Char);
			}
			else
			{
				DrawAttrCharPairXY(Render, Console, Video, MPX, MPY, Attr, Char);
			}
		}
	}

	CONSOLE_LOGIC_ResetChanges(Console, FALSE);

	return TRUE;
}


int CONSOLE_RENDER_Render(PCONSOLE_RENDER Render, PCONSOLE_LOGIC Console, PCONSOLE_VIDEO Video)
{
	ULONG LinearTotalSlots = CONSOLE_LOGIC_GetColumns(Console)*CONSOLE_LOGIC_GetRows(Console);
	ULONG LinearCurrentSlot = 0;
	ULONG X, Y, CX, CY, MPX, MPY;
	UCHAR Attr, Char;
	int MustDrawCursor;
	int MustDrawMousePointer;
	int BlinkStateChanged = FALSE;
	int CharIsUnderCursor, CharIsUnderMousePointer;
	int CharUnderCursorChanged = FALSE, CharUnderMousePointerChanged = FALSE;

	CONSOLE_LOGIC_GetCursorXY(Console, &CX, &CY);
	CONSOLE_LOGIC_GetMousePointerXY(Console, &MPX, &MPY);

	while(LinearCurrentSlot<LinearTotalSlots)
	{
		X = LinearCurrentSlot%CONSOLE_LOGIC_GetColumns(Console);
		Y = LinearCurrentSlot/CONSOLE_LOGIC_GetColumns(Console);

		CONSOLE_LOGIC_GetAttrCharPairXY(Console, X, Y, &Attr, &Char);

		if((X==CX)&&(Y==CY))
		{
			CharIsUnderCursor = TRUE;
			CharUnderCursorChanged = TRUE;
		}
		else
		{
			CharIsUnderCursor = FALSE;
		}

		if((X==MPX)&&(Y==MPY))
		{
			CharIsUnderMousePointer = TRUE;
			CharUnderMousePointerChanged = TRUE;
		}
		else
		{
			CharIsUnderMousePointer = FALSE;
		}

		if((!CharIsUnderCursor)&&(!CharIsUnderMousePointer))
		{
			DrawAttrCharPairXY(Render, Console, Video, X, Y, Attr, Char);
		}

		LinearCurrentSlot++;
	}

	if((Render->CurrentCursorBlinkFrames++)>Render->CursorBlinkFrames)
	{
		Render->CurrentCursorBlinkFrames = 0;

		Render->CurrentCursorBlinkState = !(Render->CurrentCursorBlinkState);

		BlinkStateChanged = TRUE;
	}

	MustDrawCursor = (Render->CurrentCursorBlinkState)&&(CONSOLE_LOGIC_IsCursorVisible(Console));
	MustDrawMousePointer = CONSOLE_LOGIC_IsMousePointerVisible(Console);

	if((CX==MPX)&&(CY==MPY))
	{
		if((CONSOLE_LOGIC_CursorChanged(Console))||(BlinkStateChanged)||(CONSOLE_LOGIC_MousePointerChanged(Console))||(CharUnderCursorChanged)||(CharUnderMousePointerChanged))
		{
			CONSOLE_LOGIC_GetAttrCharPairXY(Console, CX, CY, &Attr, &Char);

			if((MustDrawCursor)&&(MustDrawMousePointer))
			{
				DrawAttrCharPairUnderCursorAndMousePointerXY(Render, Console, Video, CX, CY, Attr, Char, CONSOLE_LOGIC_IsCursorInsert(Console));
			}
			else
			{
				if(MustDrawCursor)
				{
					DrawAttrCharPairUnderCursorXY(Render, Console, Video, CX, CY, Attr, Char, CONSOLE_LOGIC_IsCursorInsert(Console));
				}
				else
				{
					if(MustDrawMousePointer)
					{
						DrawAttrCharPairUnderMousePointerXY(Render, Console, Video, MPX, MPY, Attr, Char);
					}
					else
					{
						DrawAttrCharPairXY(Render, Console, Video, MPX, MPY, Attr, Char);
					}
				}
			}
		}
	}
	else
	{
		if((CONSOLE_LOGIC_CursorChanged(Console))||(BlinkStateChanged)||(CharUnderCursorChanged))
		{
			CONSOLE_LOGIC_GetAttrCharPairXY(Console, CX, CY, &Attr, &Char);

			if(MustDrawCursor)
			{
				DrawAttrCharPairUnderCursorXY(Render, Console, Video, CX, CY, Attr, Char, CONSOLE_LOGIC_IsCursorInsert(Console));
			}
			else
			{
				DrawAttrCharPairXY(Render, Console, Video, CX, CY, Attr, Char);
			}
		}

		if(CONSOLE_LOGIC_MousePointerChanged(Console)||(CharUnderMousePointerChanged))
		{
			CONSOLE_LOGIC_GetAttrCharPairXY(Console, MPX, MPY, &Attr, &Char);

			if(MustDrawMousePointer)
			{
				DrawAttrCharPairUnderMousePointerXY(Render, Console, Video, MPX, MPY, Attr, Char);
			}
			else
			{
				DrawAttrCharPairXY(Render, Console, Video, MPX, MPY, Attr, Char);
			}
		}
	}

	return TRUE;
}

int CONSOLE_RENDER_SetDropShadowOrientation(PCONSOLE_RENDER Render, UCHAR DropShadowOrientation)
{
	if(DropShadowOrientation<CONSOLE_RENDER_DROPSHADOWORIENTATIONS)
	{
		Render->DropShadowOrientation = DropShadowOrientation;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_RENDER_SetDropShadowColor(PCONSOLE_RENDER Render, ULONG DropShadowColor)
{
	Render->DropShadowColor = DropShadowColor;

	return TRUE;
}

int CONSOLE_RENDER_SetTransparentTextModeColor(PCONSOLE_RENDER Render, UCHAR TransparentTextModeColor)
{
	if(TransparentTextModeColor<16)
	{
		Render->TransparentTextModeColor = TransparentTextModeColor;
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_RENDER_SetRenderingOptions(PCONSOLE_RENDER Render, ULONG RenderingFlags)
{
	Render->RenderingFlags = RenderingFlags;

	return TRUE;
}


int CONSOLE_RENDER_GetDropShadowOrientation(PCONSOLE_RENDER Render, PUCHAR pDropShadowOrientation)
{
	*pDropShadowOrientation = Render->DropShadowOrientation;

	return TRUE;
}

int CONSOLE_RENDER_GetDropShadowColor(PCONSOLE_RENDER Render, PULONG pDropShadowColor)
{
	*pDropShadowColor = Render->DropShadowColor;

	return TRUE;
}

int CONSOLE_RENDER_GetTransparentTextModeColor(PCONSOLE_RENDER Render, PUCHAR pTransparentTextModeColor)
{
	*pTransparentTextModeColor = Render->TransparentTextModeColor;

	return TRUE;
}

int CONSOLE_RENDER_GetRenderingOptions(PCONSOLE_RENDER Render, PULONG pRenderingFlags)
{
	*pRenderingFlags = Render->RenderingFlags;

	return TRUE;
}
