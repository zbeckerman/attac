

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUnsignedCharStr  {
    public :
        CUnsignedCharStr();
        CUnsignedCharStr(char *);
        virtual ~CUnsignedCharStr();

		int appendString(unsigned char *str, long strLength);
		int endsWithString(char *str);
		int trimStringRight(int count);
		int encryptString();
		int decryptString();
		deleteString();
		long getLength();
		unsigned char* getString();

// These three functions are only used internally.
//
    private :
        unsigned char *unsignedCharPtr;
        long length;
		HANDLE appendMutex;
};



