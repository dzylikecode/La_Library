#ifndef _LA_FILE_BASE_
#define _LA_FILE_BASE_

#include <stdio.h>
#include <fstream>
#ifdef UNICODE
#define ftprintf   fwprintf
using tofstream = std::wofstream;
#else
#define ftprintf   fprintf
using tofstream = std::ofstream;
#endif



#include "La_Base.h"




#endif
