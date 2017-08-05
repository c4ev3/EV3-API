/** \file ev3_lcd.h
 * \brief Functions and constants for controlling EV3 LCD
 *
 * ev3_lcd.h contains declarations for the EV3 C API LCD functions.
 *
 * License:
 *
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
 * \author John Hansen (bricxcc_at_comcast.net)
 * \date 2013-08-04
 * \version 1
 *
 * ----------------------------------------------------------------------------
 *
 * \author Ahmand Fatoum & Simón Rodriguez Perez(Hochschule Aschaffenburg)
 * \date 2015-02-28
 * \version 2
 * \note printf function added
 */
 
#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_lcd_h
#define ev3_lcd_h

/* Print Text with Variables - Ahmad Fatoum*/
#ifdef __GNUC__
#pragma GCC system_header
#endif

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <libgen.h>

#include "ev3_constants.h"

#define LCD_BYTE_WIDTH ((LCD_WIDTH + 7) / 8)
#define LCD_BUFFER_SIZE (LCD_BYTE_WIDTH * LCD_HEIGHT)

typedef byte     IMGDATA;    //!< Image base type
typedef IMGDATA* IP;         //!< Instruction pointer type

typedef struct {
  int X;  /*!< The X coordinate. Valid range is from 0 to 177 inclusive.  */
  int Y;  /*!< The Y coordinate. Valid range is from 0 to 127 inclusive. */
} LocationType;

typedef LocationType* PLocationType;

typedef enum {
  ifRAW_FB0,
  ifRAW_BUF,
  ifXBM,
  ifP1,
  ifP4,
  ifBMP,
  ifPNG
} ImageFormat;

/*
#define SCREEN_WIDTH 178
#define SCREEN_HEIGHT 128
#define NOOF_CHARS 96
#define FONT_WIDTH 10
#define FONT_HEIGHT 16
#define CELL_WIDTH FONT_WIDTH
#define CELL_HEIGHT FONT_HEIGHT
#define DISPLAY_CHAR_WIDTH SCREEN_WIDTH / CELL_WIDTH
#define DISPLAY_CHAR_DEPTH SCREEN_HEIGHT / CELL_HEIGHT

#define ROP_CLEAR 0x00000000
#define ROP_AND 0xff000000
#define ROP_ANDREVERSE 0xff00ff00
#define ROP_COPY 0x0000ff00
#define ROP_ANDINVERTED 0xffff0000
#define ROP_NOOP 0x00ff0000
#define ROP_XOR 0x00ffff00
#define ROP_OR 0xffffff00
#define ROP_NOR 0xffffffff
#define ROP_EQUIV 0x00ffffff
#define ROP_INVERT 0x00ff00ff
#define ROP_ORREVERSE 0xffff00ff
#define ROP_COPYINVERTED 0x0000ffff
#define ROP_ORINVERTED 0xff00ffff
#define ROP_NAND 0xff0000ff
#define ROP_SET 0x000000ff
*/

bool LcdInit();
bool LcdOpen();
bool LcdClose();
bool LcdExit();
bool LcdInitialized();

void LcdRefresh();
void LcdSetAutoRefresh(bool bOn);
bool LcdUpdate();
bool LcdClean();
void LcdClearDisplay();
bool LcdScroll(short Y);
bool LcdRect(char Color, short X0, short Y0, short X1, short Y1);
bool LcdFillRect(char Color, short X0, short Y0, short X1, short Y1);
bool LcdInverseRect(short X0, short Y0, short X1, short Y1);
bool LcdPixel(char Color, short X, short Y);
bool LcdLine(char Color, short X0, short Y0, short X1, short Y1);
bool LcdDotLine(char Color, short X0, short Y0, short X1, short Y1, short OnPix, short OffPix);
bool LcdCircle(char Color, short X, short Y, short R);
bool LcdFillCircle(char Color, short X, short Y, short R);
bool LcdSelectFont(byte FontType);
byte* LcdGetDisplay();
bool LcdText(char Color, short X, short Y, char* Text);
bool LcdIcon(char Color, short X, short Y, char IconType, char IconNum);
bool LcdBmpFile(char Color, short X, short Y, char* Name);
bool LcdPicture(char Color, short X, short Y, IP pBitmap);
bool LcdVerticalBar(char Color, short X0, short Y0, short X1, short Y1, short Min, short Max, short Actual);
bool LcdFillWindow(char Color, short Y, short Y1);
byte* LcdGetFrameBuffer();
void LcdWriteDisplayToFile(char* filename, ImageFormat fmt);
void LcdWriteFrameBufferToFile(char* filename, ImageFormat fmt);

