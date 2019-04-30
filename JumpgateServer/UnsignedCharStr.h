
#if !defined( _UNSIGNED_CHAR_PTR_H )
#define _UNSIGNED_CHAR_PTR_H


#define TRADEWARS "Federation's_struggle"
#define TRADEWARS_LEN 21

#include <stdio.h>

class CUnsignedCharStr  {
    public :
        CUnsignedCharStr();
        CUnsignedCharStr(char *);
        virtual ~CUnsignedCharStr();

		int appendString(unsigned char *str, long strLength);
		int endsWithString(char *str);
		int trimStringRight(int count);
		int encryptString();
		unsigned long crc32();
		int decryptString();
		deleteString();
		long getLength();
		unsigned char* getString();
		int hasString(char []);
		unsigned char* getCrc32FromString(int count);
		int confirmCrc32(unsigned char *);

// These three functions are only used internally.
//
    private :
        unsigned char *unsignedCharPtr;
        long length;
		HANDLE appendMutex;
		unsigned long CalcCRC32(void *ptr, long size);
};



#endif  // #if !defined( _UNSIGNED_CHAR_PTR_H )
