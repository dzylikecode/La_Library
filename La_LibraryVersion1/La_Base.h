#ifndef _LA_BASE_TYPE_
#define _LA_BASE_TYPE_


#define SET_BIT(word, bit_flag)		((word) |= (bit_flag))
#define RESET_BIT(word, bit_flag)	((word) &= ~(bit_flag))


#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)




#define _LA_DEBUG_ON_
#define _LA_DEBUG_ON_BOTH_  
#define _LA_DEBUG_FILE_  0


#ifdef WIN32
#include <windows.h>
#else
typedef unsigned char  UCHAR;
typedef unsigned short USHORT;
typedef unsigned int   UINT;
typedef UCHAR          BYTE;
#endif




#endif