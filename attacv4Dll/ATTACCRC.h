#ifndef __ATTAC_CRC_H__
#define __ATTAC_CRC_H__

#include <winsock2.h>			// for network to host order and visa versa
#include <stdio.h>
#include <sys/stat.h>
/*
typedef struct					// Size: 256 bytes
{
        char id[4];				// File ID: "TWEX"
        long time_created;		// Timestamp when created (See TS)
        long ver;				// File version (See VE)
        long sectors;			// Total number of sectors
        long stardock;			// StarDock location (-1=Unknown)
        long cls0port_sol;		// Class 0 port: Sol (-1=Unknown)
        long cls0port_alpha;	// Class 0 port: Alpha Centauri (-1=Unknown)
        long cls0port_rylos;	// Class 0 port: Rylos (-1=Unknown)
        long crc32;				// Checksum (See CH)
        char reserved[220];		// Reserved for future use (Set to 0)
} TWX_FILE_HEADER;
*/
int confirmCRCinTWXFile (char * );
int updateCRCinTWXFile (char * );
void writeCRCValue(FILE *, unsigned long );
unsigned long getCRC32(FILE *, int);
unsigned long CalcCRC32(void *, long);

#endif