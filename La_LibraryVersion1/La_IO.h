#ifndef _LA_IO_
#define _LA_IO_


#include <iostream>
#include <stdio.h>

inline void SetUnicodeInMainFunciton() { setlocale(LC_CTYPE, ""); }

#ifdef UNICODE
#define tcout   std::wcout
#define tcin    std::wcin
#else
#define tcout   std::cout
#define tcin    std::cin
#endif

#include "LA_Console.h"
#include "La_FileBase.h"


#endif
