#ifndef __twx_h__
#define __twx_h__

int confirmCRCinTWXFile (char * );
int updateCRCinTWXFile (char * );
void writeCRCValue(FILE *, unsigned long );
unsigned long getCRC32(FILE *, int);
unsigned long CalcCRC32(void *, long);
int import_twx(char*);
int export_twx(char*);

#endif
