/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Initial Developer of this code is John Hansen.
 * Portions created by John Hansen are Copyright (C) 2009-2013 John Hansen.
 * All Rights Reserved.
 * 
 * ----------------------------------------------------------------------------
 *
 * \author Ahmand Fatoum & Simón Rodriguez Perez(Hochschule Aschaffenburg)
 * \date 2015-02-28
 * \version 2
 * \note printf function added
 */

#include "ev3_lcd.h"
#include "ev3_timer.h"
#include <stdarg.h>

#define DRAW_PIXELS_SET    0x00 //Basic options for pixel, line and shape drawing.
#define DRAW_PIXELS_CLEAR  0x01
#define DRAW_PIXELS_INVERT 0x02

#define DRAW_SHAPE_HOLLOW 0x00 //Extra options for shape drawing.
#define DRAW_SHAPE_FILLED 0x01

#define DRAW_BITMAP_PLAIN  0x00
#define DRAW_BITMAP_INVERT 0x01

#define DRAW_LOGICAL_COPY 0x00
#define DRAW_LOGICAL_AND  0x01
#define DRAW_LOGICAL_OR   0x02
#define DRAW_LOGICAL_XOR  0x03

#define LCD_MEM_WIDTH 60 // width of HW Buffer in bytes
#define LCD_BUFFER_LENGTH (LCD_MEM_WIDTH*LCD_HEIGHT)

#define ABS(a)    (((a)<0) ? -(a) : (a))
#define SGN(a)    (((a)<0) ? -1 : 1)
#define MAX(a,b)    (((a>b) ? (a) : (b)))
#define MIN(a,b)    (((a<b) ? (a) : (b)))

uint8_t hwBuffer[LCD_BUFFER_LENGTH];

uint8_t PixelTab[] = {
		0x00, // 000 00000000
		0xE0, // 001 11100000
		0x1C, // 010 00011100
		0xFC, // 011 11111100
		0x03, // 100 00000011
		0xE3, // 101 11100011
		0x1F, // 110 00011111
		0xFF  // 111 11111111
};

uint8_t UnPixelTab[] = {
// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
		0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 6, // 1
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D
		1, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 7  // F
};

typedef struct {
	bool Dirty;
	int DispFile;
	uint8_t *pFB0;
	uint8_t displayBuf[LCD_BUFFER_SIZE];
	uint8_t *font;
	uint8_t *pLcd;
	uint8_t currentFont;
	bool autoRefresh;
} LCDGlobals;

LCDGlobals LCDInstance;

int WriteToLcdDevice(char * bytes, int num_bytes)
{
	int result = -1;
	if (LCDInstance.DispFile >= 0)
	{
		// for some reason write is not returning num_bytes -
		// it usually returns zero
		result = write(LCDInstance.DispFile, bytes, num_bytes);
		if (result >= 0)
			return num_bytes;
	}
	return result;
}

typedef struct
{
	const char  *pFontBits;           // Pointer to start of font bitmap
	const short FontHeight;           // Character height (all inclusive)
	const short FontWidth;            // Character width (all inclusive)
	const short FontHorz;             // Number of horizontal character in font bitmap
	const char  FontFirst;            // First character supported
	const char  FontLast;             // Last character supported
} FONTINFO;

#include "normal_font.xbm"
#include "small_font.xbm"
#include "large_font.xbm"
#include "tiny_font.xbm"


FONTINFO FontInfo[] =
		{
				[FONTTYPE_NORMAL] = {
						.pFontBits    = (const char*)normal_font_bits,
						.FontHeight   = 9,
						.FontWidth    = 8,
						.FontHorz     = 16,
						.FontFirst    = 0x20,
						.FontLast     = 0x7F
				},
				[FONTTYPE_SMALL] =  {
						.pFontBits    = (const char*)small_font_bits,
						.FontHeight   = 8,
						.FontWidth    = 8,
						.FontHorz     = 16,
						.FontFirst    = 0x20,
						.FontLast     = 0x7F
				},
				[FONTTYPE_LARGE] =  {
						.pFontBits    = (const char*)large_font_bits,
						.FontHeight   = 16,
						.FontWidth    = 16,
						.FontHorz     = 16,
						.FontFirst    = 0x20,
						.FontLast     = 0x7F
				},
				[FONTTYPE_TINY] =   {
						.pFontBits    = (const char*)tiny_font_bits,
						.FontHeight   = 7,
						.FontWidth    = 5,
						.FontHorz     = 16,
						.FontFirst    = 0x20,
						.FontLast     = 0x7F
				},

		};

short dLcdGetFontWidth(uint8_t Font)
{
	return (FontInfo[Font].FontWidth);
}

short dLcdGetFontHeight(uint8_t Font)
{
	return (FontInfo[Font].FontHeight);
}

typedef   struct
{
	const unsigned char  *pIconBits;
	const short IconSize;
	const short IconHeight;
	const short IconWidth;
} ICONINFO;

#include "normal_icons.xbm"
#include "small_icons.xbm"
#include "large_icons.xbm"
#include "menu_icons.xbm"
#include "arrow_icons.xbm"


ICONINFO IconInfo[] =
		{
				[ICONTYPE_NORMAL] = {
						.pIconBits    = normal_icons_bits,
						.IconSize     = normal_icons_height,
						.IconHeight   = 12,
						.IconWidth    = normal_icons_width
				},
				[ICONTYPE_SMALL] =  {
						.pIconBits    = small_icons_bits,
						.IconSize     = small_icons_height,
						.IconHeight   = 8,
						.IconWidth    = small_icons_width
				},
				[ICONTYPE_LARGE] =  {
						.pIconBits    = large_icons_bits,
						.IconSize     = large_icons_height,
						.IconHeight   = 22,
						.IconWidth    = large_icons_width
				},
				[ICONTYPE_MENU] =   {
						.pIconBits    = menu_icons_bits,
						.IconSize     = menu_icons_height,
						.IconHeight   = 12,
						.IconWidth    = menu_icons_width
				},
				[ICONTYPE_ARROW] =  {
						.pIconBits    = arrow_icons_bits,
						.IconSize     = arrow_icons_height,
						.IconHeight   = 12,
						.IconWidth    = arrow_icons_width
				},
		};

uint8_t *dLcdGetIconBits(uint8_t Icon)
{
	return (uint8_t*)IconInfo[Icon].pIconBits;
}

short dLcdGetIconWidth(uint8_t Icon)
{
	return (IconInfo[Icon].IconWidth);
}

short dLcdGetIconHeight(uint8_t Icon)
{
	return (IconInfo[Icon].IconHeight);
}

short dLcdGetNumIcons(uint8_t Icon)
{
	return (IconInfo[Icon].IconSize / IconInfo[Icon].IconHeight);
}

