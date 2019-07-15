
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
