/*
* EV3-API
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/
*/

#ifndef EV3_BASICTYPES_H_
#define EV3_BASICTYPES_H_

#include <stdint.h>

// Some basic data type defines
#if 0
typedef   unsigned char         UBYTE;  //!< Basic Type used to symbolise 8  bit unsigned values
typedef   unsigned short        uint16_t;  //!< Basic Type used to symbolise 16 bit unsigned values
typedef   unsigned int          uint32_t;  //!< Basic Type used to symbolise 32 bit unsigned values
typedef   signed short          int16_t;  //!< Basic Type used to symbolise 16 bit signed values
typedef   signed char           int8_t;  //!< Basic Type used to symbolise 8  bit signed values
#endif
typedef   int8_t                 DATA8;  //!< VM Type for 1 byte signed value
typedef   int16_t                 DATA16; //!< VM Type for 2 byte signed value
typedef   float                 DATAF;  //!< VM Type for 4 byte floating point value

#endif //EV3_BASICTYPES_H_
