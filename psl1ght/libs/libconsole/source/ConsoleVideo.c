#include <console/ConsoleVideo.h>

int CONSOLE_VIDEO_Initialize(PCONSOLE_VIDEO Video, PUCHAR FrameBuffer, ULONG ResolutionX, ULONG ResolutionY, UCHAR ResolutionBPP, ULONG ResolutionScanLine, ULONG ViewPortX, ULONG ViewPortWidth, ULONG ViewPortY, ULONG ViewPortHeight)
{
//	PUCHAR BackBuffer;

//	BackBuffer = (PUCHAR)malloc(ViewPortWidth*ViewPortHeight*4);

//	if(!BackBuffer)
//	{
//		return FALSE;
//	}

	Video->FrameBuffer = FrameBuffer;
	Video->ResolutionX = ResolutionX;
	Video->ResolutionY = ResolutionY;
	Video->ResolutionBPP = ResolutionBPP; 
	Video->ScanLine = ResolutionScanLine;
	Video->ViewPortX = ViewPortX;
	Video->ViewPortWidth = ViewPortWidth;
	Video->ViewPortY = ViewPortY;
	Video->ViewPortHeight = ViewPortHeight;
//	Video->ViewPortBackBuffer = BackBuffer;
	Video->ViewPortStart = (PUCHAR)FrameBuffer+((ResolutionScanLine*ViewPortY)+(ViewPortX*ResolutionBPP));

	return TRUE;
}

int CONSOLE_VIDEO_Finalize(PCONSOLE_VIDEO Video)
{
//	free(Video->ViewPortBackBuffer);

//	Video->ViewPortBackBuffer = NULL;

	return TRUE;
}
/*
int CONSOLE_VIDEO_Save(PCONSOLE_VIDEO Video)
{
	ULONG i;
	PUCHAR Source;
	PUCHAR Destination;

	Source = Video->ViewPortStart;
	Destination = Video->ViewPortBackBuffer;

	for(i=0;i<(Video->ViewPortHeight);i++)
	{
		memcpy(Destination, Source, Video->ViewPortScanLine);
		Source += Video->ScanLine;
		Destination += Video->ViewPortScanLine;
	}

	return TRUE;
}

int CONSOLE_VIDEO_Restore(PCONSOLE_VIDEO Video)
{
	ULONG i;
	PUCHAR Source;
	PUCHAR Destination;

	Source = Video->ViewPortBackBuffer;
	Destination = Video->ViewPortStart;

	for(i=0;i<(Video->ViewPortHeight);i++)
	{
		memcpy(Destination, Source, Video->ViewPortScanLine);
		Source += Video->ViewPortScanLine;
		Destination += Video->ScanLine;
	}

	return TRUE;
}
*/
int CONSOLE_VIDEO_PutViewPortX(PCONSOLE_VIDEO Video, ULONG ViewPortX)
{
	ULONG Limit = (Video->ResolutionX)-(Video->ViewPortWidth);

	if(ViewPortX>Limit)
	{
		return FALSE;
	}
	else
	{
		Video->ViewPortX = ViewPortX;

		Video->ViewPortStart = (PUCHAR)(Video->FrameBuffer)+(((Video->ScanLine)*(Video->ViewPortY))+((Video->ViewPortX)*(Video->ResolutionBPP)));

		return TRUE;
	}
}

int CONSOLE_VIDEO_PutViewPortY(PCONSOLE_VIDEO Video, ULONG ViewPortY)
{
	ULONG Limit = (Video->ResolutionY)-(Video->ViewPortHeight);

	if(ViewPortY>Limit)
	{
		return FALSE;
	}
	else
	{
		Video->ViewPortY = ViewPortY;

		Video->ViewPortStart = (PUCHAR)(Video->FrameBuffer)+(((Video->ScanLine)*(Video->ViewPortY))+((Video->ViewPortX)*(Video->ResolutionBPP)));

		return TRUE;
	}
}

int CONSOLE_VIDEO_PutViewPortWidth(PCONSOLE_VIDEO Video, ULONG ViewPortWidth)
{
	if(((Video->ViewPortX)+ViewPortWidth)>(Video->ResolutionX))
	{
		return FALSE;
	}
	else
	{
		Video->ViewPortWidth = ViewPortWidth;

		Video->ViewPortScanLine = ViewPortWidth*(Video->ResolutionBPP);

		return TRUE;
	}
}

int CONSOLE_VIDEO_PutViewPortHeight(PCONSOLE_VIDEO Video, ULONG ViewPortHeight)
{
	if(((Video->ViewPortY)+ViewPortHeight)>(Video->ResolutionY))
	{
		return FALSE;
	}
	else
	{
		Video->ViewPortHeight = ViewPortHeight;

		return TRUE;
	}
}

int CONSOLE_VIDEO_GetViewPortX(PCONSOLE_VIDEO Video, PULONG pViewPortX)
{
	*pViewPortX = Video->ViewPortX;

	return TRUE;
}

int CONSOLE_VIDEO_GetViewPortY(PCONSOLE_VIDEO Video, PULONG pViewPortY)
{
	*pViewPortY = Video->ViewPortY;

	return TRUE;
}

int CONSOLE_VIDEO_GetViewPortWidth(PCONSOLE_VIDEO Video, PULONG pViewPortWidth)
{
	*pViewPortWidth = Video->ViewPortWidth;

	return TRUE;
}

int CONSOLE_VIDEO_GetViewPortHeight(PCONSOLE_VIDEO Video, PULONG pViewPortHeight)
{
	*pViewPortHeight = Video->ViewPortHeight;

	return TRUE;
}

int CONSOLE_VIDEO_PutPixel(PCONSOLE_VIDEO Video, ULONG X, ULONG Y, ULONG Color)
{
	PULONG FBAsDword;
	PUSHORT FBAsWord;
	PUCHAR FBAsByte;

	if((X<Video->ViewPortWidth)&&(Y<Video->ViewPortHeight))
	{
		switch(Video->ResolutionBPP)
		{
		case 4:
			
			FBAsDword = (PULONG)(Video->ViewPortStart+(Y*(Video->ScanLine))+(X*(Video->ResolutionBPP)));

			*FBAsDword = Color;

			break;

		case 2:

			FBAsWord = (PUSHORT)(Video->ViewPortStart+(Y*(Video->ScanLine))+(X*(Video->ResolutionBPP)));

			*FBAsWord = (USHORT)Color;

			break;

		case 1:

			FBAsByte = (PUCHAR)(Video->ViewPortStart+(Y*(Video->ScanLine))+(X*(Video->ResolutionBPP)));

			*FBAsByte = (UCHAR)Color;

			break;

		default:

			return FALSE;
		}
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
