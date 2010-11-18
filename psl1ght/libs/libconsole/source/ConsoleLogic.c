#include <stdlib.h>
#include <string.h>

#include <console/ConsoleLogic.h>

int CONSOLE_LOGIC_Initialize(PCONSOLE_LOGIC Console, ULONG Columns, ULONG Rows)
{
	PATTR_CHAR_PAIR GridBuffer;
	PUCHAR ChangedSlotsBuffer;

	GridBuffer = malloc(Columns*Rows*sizeof(ATTR_CHAR_PAIR));

	if(!GridBuffer)
	{
		return FALSE;
	}

	ChangedSlotsBuffer = malloc(Columns*Rows*sizeof(UCHAR));

	if(!ChangedSlotsBuffer)
	{
		free(GridBuffer);

		return FALSE;
	}

	memset(GridBuffer, 0, Columns*Rows*sizeof(ATTR_CHAR_PAIR));
	memset(ChangedSlotsBuffer, TRUE, Columns*Rows*sizeof(UCHAR));

	Console->CursorChanged = TRUE;
	Console->MousePointerChanged = TRUE;

	Console->Columns = Columns;
	Console->Rows = Rows;
	Console->CursorX = 0;
	Console->CursorY = 0;
	Console->CursorShow = TRUE;
	Console->MousePointerX = Columns/2;
	Console->MousePointerY = Rows/2;
	Console->MousePointerShow = TRUE;
	Console->AttrCharPairGrid = GridBuffer;
	Console->ChangedAttrCharPairs = ChangedSlotsBuffer;

	return TRUE;
}

int CONSOLE_LOGIC_Finalize(PCONSOLE_LOGIC Console)
{
	free(Console->AttrCharPairGrid);
	free(Console->ChangedAttrCharPairs);

	Console->AttrCharPairGrid = NULL;
	Console->ChangedAttrCharPairs = NULL;

	return TRUE;
}

ULONG CONSOLE_LOGIC_GetColumns(PCONSOLE_LOGIC Console)
{
	return Console->Columns;
}

ULONG CONSOLE_LOGIC_GetRows(PCONSOLE_LOGIC Console)
{
	return Console->Rows;
}