void frameBufferToLcd(uint8_t* pSrc, uint8_t* pDst)
{
	unsigned long Pixels;
	unsigned short X;
	unsigned short Y;

	for (Y = 0; Y < LCD_HEIGHT; Y++)
	{
		for (X = 0; X < 7; X++)
		{
			// read 8 bytes (3 bits per uint8_t) into a 32-bit unsigned long from our source
			Pixels  = (unsigned long)UnPixelTab[*pSrc] << 0;  pSrc++;
			Pixels |= (unsigned long)UnPixelTab[*pSrc] << 3;  pSrc++;
			Pixels |= (unsigned long)UnPixelTab[*pSrc] << 6;  pSrc++;
			Pixels |= (unsigned long)UnPixelTab[*pSrc] << 9;  pSrc++;
			Pixels |= (unsigned long)UnPixelTab[*pSrc] << 12; pSrc++;
			Pixels |= (unsigned long)UnPixelTab[*pSrc] << 15; pSrc++;
			Pixels |= (unsigned long)UnPixelTab[*pSrc] << 18; pSrc++;
			Pixels |= (unsigned long)UnPixelTab[*pSrc] << 21; pSrc++;

			// now write the 24 bits to 3 8-bit bytes in our destination
			*pDst = (uint8_t)(Pixels >> 0);  pDst++;
			*pDst = (uint8_t)(Pixels >> 8);  pDst++;
			*pDst = (uint8_t)(Pixels >> 16); pDst++;
		}
		Pixels  = (unsigned long)UnPixelTab[*pSrc] << 0;  pSrc++;
		Pixels |= (unsigned long)UnPixelTab[*pSrc] << 3;  pSrc++;
		Pixels |= (unsigned long)UnPixelTab[*pSrc] << 6;  pSrc++;
		Pixels |= (unsigned long)UnPixelTab[*pSrc] << 9;  pSrc++;

		*pDst = (uint8_t)(Pixels >> 0);  pDst++;
		*pDst = (uint8_t)(Pixels >> 8);  pDst++;
	}
}

void lcdToFrameBuffer(uint8_t* pSrc, uint8_t* pDst)
{
	unsigned long Pixels;
	unsigned short X;
	unsigned short Y;

	for (Y = 0; Y < LCD_HEIGHT; Y++)
	{
		for (X = 0; X < 7; X++)
		{
			Pixels  =  (unsigned long)*pSrc << 0;  pSrc++;
			Pixels |=  (unsigned long)*pSrc << 8;  pSrc++;
			Pixels |=  (unsigned long)*pSrc << 16; pSrc++;

			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
			Pixels >>= 3;
			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
			Pixels >>= 3;
			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
			Pixels >>= 3;
			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
			Pixels >>= 3;
			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
			Pixels >>= 3;
			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
			Pixels >>= 3;
			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
			Pixels >>= 3;
			*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
		}
		Pixels  =  (unsigned long)*pSrc << 0; pSrc++;
		Pixels |=  (unsigned long)*pSrc << 8; pSrc++;

		*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
		Pixels >>= 3;
		*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
		Pixels >>= 3;
		*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
		Pixels >>= 3;
		*pDst   =  PixelTab[Pixels & 0x07]; pDst++;
	}
}

void doUpdateScreen()
{
	if (LCDInstance.Dirty && (LCDInstance.pFB0 != NULL) && (LCDInstance.pLcd != NULL))
	{
		lcdToFrameBuffer(LCDInstance.pLcd, hwBuffer);
		memmove((void*)LCDInstance.pFB0, (const void *)hwBuffer, LCD_BUFFER_LENGTH);
		LCDInstance.Dirty = false;
	}
}

void LcdUpdateHandler(int sig)
{
	if (LCDInstance.autoRefresh)
		doUpdateScreen();
}

void LcdCloseDevices()
{
	if (!LcdInitialized())
		return;

	if (LCDInstance.DispFile >= 0)
	{
		close(LCDInstance.DispFile);
		if (LCDInstance.pFB0 != NULL)
			munmap(LCDInstance.pFB0, LCD_BUFFER_LENGTH);
		LCDInstance.pFB0 = NULL;
		LCDInstance.DispFile = -1;
	}
}

bool LcdInit()
{
	uint8_t * pTmp;
	if (LcdInitialized()) return true;

	LCDInstance.autoRefresh = true;
	LCDInstance.Dirty = false;
	LCDInstance.pFB0 = NULL;
	LCDInstance.DispFile = -1;
	LCDInstance.font = NULL;
	LCDInstance.pLcd = NULL;
	LCDInstance.currentFont = FONTTYPE_NORMAL;

	LCDInstance.DispFile = open(LMS_LCD_DEVICE_NAME, O_RDWR);
	if (LCDInstance.DispFile != -1)
	{
		pTmp = (uint8_t*)mmap(NULL, LCD_BUFFER_LENGTH, PROT_READ + PROT_WRITE, MAP_SHARED, LCDInstance.DispFile, 0);
		if (pTmp == MAP_FAILED)
		{
			LcdCloseDevices();
			return false;
		}
		else
		{
			LCDInstance.pFB0 = pTmp;
//      LCDInstance.font := @(font_data[0]);
			LCDInstance.pLcd = LCDInstance.displayBuf;

#ifndef DISABLE_TIMERS

			// initialize timer system
			TimerInit(25);

			// register update handler with timer system
			SetTimerCallback(ti250ms, &LcdUpdateHandler);
#endif

			return true;
		}
	}

        return false;
}

bool LcdExit()
{
	// if not initialized then just exit
	if (!LcdInitialized())
		return true;

	LcdCloseDevices();

	LCDInstance.font = NULL;
	LCDInstance.pLcd = NULL;
	return true;
}

bool LcdInitialized()
{
	return (LCDInstance.DispFile != -1) &&
		   (LCDInstance.pFB0 != NULL);
}

void dLcdDrawPixel(uint8_t *pImage, char Color, short X0, short Y0)
{
	if ((X0 >= 0) && (X0 < LCD_WIDTH) && (Y0 >= 0) && (Y0 < LCD_HEIGHT))
	{
		if (Color)
		{
			pImage[(X0 >> 3) + Y0 * LCD_BYTE_WIDTH]  |=  (1 << (X0 % 8));
		}
		else
		{
			pImage[(X0 >> 3) + Y0 * LCD_BYTE_WIDTH]  &= ~(1 << (X0 % 8));
		}
	}
}

char dLcdReadPixel(uint8_t *pImage, short X0, short Y0)
{
	char Result = 0;
	if ((X0 >= 0) && (X0 < LCD_WIDTH) && (Y0 >= 0) && (Y0 < LCD_HEIGHT))
	{
		if ((pImage[(X0 >> 3) + Y0 * LCD_BYTE_WIDTH] & (1 << (X0 % 8))))
		{
			Result = 1;
		}
	}
	return (Result);
}