/**
 * Draw a circle.
 * This function lets you draw a circle on the screen with its center at the
 * specified x and y location, using the specified radius. Optionally specify
 * drawing options. If this argument is not specified it defaults to \ref DRAW_OPT_NORMAL.
 * Valid display option constants are listed in the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawCircle, DrawCircleType
 *
 * \param x The x value for the center of the circle.
 * \param y The y value for the center of the circle.
 * \param radius The radius of the circle.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char CircleOutEx(int x, int y, byte radius, unsigned long options);
#define CircleOut(_x, _y, _r) CircleOutEx((_x), (_y), (_r), DRAW_OPT_NORMAL)

/**
 * Draw a line.
 * This function lets you draw a line on the screen from x1, y1 to x2, y2.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawLine, DrawLineType
 *
 * \param x1 The x value for the start of the line.
 * \param y1 The y value for the start of the line.
 * \param x2 The x value for the end of the line.
 * \param y2 The y value for the end of the line.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char LineOutEx(int x1, int y1, int x2, int y2, unsigned long options);
#define LineOut(_x1, _y1, _x2, _y2) LineOutEx((_x1), (_y1), (_x2), (_y2), DRAW_OPT_NORMAL)

/**
 * Draw a point.
 * This function lets you draw a point on the screen at x, y.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawPoint, DrawPointType
 *
 * \param x The x value for the point.
 * \param y The y value for the point.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char PointOutEx(int x, int y, unsigned long options);
#define PointOut(_x, _y) PointOutEx((_x), (_y), DRAW_OPT_NORMAL)

/**
 * Draw a rectangle.
 * This function lets you draw a rectangle on the screen at x, y with the
 * specified width and height.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawRect, DrawRectType
 *
 * \param x The x value for the top left corner of the rectangle.
 * \param y The y value for the top left corner of the rectangle.
 * \param width The width of the rectangle.
 * \param height The height of the rectangle.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char RectOutEx(int x, int y, int width, int height, unsigned long options);
#define RectOut(_x, _y, _w, _h) RectOutEx((_x), (_y), (_w), (_h), DRAW_OPT_NORMAL)

/**
 * Draw text.
 * Draw a text value on the screen at the specified x and y location. The y
 * value must be a multiple of 8.  Valid line number constants are listed in
 * the \ref LineConstants group.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawText, DrawTextType
 *
 * \param x The x value for the start of the text output.
 * \param y The text line number for the text output.
 * \param str The text to output to the LCD screen.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char TextOutEx(int x, int y, char* str, unsigned long options);
#define TextOut(_x, _y, _str) TextOutEx((_x), (_y), (_str), DRAW_OPT_NORMAL)

/**
 * Draw a number.
 * Draw a numeric value on the screen at the specified x and y location. The y
 * value must be a multiple of 8.  Valid line number constants are listed in
 * the \ref LineConstants group.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawText, DrawTextType
 *
 * \param x The x value for the start of the number output.
 * \param y The text line number for the number output.
 * \param value The value to output to the LCD screen. Any numeric type is supported.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char NumOutEx(int x, int y, int value, unsigned long options);
#define NumOut(_x, _y, _v) NumOutEx((_x), (_y), (_v), DRAW_OPT_NORMAL)

/**
 * Draw an ellipse.
 * This function lets you draw an ellipse on the screen with its center at the
 * specified x and y location, using the specified radii. Optionally specify
 * drawing options. If this argument is not specified it defaults to \ref DRAW_OPT_NORMAL.
 * Valid display option constants are listed in the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawEllipse, DrawEllipseType
 *
 * \warning This function requires the enhanced NBC/NXC firmware version 1.28+.
 *
 * \param x The x value for the center of the ellipse.
 * \param y The y value for the center of the ellipse.
 * \param radiusX The x axis radius.
 * \param radiusY The y axis radius.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char EllipseOutEx(int x, int y, byte radiusX, byte radiusY, unsigned long options);
#define EllipseOut(_x, _y, _rx, _ry) EllipseOutEx((_x), (_y), (_rx), (_ry), DRAW_OPT_NORMAL)

/**
 * Draw a polygon.
 * This function lets you draw a polygon on the screen using an array of points.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group.
 * \sa SysDrawPolygon, DrawPolygonType
 *
 * \warning This function requires the enhanced NBC/NXC firmware version 1.28+.
 *
 * \param points An array of LocationType points that define the polygon.
 * \param options The optional drawing options.
 * \return The result of the drawing operation.
 */
char PolyOutEx(PLocationType points, unsigned long options);
#define PolyOut(_p) PolyOutEx((_p), DRAW_OPT_NORMAL)

/**
 * Draw a graphic image.
 * Draw a graphic image file on the screen at the specified x and y location.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group. If the file cannot be found then
 * nothing will be drawn and no errors will be reported.
 * \sa SysDrawGraphic, DrawGraphicType
 *
 * \param x The x value for the position of the graphic image.
 * \param y The y value for the position of the graphic image.
 * \param filename The filename of the RIC graphic image.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char GraphicOutEx(int x, int y, char* filename, unsigned long options);
#define GraphicOut(_x, _y, _f) GraphicOutEx((_x), (_y), (_f), DRAW_OPT_NORMAL)

/**
 * Draw a graphic image from byte array.
 * Draw a graphic image byte array on the screen at the specified x and y location.
 * Optionally specify drawing options. If this argument is not specified it
 * defaults to \ref DRAW_OPT_NORMAL. Valid display option constants are listed in
 * the \ref DisplayDrawOptionConstants group. If the file cannot be found then
 * nothing will be drawn and no errors will be reported.
 * \sa SysDrawGraphicArray, DrawGraphicArrayType
 *
 * \param x The x value for the position of the graphic image.
 * \param y The y value for the position of the graphic image.
 * \param data The byte array of the RIC graphic image.
 * \param options The drawing options.
 * \return The result of the drawing operation.
 */
char GraphicArrayOutEx(int x, int y, byte* data, unsigned long options);
#define GraphicArrayOut(_x, _y, _d) GraphicArrayOutEx((_x), (_y), (_d), DRAW_OPT_NORMAL)


bool LcdTextf(char Color, short X, short Y, const char *fmt, ...);
int LcdPrintf(char __color, const char * __fmt, ...);

#ifdef __ASPRINTF
#include <stdarg.h>
int asprintf(char **, const char *, ...);
int vasprintf(char **, const char *, va_list)
#endif


#endif // ev3_lcd_h

#ifdef __cplusplus
}
#endif
