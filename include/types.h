#ifndef _TYPES_H_
#define _TYPES_H_
#include <stdbool.h>
#include <stdint.h>

typedef unsigned char         u8;
typedef unsigned short        u16;
typedef unsigned int          u32;
typedef uintptr_t							ptr_t;

#ifndef min
#define min(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);	\
	_x < _y ? _x : _y; })
#endif

#ifndef max
#define max(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);	\
	_x > _y ? _x : _y; })
#endif

#endif