void dLcdDrawChar(uint8_t *pImage, char Color, short X0, short Y0, uint8_t Font, char Char)
{
	short CharWidth;
	short CharHeight;
	short CharByteIndex;
	short LcdByteIndex;
	uint8_t  CharByte;
	short Tmp,X,Y,TmpX,MaxX;
	uint8_t  bC1, bC2;

	CharWidth  = FontInfo[Font].FontWidth;
	CharHeight = FontInfo[Font].FontHeight;

	if ((Char >= FontInfo[Font].FontFirst) && (Char <= FontInfo[Font].FontLast))
	{
		Char -= FontInfo[Font].FontFirst;

		CharByteIndex  = (Char % FontInfo[Font].FontHorz) * ((CharWidth + 7) / 8);
		CharByteIndex += ((Char / FontInfo[Font].FontHorz) * ((CharWidth + 7) / 8) * CharHeight * FontInfo[Font].FontHorz);

		if (((CharWidth % 8) == 0) && ((X0 % 8) == 0))
		{
			// Font aligned
			X0           = (X0 >> 3) << 3;
			LcdByteIndex = (X0 >> 3) + Y0 * LCD_BYTE_WIDTH;
			while (CharHeight)
			{
				Tmp = 0;
				do
				{
					if (LcdByteIndex < LCD_BUFFER_SIZE)
					{
						if (Color)
							CharByte = FontInfo[Font].pFontBits[CharByteIndex + Tmp];
						else
							CharByte = ~FontInfo[Font].pFontBits[CharByteIndex + Tmp];
						pImage[LcdByteIndex + Tmp] = CharByte;
					}
					Tmp++;
				}
				while (Tmp < (CharWidth / 8));
				CharByteIndex += (CharWidth * FontInfo[Font].FontHorz) / 8;
				LcdByteIndex  += LCD_BYTE_WIDTH;
				CharHeight--;
			}
		}
		else
		{
			// Font not aligned
			MaxX = X0 + CharWidth;
			if (Color)
			{
				bC1 = 1;
				bC2 = 0;
			}
			else
			{
				bC1 = 0;
				bC2 = 1;
			}
			for (Y = 0;Y < CharHeight;Y++)
			{
				TmpX = X0;
				for (X = 0;X < ((CharWidth + 7) / 8);X++)
				{
					CharByte = FontInfo[Font].pFontBits[CharByteIndex + X];
					for (Tmp = 0;(Tmp < 8) && (TmpX < MaxX);Tmp++)
					{
						if (CharByte & 1)
						{
							dLcdDrawPixel(pImage,bC1,TmpX,Y0);
						}
						else
						{
							dLcdDrawPixel(pImage,bC2,TmpX,Y0);
						}
						CharByte >>= 1;
						TmpX++;
					}
				}
				Y0++;
				CharByteIndex += ((CharWidth + 7) / 8) * FontInfo[Font].FontHorz;
			}
		}
	}
}

void dLcdDrawText(uint8_t *pImage, char Color, short X0, short Y0, uint8_t Font, char *pText)
{
	while (*pText)
	{
		if (X0 < (LCD_WIDTH - FontInfo[Font].FontWidth))
		{
			dLcdDrawChar(pImage,Color,X0,Y0,Font,*pText);
			X0 += FontInfo[Font].FontWidth;
		}
		pText++;
	}
}

void dLcdDrawPicture(uint8_t *pImage,char Color,short X0,short Y0,short IconWidth,short IconHeight,uint8_t *pIconBits)
{
	short IconByteIndex;
	short LcdByteIndex;
	short Tmp;
	uint8_t  IconByte;

	IconByteIndex = 0;

	X0           = (X0 >> 3) << 3;
	LcdByteIndex = (X0 >> 3) + Y0 * LCD_BYTE_WIDTH;

	while (IconHeight)
	{
		for (Tmp = 0;Tmp < (IconWidth / 8);Tmp++)
		{
			if (Color)
				IconByte = pIconBits[IconByteIndex + Tmp];
			else
				IconByte = ~pIconBits[IconByteIndex + Tmp];

			pImage[LcdByteIndex + Tmp] = IconByte;
		}

		IconByteIndex += IconWidth / 8;
		LcdByteIndex  += LCD_BYTE_WIDTH;
		IconHeight--;
	}
}

void dLcdDrawIcon(uint8_t *pImage, char Color, short X0, short Y0, char Icon, uint8_t Num)
{
	short IconByteIndex;
	short IconHeight;
	short IconWidth;
	uint8_t  *pIconBits;

	IconHeight = dLcdGetIconHeight(Icon);
	IconWidth  = dLcdGetIconWidth(Icon);

	if (Num <= dLcdGetNumIcons(Icon))
	{
		pIconBits     = dLcdGetIconBits(Icon);
		IconByteIndex = ((short)Num * IconWidth * IconHeight) / 8;
		dLcdDrawPicture(pImage,Color,X0,Y0,IconWidth,IconHeight,&pIconBits[IconByteIndex]);
	}
}

void dLcdGetBitmapSize(IP pBitmap, short *pWidth, short *pHeight)
{
	*pWidth  = 0;
	*pHeight = 0;

	if (pBitmap)
	{
		*pWidth  = (short)pBitmap[0];
		*pHeight = (short)pBitmap[1];
	}
}

void dLcdDrawBitmap(uint8_t *pImage, char Color, short X0, short Y0, IP pBitmap)
{
	short  BitmapWidth;
	short  BitmapHeight;
	short  BitmapByteIndex;
	uint8_t   *pBitmapBytes;
	uint8_t   BitmapByte;
	short  Tmp,X,Y,TmpX,MaxX;
	short  LcdByteIndex;
	uint8_t   bC1, bC2;

	if (pBitmap)
	{
		BitmapWidth   =  (short)pBitmap[0];
		BitmapHeight  =  (short)pBitmap[1];
		MaxX          =  X0 + BitmapWidth;
		pBitmapBytes  =  &pBitmap[2];

		if ((BitmapWidth >=0) && (BitmapHeight >= 0))
		{
			if ((X0 % 8) || (BitmapWidth % 8))
			{ // X is not aligned
				BitmapWidth = ((BitmapWidth + 7) >> 3) << 3;
				if (Color)
				{
					bC1 = 1;
					bC2 = 0;
				}
				else
				{
					bC1 = 0;
					bC2 = 1;
				}
				for (Y = 0;Y < BitmapHeight;Y++)
				{
					BitmapByteIndex = (Y * BitmapWidth) / 8;
					TmpX            = X0;

					for (X = 0;X < (BitmapWidth / 8);X++)
					{
						BitmapByte = pBitmapBytes[BitmapByteIndex + X];

						for (Tmp = 0;(Tmp < 8) && (TmpX < MaxX);Tmp++)
						{
							if (BitmapByte & 1)
							{
								dLcdDrawPixel(pImage,bC1,TmpX,Y0);
							}
							else
							{
								dLcdDrawPixel(pImage,bC2,TmpX,Y0);
							}
							BitmapByte >>= 1;
							TmpX++;
						}
					}
					Y0++;
				}
			}
			else
			{ // X is uint8_t aligned
				BitmapByteIndex = 0;
				LcdByteIndex    = (X0 >> 3) + Y0 * LCD_BYTE_WIDTH;
				while (BitmapHeight)
				{
					X  =  X0;
					for (Tmp = 0;Tmp < (BitmapWidth / 8);Tmp++)
					{
						if (((LcdByteIndex + Tmp) < LCD_BUFFER_SIZE) && (X < LCD_WIDTH) && (X >= 0))
						{
							if (Color)
								BitmapByte = pBitmapBytes[BitmapByteIndex + Tmp];
							else
								BitmapByte = ~pBitmapBytes[BitmapByteIndex + Tmp];
							pImage[LcdByteIndex + Tmp] = BitmapByte;
						}
						X +=  8;
					}
					BitmapByteIndex += BitmapWidth / 8;
					LcdByteIndex    += LCD_BYTE_WIDTH;
					BitmapHeight--;
				}
			}
		}
	}
}

void LcdSetAutoRefresh(bool bOn)
{
	LCDInstance.autoRefresh = bOn;
}

bool LcdUpdate()
{
	if (!LcdInitialized())
		return false;

	doUpdateScreen();
	LCDInstance.Dirty = false;
	return true;
}