int CONSOLE_LOGIC_PutAttrCharPairXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, UCHAR Attr, UCHAR Char)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		if((Console->AttrCharPairGrid[LinearSlot].Attr!=Attr)||(Console->AttrCharPairGrid[LinearSlot].Char!=Char))
		{
			Console->AttrCharPairGrid[LinearSlot].Attr = Attr;
			Console->AttrCharPairGrid[LinearSlot].Char = Char;

			Console->ChangedAttrCharPairs[LinearSlot] = TRUE;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_GetAttrCharPairXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, PUCHAR pAttr, PUCHAR pChar)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		*pAttr = Console->AttrCharPairGrid[LinearSlot].Attr;
		*pChar = Console->AttrCharPairGrid[LinearSlot].Char;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_PutCharXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, UCHAR Char)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		if(Console->AttrCharPairGrid[LinearSlot].Char!=Char)
		{
			Console->AttrCharPairGrid[LinearSlot].Char = Char;

			Console->ChangedAttrCharPairs[LinearSlot] = TRUE;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_GetCharXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, PUCHAR pChar)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		*pChar = Console->AttrCharPairGrid[LinearSlot].Char;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_PutAttrXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, UCHAR Attr)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		if(Console->AttrCharPairGrid[LinearSlot].Attr!=Attr)
		{
			Console->AttrCharPairGrid[LinearSlot].Attr = Attr;

			Console->ChangedAttrCharPairs[LinearSlot] = TRUE;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_GetAttrXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, PUCHAR pAttr)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		*pAttr = Console->AttrCharPairGrid[LinearSlot].Attr;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_PutBackAttrXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, UCHAR BackAttr)
{
	ULONG LinearSlot;
	UCHAR TempAttr;

	if(BackAttr>=16)
	{
		return FALSE;
	}

	BackAttr <<= 4;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		TempAttr = Console->AttrCharPairGrid[LinearSlot].Attr;

		if((TempAttr&0xF0)!=BackAttr)
		{
			Console->AttrCharPairGrid[LinearSlot].Attr = (TempAttr&0x0F)|BackAttr;

			Console->ChangedAttrCharPairs[LinearSlot] = TRUE;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_GetBackAttrXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, PUCHAR pBackAttr)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		*pBackAttr = (Console->AttrCharPairGrid[LinearSlot].Attr)>>4;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_PutTextAttrXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, UCHAR TextAttr)
{
	ULONG LinearSlot;
	UCHAR TempAttr;

	if(TextAttr>=16)
	{
		return FALSE;
	}

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		TempAttr = Console->AttrCharPairGrid[LinearSlot].Attr;

		if((TempAttr&0x0F)!=TextAttr)
		{
			Console->AttrCharPairGrid[LinearSlot].Attr = (TempAttr&0xF0)|TextAttr;

			Console->ChangedAttrCharPairs[LinearSlot] = TRUE;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_GetTextAttrXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y, PUCHAR pTextAttr)
{
	ULONG LinearSlot;

	if((X<Console->Columns)&&(Y<Console->Rows))
	{
		LinearSlot = (Y*Console->Columns)+X;

		*pTextAttr = (Console->AttrCharPairGrid[LinearSlot].Attr)&0x0F;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CONSOLE_LOGIC_GetCursorXY(PCONSOLE_LOGIC Console, PULONG pX, PULONG pY)
{
	*pX = Console->CursorX;
	*pY = Console->CursorY;

	return TRUE;
}

int CONSOLE_LOGIC_ShowCursor(PCONSOLE_LOGIC Console, UCHAR Show)
{
	ULONG LinearSlot;

	LinearSlot = ((Console->CursorY)*Console->Columns)+(Console->CursorX);

	Console->ChangedAttrCharPairs[LinearSlot] = TRUE;

	Console->CursorShow = Show;

	Console->CursorChanged = TRUE;

	return TRUE;
}

int CONSOLE_LOGIC_IsCursorVisible(PCONSOLE_LOGIC Console)
{
	return Console->CursorShow;
}

int CONSOLE_LOGIC_PutCursorXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y)
{
	ULONG LinearSlot;

	if((X>=Console->Columns)||(Y>=Console->Rows))
	{
		return FALSE;
	}

	if((X==Console->CursorX)&&(Y==Console->CursorY))
	{
		return TRUE;
	}

	if(CONSOLE_LOGIC_IsCursorVisible(Console))
	{
		LinearSlot = ((Console->CursorY)*Console->Columns)+(Console->CursorX);

		Console->ChangedAttrCharPairs[LinearSlot] = TRUE;

		LinearSlot = (Y*Console->Columns)+X;

		Console->ChangedAttrCharPairs[LinearSlot] = TRUE;
	}

	Console->CursorChanged = TRUE;

	if((Console->MousePointerX==Console->CursorX)&&(Console->MousePointerY==Console->CursorY))
	{
		Console->MousePointerChanged = TRUE;
	}

	Console->CursorX = X;
	Console->CursorY = Y;

	return TRUE;
}

int CONSOLE_LOGIC_PutCursorInsert(PCONSOLE_LOGIC Console, UCHAR Insert)
{
	ULONG LinearSlot;

	LinearSlot = ((Console->CursorY)*Console->Columns)+(Console->CursorX);

	Console->ChangedAttrCharPairs[LinearSlot] = TRUE;

	Console->CursorInsert = Insert;

	Console->CursorChanged = TRUE;

	return TRUE;
}

int CONSOLE_LOGIC_IsCursorInsert(PCONSOLE_LOGIC Console)
{
	return Console->CursorInsert;
}

int CONSOLE_LOGIC_GetMousePointerXY(PCONSOLE_LOGIC Console, PULONG pX, PULONG pY)
{
	*pX = Console->MousePointerX;
	*pY = Console->MousePointerY;

	return TRUE;
}

int CONSOLE_LOGIC_ShowMousePointer(PCONSOLE_LOGIC Console, UCHAR Show)
{
	ULONG LinearSlot;

	LinearSlot = ((Console->MousePointerY)*Console->Columns)+(Console->MousePointerX);

	Console->ChangedAttrCharPairs[LinearSlot] = TRUE;

	Console->MousePointerShow = Show;

	Console->MousePointerChanged = TRUE;

	return TRUE;
}

int CONSOLE_LOGIC_IsMousePointerVisible(PCONSOLE_LOGIC Console)
{
	return Console->MousePointerShow;
}

int CONSOLE_LOGIC_PutMousePointerXY(PCONSOLE_LOGIC Console, ULONG X, ULONG Y)
{
	ULONG LinearSlot;

	if((X>=Console->Columns)||(Y>=Console->Rows))
	{
		return FALSE;
	}

	if((X==Console->MousePointerX)&&(Y==Console->MousePointerY))
	{
		return TRUE;
	}

	if(CONSOLE_LOGIC_IsMousePointerVisible(Console))
	{
		LinearSlot = ((Console->MousePointerY)*Console->Columns)+(Console->MousePointerX);

		Console->ChangedAttrCharPairs[LinearSlot] = TRUE;

		LinearSlot = (Y*Console->Columns)+X;

		Console->ChangedAttrCharPairs[LinearSlot] = TRUE;
	}

	Console->MousePointerChanged = TRUE;

	if((Console->MousePointerX==Console->CursorX)&&(Console->MousePointerY==Console->CursorY))
	{
		Console->CursorChanged = TRUE;
	}

	Console->MousePointerX = X;
	Console->MousePointerY = Y;

	return TRUE;
}

int CONSOLE_LOGIC_ResetChanges(PCONSOLE_LOGIC Console, UCHAR FalseTrue)
{
	ULONG Slots = Console->Columns*Console->Rows;

	memset(Console->ChangedAttrCharPairs, FalseTrue, Slots*sizeof(UCHAR));

	Console->CursorChanged = FalseTrue;
	Console->MousePointerChanged = FalseTrue;

	return TRUE;
}

int CONSOLE_LOGIC_GetNextChangedSlot(PCONSOLE_LOGIC Console, PULONG pHandle, PULONG pX, PULONG pY)
{
	ULONG Slots = Console->Columns*Console->Rows;

	if((*pHandle)>=Slots)
	{
		return FALSE;
	}

	for(;(*pHandle)<Slots;(*pHandle)++)
	{
		if(Console->ChangedAttrCharPairs[*pHandle])
		{
			*pX = (*pHandle)%Console->Columns;
			*pY = (*pHandle)/Console->Columns;

			(*pHandle)++;

			return TRUE;
		}
	}

	return FALSE;
}

int CONSOLE_LOGIC_CursorChanged(PCONSOLE_LOGIC Console)
{
	return Console->CursorChanged;
}

int CONSOLE_LOGIC_MousePointerChanged(PCONSOLE_LOGIC Console)
{
	return Console->MousePointerChanged;
}
