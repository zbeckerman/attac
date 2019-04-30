#ifndef _TWBFIND_H_
#define _TWBFIND_H_


// Choose between C and C++
// Comment the following line for C only
//#define TWATTAC_EXPORTS


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TWATTAC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TWATTAC_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef TWATTAC_EXPORTS
	#ifdef TWATTAC_CPP
		#define TWATTAC_API __declspec(dllexport)
		#define TWATTAC_CLASS TWATTAC_API
	#else
		#ifdef __cplusplus
			#define TWATTAC_API extern "C" __declspec(dllexport)
		#else
			#define TWATTAC_API __declspec(dllexport)
		#endif
		#define TWATTAC_CLASS
	#endif
#else
	#ifdef TWATTAC_CPP
		#define TWATTAC_API __declspec(dllimport)
		#define TWATTAC_CLASS TWATTAC_API
	#else
		#ifdef __cplusplus
			#define TWATTAC_API extern "C" __declspec(dllimport)
		#else
			#define TWATTAC_API __declspec(dllimport)
		#endif
		#define TWATTAC_CLASS
	#endif
#endif

#define USAGE_HELP                       0
#define BAD_MIN_SECTORS                  1
#define BAD_MAX_SECTORS                  2
#define BAD_SECTOR_FILE                  3
#define BAD_REPORT_FILE                  4
#define BAD_TOTAL_SECTORS                5
#define BAD_PARAMETER                    6
#define BAD_MAX_ENTRANCES                7
#define BAD_PRIORITY_CLASS               8
#define BAD_PRIORITY_DELTA               9
#define BAD_ENT_MULTIPLIER               10
#define BAD_PLOT_FILE                    11
#define NO_SECTOR_DATA_FILE              12
#define BAD_DUPE_OPTION                  13
#define BAD_MAX_BUBBLES                  14
#define BADSECT_UNSPECIFIED              100
#define BADSECT_TOO_LARGE                101
#define BADSECT_NOT_INTEGER              102
#define BADSECT_DUP_DATA                 103
#define BADSECT_MALFORMED_PLOT           104
#define SECTOR_READ                      201
#define REPORT_WRITE                     202
#define BINARY_WRITE                     203
#define INVALID_BINARY                   204
#define INCOMPATIBLE_INFO                205

#define DLL_MAX_WARPS 6

#define _TWBSIG_1_ 0xADDE3412
#define _TWBSIG_2_ 0x2143CEFA

#define _TWBMIN_MAJOR_ 0L
#define _TWBMIN_MINOR_ 14L

#include "../attac/attac_src/structs.h"
/*
typedef struct {            // Size: 96 bytes
  char info;               // Sector info (See SI)
  char navhaz;             // NavHaz percentage (0-100) (-1=Unknown)
  short reserved2;         // Reserved for future use (Set to 0)
  long sector_update;     // Timestamp from last sector update
  long ftrs;              // Fighters (-1=Unknown)
  short ftr_owner;         // Fighter owner (Reserved, set to -1)
  char ftr_type;           // Fighter type (1=Toll, 2=Offensive, 3=Defensive,
                            //  0=Mercenary, -1=Unknown)
  char anom;               // Anomality (0=No, 1=Yes, -1=Unknown)
  short armids;            // Armid mines (-1=Unknown)
  short armid_owner;       // Armid owner (Reserved, set to -1)
  short limpets;           // Limpet mines (-1=Unknown)
  short limpet_owner;      // Limpet owner (Reserved, set to -1)
                            //  (0=Never updated) (See TS and USI)
  long port_amt[3];       // Port amount [FOE] (-1=Unknown)
  char port_per[3];        // Port percentage [FOE] (0-100) (-1=Unknown)
  char warps;              // # of warp sectors (-1=Unknown)
  long warp_sect[6];      // Warp sectors
  long port_update;       // Timestamp from last port update
                            //  (0=Never updated) (See TS and USI)
  long density;           // Sector density (-1=Unknown)
  long lastBusted;
  char avoided;             // avoided? (0=No - 1 = Yes)
  char reserved[31];       // Reserved for future use (Set to 0)
} TWX_SECTOR;
*/
TWATTAC_API void findATTACBubbles (int argc, char* argv[], TWX_SECTOR *sectors, char* bubbleString);

#endif