bool LcdClean()
{
	if (!LcdInitialized())
		return false;
	LCDInstance.currentFont = FONTTYPE_NORMAL;
	memset((void*)LCDInstance.pLcd, 0, LCD_BUFFER_SIZE);
	LCDInstance.Dirty = true;
	return true;
}
bool LcdScroll(short Y)
{
	if (LcdInitialized())
	{
		if ((Y > 0) && (Y < LCD_HEIGHT))
		{
			memmove(LCDInstance.pLcd,&(LCDInstance.pLcd[LCD_BYTE_WIDTH * Y]),(LCD_HEIGHT - Y) * LCD_BYTE_WIDTH);
			memset(&(LCDInstance.pLcd[(LCD_HEIGHT - Y) * LCD_BYTE_WIDTH]),0,LCD_BYTE_WIDTH * Y);
		}
		else if ((Y < 0) && (Y > -LCD_HEIGHT))
		{
			Y = ABS(Y);
			memmove(&(LCDInstance.pLcd[LCD_BYTE_WIDTH * Y]),LCDInstance.pLcd,(LCD_HEIGHT - Y) * LCD_BYTE_WIDTH);
			memset(LCDInstance.pLcd,0,LCD_BYTE_WIDTH * Y);
		}
		return true;
	}
	return false;
}

bool LcdSelectFont(uint8_t FontType)
{
	if (!LcdInitialized())
		return false;

	if (FontType >= NUM_FONTTYPES)
	{
		LCDInstance.currentFont = FONTTYPE_NORMAL;
	}
	else
	{
		LCDInstance.currentFont = FontType;
	}
	return true;
}

uint8_t* LcdGetDisplay()
{
	if (!LcdInitialized())
		return NULL;

	return LCDInstance.pLcd;
}

uint8_t* LcdGetFrameBuffer()
{
	if (!LcdInitialized())
		return NULL;

	// read from frame buffer into hwBuffer
	memmove((void *)hwBuffer, (const void*)LCDInstance.pFB0, LCD_BUFFER_LENGTH);
	// convert to display buffer
	frameBufferToLcd(hwBuffer, LCDInstance.pLcd);
	// return display buffer
	return LCDInstance.pLcd;
}

