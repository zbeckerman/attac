#ifndef _ABENDS_HPP_
#define _ABENDS_HPP_

#include "TWTypes.hpp"

char* BadUsage(char* cause, ULONG type);

char* BadSector(char* sect, USHORT line, ULONG type);

char* FileError(ULONG type);

char* OutOfMemory(void);

#endif