static const unsigned char reverse_bits_table[] = {
		0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

uint8_t reverse_bits(uint8_t b)
{
	return reverse_bits_table[b];
}


void _lcdWriteBytesToFile(ImageFormat fmt, uint8_t* data, char* filename, uint8_t width, uint8_t height)
{
	char fullname[128];
	char* fn_copy = strdup(filename);
	char *base, *path;
	path = dirname(fn_copy);
	base = basename(filename);

	uint8_t len = strlen(base);
	// check for and remove extension if it exists
	char* ext = strrchr(base, '.');
	if (ext) {
		len = ext-base;
	}
	// now build full path
	strcpy(fullname, path);
	strcat(fullname, "/");
	strncat(fullname, base, len);
	switch(fmt)
	{
		case ifP1:
		case ifP4:
			strcat(fullname, ".pbm");
			break;
		case ifBMP:
			strcat(fullname, ".bmp");
			break;
		case ifPNG:
			strcat(fullname, ".png");
			break;
		default:
			// default is XBM
			strcat(fullname, ".xbm");
			break;
	}

	FILE * pFile = NULL;
	pFile = fopen(fullname, "w");

	if (pFile != NULL)
	{
		int i, j, k, cnt, bw;
		bw = ((width + 7) / 8);
		switch (fmt)
		{
			case ifP1:
				fprintf(pFile, "P1\n%d %d\n", width, height);
				for (i = 0; i < height; i++)
				{
					uint8_t b;
					for (j = 0; j < bw-1; j++)
					{
						// get a uint8_t
						b = reverse_bits(data[i*bw+j]);
						// output 8 bits per uint8_t except for last uint8_t per line
						for (k = 7; k >= 0; k--)
						{
							fprintf(pFile, "%c ", (b & (1 << k)) ? '1' : '0');
						}
					}
					// handle last uint8_t per line
					b = reverse_bits(data[i*bw+(bw-1)]);
					// how many bits are extra?
					cnt = bw*8 - width; // e.g., 6 if width = 178
					for (k = 7; k >= cnt; k--)
					{
						fprintf(pFile, "%c ", (b & (1 << k)) ? '1' : '0');
					}
					fprintf(pFile, "\n");
				}
				break;
			case ifP4:
				fprintf(pFile, "P4\n%d %d\n", width, height);
				cnt = bw*height;
				for (i=0; i < cnt; i++)
				{
					uint8_t b = reverse_bits(data[i]);
					fwrite(&b, 1, 1, pFile);
				}
				break;
			case ifBMP:
			case ifPNG:
				printf("BMP and PNG image formats are not yet supported\n");
				break;
			default:
				// XBM format
				fprintf(pFile, "#define %s_width %d\n", base, width);
				fprintf(pFile, "#define %s_height %d\n", base, height);
				fprintf(pFile, "static char %s_bits[] = {\n", base);
				cnt = bw*height;
				for (i=0; i < cnt; i++)
				{
					fprintf(pFile, "0x%2.2X,", data[i]);
					if ((i+1) % 12 == 0)
						fprintf(pFile, "\n");
				}
				fprintf(pFile, " };\n");
				break;
		}
		fclose(pFile);
	}
}

void LcdWriteDisplayToFile(char* filename, ImageFormat fmt)
{
	if (!LcdInitialized())
		return;

	uint8_t * pSrc = LcdGetDisplay();
	_lcdWriteBytesToFile(fmt, pSrc, filename, 178, 128);
}

void LcdWriteFrameBufferToFile(char* filename, ImageFormat fmt)
{
	if (!LcdInitialized())
		return;

	uint8_t * pSrc = LcdGetFrameBuffer();
	_lcdWriteBytesToFile(fmt, pSrc, filename, 178, 128);
}

bool LcdText(char Color, short X, short Y, char* Text)
{
	if (!LcdInitialized())
		return false;

	dLcdDrawText(LCDInstance.pLcd, Color, X, Y, LCDInstance.currentFont, Text);
	LCDInstance.Dirty = true;
	return true;
}

bool LcdIcon(char Color, short X, short Y, char IconType, char IconNum)
{
	if (!LcdInitialized())
		return false;

	if ((signed char)IconNum < 0)
		return false;

	dLcdDrawIcon(LCDInstance.pLcd, Color, X, Y, IconType, IconNum);
	LCDInstance.Dirty = true;
	return true;
}

bool LcdBmpFile(char Color, short X, short Y, char* Name)
{
	if (LcdInitialized())
	{
		uint8_t pBmp[LCD_BUFFER_SIZE];
		int File = -1;
		File = open(Name, O_RDONLY);
		if (File >= 0)
		{
			read(File, pBmp, LCD_BUFFER_SIZE);
			close(File);
			dLcdDrawBitmap(LCDInstance.pLcd, Color, X, Y, (IP)pBmp);
			LCDInstance.Dirty = true;
			return true;
		}
	}
	return false;
}

bool LcdPicture(char Color, short X, short Y, IP pBitmap)
{
	if (!LcdInitialized())
		return false;

	dLcdDrawBitmap(LCDInstance.pLcd, Color, X, Y, pBitmap);
	LCDInstance.Dirty = true;
	return true;
}

bool LcdFillWindow(char Color, short Y, short Y1)
{
	if (!LcdInitialized())
		return false;

	short Y2, Y3, Tmp;

	LCDInstance.currentFont = FONTTYPE_NORMAL;

	if ((Y + Y1) < LCD_HEIGHT)
	{
		if ((Color == 0) || (Color == 1))
		{
			Y *= LCD_BYTE_WIDTH;

			if (Y1)
			{
				Y1 *= LCD_BYTE_WIDTH;
			}
			else
			{
				Y1 = LCD_BUFFER_SIZE - Y;
			}

			if (Color)
			{
				Color = -1;
			}
			memset(&(LCDInstance.pLcd[Y]), Color, Y1);
		}
		else
		{
			if (Y1 == 0)
			{
				Y1 = LCD_HEIGHT;
			}
			Y2 = LCD_BYTE_WIDTH;
			for (Tmp = Y;Tmp < Y1;Tmp++)
			{
				Y3 = Tmp * LCD_BYTE_WIDTH;
				memset(&(LCDInstance.pLcd[Y3]), Color, Y2);
				Color = ~Color;
			}
		}
	}
	LCDInstance.Dirty = true;
	return true;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************

#define TRANSLATE_Y(_Y) (LCD_HEIGHT - 1 - (_Y))

// OP codes supported by RIC files
enum {
	IMG_DESCRIPTION_ID = 0, // Ignored at this time
	IMG_SPRITE_ID = 1,
	IMG_VARMAP_ID = 2,
	IMG_COPYBITS_ID = 3,
	IMG_PIXEL_ID = 4,
	IMG_LINE_ID = 5,
	IMG_RECTANGLE_ID = 6,
	IMG_CIRCLE_ID = 7,
	IMG_NUMBOX_ID = 8,
	IMG_ELLIPSE_ID = 9,
	IMG_POLYGON_ID = 10
};

#define IMG_SYMB_USEARGS(_v) (_v & (SWORD)0xF000)
#define IMG_SYMB_MAP(_v)  ((_v & 0x0F00) >> 8)
#define IMG_SYMB_ARG(_v)  (_v & 0x00FF)

int DisplayLineHeight()
{
	// return the height of a character in the current font
	return dLcdGetFontHeight(LCDInstance.currentFont);
}

void DisplayEraseLine(uint8_t Line)
{
	int cnt = DisplayLineHeight()*LCD_BYTE_WIDTH;
	memset((void*)&(LCDInstance.pLcd[Line*cnt]), 0, cnt);
	LCDInstance.Dirty = true;
}

void DisplayErase()
{
	memset((void*)LCDInstance.pLcd, 0, LCD_BUFFER_SIZE);
	LCDInstance.Dirty = true;
}

void DisplaySetPixel(uint8_t X, uint8_t Y)
{
	if ((X < LCD_WIDTH) && (Y < LCD_HEIGHT))
	{
		LCDInstance.pLcd[(X / 8) + (LCD_BYTE_WIDTH * Y)] |= (1 << (X % 8));
		LCDInstance.Dirty = true;
	}
}

void DisplayClrPixel(uint8_t X, uint8_t Y)
{
	if ((X < LCD_WIDTH) && (Y < LCD_HEIGHT))
	{
		LCDInstance.pLcd[(X / 8) + (LCD_BYTE_WIDTH * Y)] &= ~(1 << (X % 8));
		LCDInstance.Dirty = true;
	}
}

void DisplayXorPixel(uint8_t X, uint8_t Y)
{
	if ((X < LCD_WIDTH) && (Y < LCD_HEIGHT))
	{
		LCDInstance.pLcd[(X / 8) + (LCD_BYTE_WIDTH * Y)] ^= (1 << (X % 8));
		LCDInstance.Dirty = true;
	}
}

uint8_t Masks[] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

void DisplayFillScreen(uint8_t StartX, uint8_t StartY, uint8_t PixelsX, uint8_t PixelsY, uint8_t PixelMode)
{
	uint8_t X1, Y1, X2, Y2, X, Y, M;
	uint8_t* pDst;

	X1 = StartX;
	Y1 = StartY;
	X2 = (uint8_t)((int)StartX + (int)PixelsX - 1);
	Y2 = (uint8_t)((int)StartY + (int)PixelsY - 1);

	if (X2 > (LCD_WIDTH-1))
		X2 = (LCD_WIDTH-1);
	if (Y2 > (LCD_HEIGHT-1))
		Y2 = (LCD_HEIGHT-1);

	X = (X1 / 8) * 8; // multiple of 8

	while (X <= X2)
	{
		M = 0xff;
		if (X < X1)
			M &= ~Masks[X1 % 8];
		if ((X2-X) < 8)
			M &= Masks[(X2 % 8) + 1];
		pDst = &(LCDInstance.pLcd[(X / 8) + (LCD_BYTE_WIDTH * Y1)]);
		switch (PixelMode)
		{
			case DRAW_PIXELS_INVERT:
				for (Y = Y1; Y <= Y2; Y++)
				{
					*pDst ^= M;
					pDst += LCD_BYTE_WIDTH;
				}
				break;
			case DRAW_PIXELS_CLEAR:
				for (Y = Y1; Y <= Y2; Y++)
				{
					*pDst &= ~M;
					pDst += LCD_BYTE_WIDTH;
				}
				break;
			default:
				for (Y = Y1; Y <= Y2; Y++)
				{
					*pDst |= M;
					pDst += LCD_BYTE_WIDTH;
				}
				break;
		}
		X += 8;
	}
	LCDInstance.Dirty = true;
}

void DisplayLineX(uint8_t X1, uint8_t X2, uint8_t Y, uint8_t PixelMode)
{
	uint8_t X, M, t;
	uint8_t* pDst;

	if (Y > LCD_HEIGHT) return;
	if (X1 > X2)
	{
		t = X1; X1 = X2; X2 = t;
	}
	if (X2 > (LCD_WIDTH-1))
		X2 = (LCD_WIDTH-1);

	// starting point of X is the uint8_t containing X1
	X = (X1 / 8) * 8;

	while (X <= X2)
	{
		M = 0xff;
		if (X < X1)
			M &= ~Masks[X1 % 8];
		if ((X2-X) < 8)
			M &= Masks[(X2 % 8) + 1];
		pDst = &(LCDInstance.pLcd[(X / 8) + (LCD_BYTE_WIDTH * Y)]);
		switch (PixelMode)
		{
			case DRAW_PIXELS_INVERT:
				*pDst ^= M;
				break;
			case DRAW_PIXELS_CLEAR:
				*pDst &= ~M;
				break;
			default:
				*pDst |= M;
				break;
		}
		X += 8;
	}
	LCDInstance.Dirty = true;
}

void DisplayLineY(uint8_t X, uint8_t Y1, uint8_t Y2, uint8_t PixelMode)
{
	uint8_t Y, M, t;
	uint8_t* pDst;

	if (X > LCD_WIDTH) return;
	if (Y1 > Y2)
	{
		t = Y1; Y1 = Y2; Y2 = t;
	}
	if (Y2 > (LCD_HEIGHT-1))
		Y2 = (LCD_HEIGHT-1);

	M = 1 << (X % 8);
	pDst = &(LCDInstance.pLcd[(X / 8) + (LCD_BYTE_WIDTH * Y1)]);
	for (Y = Y1; Y <= Y2; Y++)
	{
		switch (PixelMode)
		{
			case DRAW_PIXELS_INVERT:
				*pDst ^= M;
				break;
			case DRAW_PIXELS_CLEAR:
				*pDst &= ~M;
				break;
			default:
				*pDst |= M;
				break;
		}
		pDst += LCD_BYTE_WIDTH;
	}
	LCDInstance.Dirty = true;
}

void DisplayFrame(uint8_t StartX, uint8_t StartY, uint8_t PixelsX, uint8_t PixelsY, uint8_t PixelMode)
{
	DisplayLineX(StartX, StartX + PixelsX-1, StartY, PixelMode);
	if (PixelsY > 1)
	{
		DisplayLineY(StartX, StartY + 1, StartY + PixelsY - 1, PixelMode);
		DisplayLineY(StartX + PixelsX - 1, StartY + 1, StartY + PixelsY - 1, PixelMode);
	}
	LCDInstance.Dirty = true;
}

void DisplayDraw(uint8_t Cmd, uint8_t PixelMode, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2)
{
	if (!LcdInitialized())
		return;

	switch (Cmd)
	{
		case DISPLAY_ERASE_ALL :
			DisplayErase();
			break;
		case DISPLAY_PIXEL :
			switch (PixelMode)
			{
				case DRAW_PIXELS_INVERT:
					DisplayXorPixel(X1, Y1);
					break;
				case DRAW_PIXELS_CLEAR:
					DisplayClrPixel(X1, Y1);
					break;
				default:
					DisplaySetPixel(X1, Y1);
					break;
			}
			break;
		case DISPLAY_HORIZONTAL_LINE :
			DisplayLineX(X1,X2,Y1,PixelMode);
			break;
		case DISPLAY_VERTICAL_LINE :
			DisplayLineY(X1,Y1,Y2,PixelMode);
			break;
		case DISPLAY_CHAR :
//      DisplayChar(IOMapDisplay.pFont, PixelMode, X1, Y1, X2);
			break;
		case DISPLAY_ERASE_LINE :
			DisplayEraseLine(X1);
			break;
		case DISPLAY_FILL_REGION :
			DisplayFillScreen(X1, Y1, X2, Y2, PixelMode);
			break;
		case DISPLAY_FRAME :
			DisplayFrame(X1, Y1, X2, Y2, PixelMode);
			break;
	}
}

bool CmdResolveDrawingMode(unsigned short DrawingOptions, uint8_t* pPixelMode, uint8_t* pFillMode)
{
	// Extract shape fill option:
	if (DrawingOptions & DRAW_OPT_FILL_SHAPE)
		*pFillMode = DRAW_SHAPE_FILLED;
	else
		*pFillMode = DRAW_SHAPE_HOLLOW;

	// Extract pixel drawing options:
	if (DrawingOptions & DRAW_OPT_INVERT)
	{
		//Drawing with white pixels:
		switch (DrawingOptions & DRAW_OPT_LOGICAL_OPERATIONS)
		{
			//Only these cases do anything:
			case DRAW_OPT_LOGICAL_COPY:
			case DRAW_OPT_LOGICAL_AND:
				*pPixelMode = DRAW_PIXELS_CLEAR;
				return true;
		}
	}
	else
	{
		//Drawing with black pixels:
		switch (DrawingOptions & DRAW_OPT_LOGICAL_OPERATIONS)
		{
			//Only these cases do anything:
			case DRAW_OPT_LOGICAL_COPY:
			case DRAW_OPT_LOGICAL_OR:
				*pPixelMode = DRAW_PIXELS_SET;
				return true;
			case DRAW_OPT_LOGICAL_XOR:
				*pPixelMode = DRAW_PIXELS_INVERT;
				return true;
		}
	}
	// If no operation is required, set defaults and return false.
	// e.g. 'AND' on its own is meaningless for line drawing,
	// 'INVERT + OR' and 'INVERT + XOR'  do nothing either.
	*pPixelMode = DRAW_PIXELS_SET;
	*pFillMode  = DRAW_SHAPE_HOLLOW;
	return false;
}

void CmdSetPixel(int X, int Y, uint8_t PixelMode)
{
	Y = TRANSLATE_Y(Y);
	if ((X >= 0) && (X < LCD_WIDTH) && (Y >= 0) && (Y < LCD_HEIGHT))
		DisplayDraw(DISPLAY_PIXEL, PixelMode, (uint8_t)X, (uint8_t)Y, 0, 0);
}

void CmdDrawLine(int x1, int y1, int x2, int y2, uint8_t PixelMode)
{
	int tx, ty, dx, dy;
	int d, x, y, ax, ay, sx, sy;

	dx = x2-x1;
	dy = y2-y1;

	//Clip line ends vertically - easier if y1<y2:
	if (y1 > y2)
	{
		tx = x1; x1 = x2; x2 = tx;
		ty = y1; y1 = y2; y2 = ty;
	}

	//Is line completely off screen?
	if ((y2 < 0) || (y1 >= LCD_HEIGHT)) return;

	//Trim y1 end:
	if (y1 < 0)
	{
		if ((dx != 0) && (dy != 0))
			x1 = x1 + (((0-y1)*dx) / dy);
		y1 = 0;
	}
	//Trim y2 end:
	if (y2 > LCD_HEIGHT-1)
	{
		if ((dx != 0) && (dy != 0))
			x2 = x2 - (((y2-(LCD_HEIGHT-1))*dx) / dy);
		y2 = LCD_HEIGHT-1;
	}

	//Clip horizontally - easier if x1<x2
	if (x1 > x2)
	{
		tx = x1; x1 = x2; x2 = tx;
		ty = y1; y1 = y2; y2 = ty;
	}

	//Is line completely off screen?
	if ((x2 < 0) || (x1 >= LCD_WIDTH)) return;

	//Trim x1 end:
	if (x1 < 0)
	{
		if ((dx != 0) && (dy != 0))
			y1 = y1 + (((0-x1)*dy) / dx);
		x1 = 0;
	}
	//Trim x2 end:
	if (x2 > LCD_WIDTH-1)
	{
		if ((dx != 0) && (dy != 0))
			y2 = y2 - (((x2-(LCD_WIDTH-1))*dy) / dx);
		x2 = LCD_WIDTH-1;
	}

	if (x1 == x2)
	{
		// vertical line or a single point
		if (y1 == y2)
		{
			DisplayDraw(DISPLAY_PIXEL, PixelMode, (uint8_t)x1, TRANSLATE_Y(y1), 0, 0);   //JJR
		}
		else
		{
			DisplayDraw(DISPLAY_VERTICAL_LINE, PixelMode, x1, TRANSLATE_Y(y1), 0, TRANSLATE_Y(y2));
		}
	}
	else if (y1 == y2)
	{
		// horizontal line (single point already dealt with)
		DisplayDraw(DISPLAY_HORIZONTAL_LINE, PixelMode, x1, TRANSLATE_Y(y1), x2, 0);   //JJR
	}
	else
	{
		// Initialize variables
		dx = x2-x1;
		ax = ABS(dx) << 1;
		sx = SGN(dx);
		dy = y2-y1;
		ay = ABS(dy) << 1;
		sy = SGN(dy);
		x  = x1;
		y  = y1;
		if (ax > ay)
		{    // x dominant
			d = ay - (ax >> 1);
			while (true)
			{
				DisplayDraw(DISPLAY_PIXEL, PixelMode, (uint8_t)x, TRANSLATE_Y(y), 0, 0);   //JJR
				if (x == x2) return;
				if (d >= 0)
				{
					y += sy;
					d -= ax;
				}
				x += sx;
				d += ay;
			}
		}
		else
		{      // y dominant
			d = ax - (ay >> 1);
			while (true)
			{
				DisplayDraw(DISPLAY_PIXEL, PixelMode, (uint8_t)x, TRANSLATE_Y(y), 0, 0);   //JJR
				if (y == y2) return;
				if (d >= 0)
				{
					x += sx;
					d -= ay;
				}
				y += sy;
				d += ax;
			}
		}
	}
}

void CmdDrawRect(int left, int bottom, int width, int height, uint8_t PixelMode, uint8_t FillMode)
{
	int x1 = left;
	int x2 = left + width;
	int y1 = bottom;
	int y2 = bottom + height;
	int t;

	if (x1 > x2)
	{
		t = x1; x1 = x2; x2 = t;
	}
	if (y1 > y2)
	{
		t = y1; y1 = y2; y2 = t;
	}

	if ((y2 == y1) || (x2 == x1))
	{
		// height == 0 so draw a single pixel horizontal line OR
		// width == 0 so draw a single pixel vertical line
		CmdDrawLine(x1, y1, x2, y2, PixelMode);
		return;
	}
	// rectangle has abs(width) or abs(height) >= 1
	if (FillMode == DRAW_SHAPE_FILLED)
	{
		if ((x2 < 0) || (y2 < 0) || (x1 > LCD_WIDTH-1) || (y1 > LCD_HEIGHT-1))
			return;
		if (x1 < 0)
			x1 = 0;
		if (y1 < 0)
			y1 = 0;
		if (x2 > LCD_WIDTH-1)
			x2 = LCD_WIDTH-1;
		if (y2 > LCD_HEIGHT-1)
			y2 = LCD_HEIGHT-1;
		DisplayDraw(DISPLAY_FILL_REGION, PixelMode, x1, TRANSLATE_Y(y2), x2-x1+1, y2-y1+1);
	}
	else
	{
		//Use the full line drawing functions rather than horizontal/vertical
		//functions so these get clipped properly.  These will fall straight
		//through to the faster functions anyway.
		//Also don't re-draw parts of slim rectangles since XOR might be on.

		CmdDrawLine(x1, y1, x2, y1, PixelMode);
		if (y2 > y1)
		{
			CmdDrawLine(x1, y2, x2, y2, PixelMode);
			if (y2 > y1+1)
			{
				CmdDrawLine(x2, y1+1, x2, y2-1, PixelMode);
				if (x2 > x1)
					CmdDrawLine(x1, y1+1, x1, y2-1, PixelMode);
			}
		}
	}
}

void CmdDrawEllipse(short xc, short yc, short a, short b, uint8_t PixelMode, uint8_t FillMode)
{
//			(* e(x,y) = b^2*x^2 + a^2*y^2 - a^2*b^2 *)
	short x  = 0;
	short y  = b;
	short rx = x;
	short ry = y;
	unsigned short width = 1;
	unsigned short height = 1;
	int a2 = (int)a*(int)a;
	int b2 = (int)b*(int)b;
	int crit1 = -((a2 / 4) + (a % 2) + b2);
	int crit2 = -((b2 / 4) + (b % 2) + a2);
	int crit3 = -((b2 / 4) + (b % 2));
	int t = -a2*y; //(* e(x+1/2,y-1/2) - (a^2+b^2)/4 *)
	int dxt  = 2*b2*x;
	int dyt  = -2*a2*y;
	int d2xt = 2*b2;
	int d2yt = 2*a2;

	if (b == 0)
	{
		CmdDrawRect(xc-a, yc, 2*a, 0, PixelMode, FillMode);
		return;
	}
	if (a == 0)
	{
		CmdDrawRect(xc, yc-b, 0, 2*b, PixelMode, FillMode);
		return;
	}

	while ((y >= 0) && (x <= a))
	{
		if (FillMode != DRAW_SHAPE_FILLED)
		{
			CmdSetPixel(xc+x, yc+y, PixelMode);
			if ((x != 0) || (y != 0))
				CmdSetPixel(xc-x, yc-y, PixelMode);
			if ((x != 0) && (y != 0))
			{
				CmdSetPixel(xc+x, yc-y, PixelMode);
				CmdSetPixel(xc-x, yc+y, PixelMode);
			}
		}
		if ((t + b2*x <= crit1) ||     //(* e(x+1,y-1/2) <= 0 *)
			(t + a2*y <= crit3))       //(* e(x+1/2,y) <= 0 *)
		{
			if (FillMode == DRAW_SHAPE_FILLED)
			{
				if (height == 1)
				{
					; //(* draw nothing *)
				}
				else if (ry*2+1 > (height-1)*2)
				{
					CmdDrawRect(xc-rx, yc-ry, width-1, height-1, PixelMode, FillMode);
					CmdDrawRect(xc-rx, yc+ry, width-1, -(height-1), PixelMode, FillMode);
					ry -= (height-1);
					height = 1;
				}
				else
				{
					CmdDrawRect(xc-rx, yc-ry, width-1, ry*2, PixelMode, FillMode);
					ry -= ry;
					height = 1;
				}
				rx++;
				width += 2;
			}
			x++;
			dxt += d2xt;
			t   += dxt;
		}
		else if (t - a2*y > crit2) //(* e(x+1/2,y-1) > 0 *)
		{
			y--;
			dyt += d2yt;
			t   += dyt;
			if (FillMode == DRAW_SHAPE_FILLED)
				height++;
		}
		else
		{
			if (FillMode == DRAW_SHAPE_FILLED)
			{
				if (ry*2+1 > height*2)
				{
					CmdDrawRect(xc-rx, yc-ry, width-1, height-1, PixelMode, FillMode);
					CmdDrawRect(xc-rx, yc+ry, width-1, -(height-1), PixelMode, FillMode);
				}
				else
				{
					CmdDrawRect(xc-rx, yc-ry, width-1, ry*2, PixelMode, FillMode);
				}
				width += 2;
				ry -= height;
				height = 1;
				rx++;
			}
			x++;
			dxt += d2xt;
			t   += dxt;
			y--;
			dyt += d2yt;
			t   += dyt;
		}
	}
	if (FillMode == DRAW_SHAPE_FILLED)
	{
		if (ry > height)
		{
			CmdDrawRect(xc-rx, yc-ry, width-1, height-1, PixelMode, FillMode);
			CmdDrawRect(xc-rx, yc+ry, width-1, -(height-1), PixelMode, FillMode);
		}
		else
		{
			CmdDrawRect(xc-rx, yc-ry, width-1, ry*2, PixelMode, FillMode);
		}
	}
}

void CmdDrawCircle(int cx, int cy, int radius, uint8_t PixelMode, uint8_t FillMode)
{
	CmdDrawEllipse(cx, cy, radius, radius, PixelMode, FillMode);
}

char CircleOutEx(int x, int y, uint8_t radius, unsigned long options)
{
	if (!LcdInitialized())
		return 1;
	uint8_t pixelMode, fillMode;
	if (CmdResolveDrawingMode(options, &pixelMode, &fillMode))
		CmdDrawCircle(x, y, radius, pixelMode, fillMode);
	return 0;
}

char LineOutEx(int x1, int y1, int x2, int y2, unsigned long options)
{
	if (!LcdInitialized())
		return 1;
	uint8_t pixelMode, fillMode;
	if (CmdResolveDrawingMode(options, &pixelMode, &fillMode))
		CmdDrawLine(x1, y1, x2, y2, pixelMode);
	return 0;
}

char PointOutEx(int x, int y, unsigned long options)
{
	if (!LcdInitialized())
		return 1;
	uint8_t pixelMode, fillMode;
	if (CmdResolveDrawingMode(options, &pixelMode, &fillMode))
		CmdSetPixel(x, y, pixelMode);
	return 0;
}

char RectOutEx(int x, int y, int width, int height, unsigned long options)
{
	if (!LcdInitialized())
		return 1;
	uint8_t pixelMode, fillMode;
	if (CmdResolveDrawingMode(options, &pixelMode, &fillMode))
		CmdDrawRect(x, y, width, height, pixelMode, fillMode);
	return 0;
}

char EllipseOutEx(int x, int y, uint8_t radiusX, uint8_t radiusY, unsigned long options)
{
	if (!LcdInitialized())
		return 1;
	uint8_t pixelMode, fillMode;
	if (CmdResolveDrawingMode(options, &pixelMode, &fillMode))
		CmdDrawEllipse(x, y, radiusX, radiusY, pixelMode, fillMode);
	return 0;
}


/*******************
 * Redundant functions
 */
#if 1

// Same as LcdUpdate
void LcdRefresh()
{
	if (!LcdInitialized())
		return;
	doUpdateScreen();
}

#endif

/********************************************************************************************/
/**
* Print Text with Variables 
* author: Ahmad Fatoum
*
*
*/

static short CURSOR_X = 0;
static short CURSOR_Y = 0;

#ifdef __GNUC__
#define ___FORMAT(x,y) __attribute__ ((format (printf, x, y)))
#define ___NONNULL(...) __attribute__((nonnull (##__VA_ARGS__)))
#else
#define ___FORMAT(x,y) 
#endif

int vasprintf(char **buf, const char *fmt, va_list _va)
{
	va_list va;
	va_copy(va, _va);
	int len = vsnprintf(NULL, 0, fmt, va);
	va_end(va);

	if (len < 0) return len;

	*buf = (char*)malloc(len + 1);
	if (!*buf)
		return -1; /* out of memory */

	len = vsprintf(*buf, fmt, _va);

	return len;
}

int asprintf(char **buf, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int ret = vasprintf(buf, fmt, va);
	va_end(va);
	return ret;
}

bool LcdTextf(char Color, short X, short Y, const char *fmt, ...)
{
	int ret;
	(void) ret; // do something with this!
	va_list ap;
	char *buf;

	va_start(ap, fmt);
	ret = vasprintf(&buf, fmt, ap);
	va_end(ap);

	return LcdText(Color, X, Y, buf);
}

#define TAB_SIZE 2
int LcdPrintf(char color, const char *fmt, ...)
{
	static short X0 = 0;
	static short Y0 = 0;
	static short indent = 0;
    
    X0 = CURSOR_X;
    Y0 = CURSOR_Y;
    
	if (!LcdInitialized())
		return -1;

	int ret;
	(void) ret; // do something with this!
	va_list ap;
	char *buf;

	va_start(ap, fmt);
	ret = vasprintf(&buf, fmt, ap);
	va_end(ap);

	const char *c = buf;
	const short width = FontInfo[LCDInstance.currentFont].FontWidth;
	const short height = FontInfo[LCDInstance.currentFont].FontHeight;
	while (*c)
	{
		if (indent == TAB_SIZE) indent = 0;
		if (! (0 <= X0 && X0 < LCD_WIDTH - width) )
		{
			Y0 += height + 2;
			X0 = 0;
		}
		switch (*c)
		{
			case '\n':
				Y0 += height + 2;
				/* fall through */
			case '\r':
				X0 = 0;
				break;
			case '\b':
				X0 -= width;
				break;
			case '\v':
				Y0 += height + 2;
				break;
			case '\f':
				X0 = Y0 = 0;
				break;
			case '\t':
				X0 += width * (TAB_SIZE - indent);
				break;
			default:
				indent++;
				dLcdDrawChar(LCDInstance.pLcd, color, X0, Y0, LCDInstance.currentFont, *c);
				X0 += width;
		}
		indent++;
		c++;
	}
	LCDInstance.Dirty = true;

    CURSOR_X = X0;
    CURSOR_Y = Y0;
    return c - buf;
}

int Ev3Printf(const char *fmt, ...)
{
	va_list args;
	char *buffer;

	va_start(args, fmt);
	vasprintf(&buffer, fmt, args);
	va_end(args);

	int ret = LcdPrintf(1, buffer);	// LcdPrintf with color=1 for black text

	return ret;
}

int Ev3Println(const char *fmt, ...)
{
	va_list args;
	char *buffer2;

	va_start(args, fmt);
	vasprintf(&buffer2, fmt, args);
	va_end(args);
	
	return Ev3Printf("%s\n", buffer2);
}

int TermPrintf(const char *fmt, ...)
{
    static short X0 = 0;
	static short Y0 = 0;
	static short indent = 0;
    
    X0 = CURSOR_X;
    Y0 = CURSOR_Y;
    
	if (!LcdInitialized())
		return -1;

	int ret;
	(void) ret; // do something with this!
	va_list ap;
	char *buf;

	va_start(ap, fmt);
	ret = vasprintf(&buf, fmt, ap);
	va_end(ap);

	const char *c = buf;
	const short width = FontInfo[LCDInstance.currentFont].FontWidth;
	const short height = FontInfo[LCDInstance.currentFont].FontHeight;
	while (*c)
	{
		if (indent == TAB_SIZE) indent = 0;
		if (! (0 <= X0 && X0 < LCD_WIDTH - width) )
		{
			Y0 += height + 2;
			X0 = 0;
		}
		if (Y0 + height + 2 > LCD_HEIGHT) {
            Y0 -= height + 2;
            LcdScroll(height + 2);
        }
		switch (*c)
		{
			case '\n':
				Y0 += height + 2;
				/* fallthrough */
			case '\r':
				X0 = 0;
				break;
			case '\b':
				X0 -= width;
				break;
			case '\v':
				Y0 += height + 2;
				break;
			case '\f':
				X0 = Y0 = 0;
				break;
			case '\t':
				X0 += width * (TAB_SIZE - indent);
				break;
			default:
				indent++;
				dLcdDrawChar(LCDInstance.pLcd, 1, X0, Y0, LCDInstance.currentFont, *c);
				X0 += width;
		}
		indent++;
		c++;
	}
	LCDInstance.Dirty = true;

    CURSOR_X = X0;
    CURSOR_Y = Y0;
    
    return c - buf;
}

int TermPrintln(const char *fmt, ...)
{
    va_list args;
	char *buffer2;

	va_start(args, fmt);
	vasprintf(&buffer2, fmt, args);
	va_end(args);
	
	return TermPrintf("%s\n", buffer2);
}

void Ev3Clear()
{
	LcdClean();
	LcdPrintf(1, "%s", "\f");
}
