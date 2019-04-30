// Common.h
#ifndef COMMON_H
#define COMMON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Common include files
#include <windows.h>
#include <stdio.h>
#include "List.h"
#include "Hash.h"
#include "3PP/DES/DES.h"

// Common defines
#define MAX_LENGTH 1024
#define MAX_BUF_LEN 2048
#define MAX_GAMES 10
#define MAX_PLAYERS 800
#define MAX_PLANETS 2000
#define READ_WRITE_AMOUNT 1
#define UNDER_CONSTRUCTION 16
#define TWXSYNX_VERSION 1
#define CORP_CONSTANT 10000
#define DATA_COMPLETE "COMPLETED"
#define DATA_COMPLETE_LEN strlen(DATA_COMPLETE)
#define CRC_32 "CRC_32"
#define CRC_32_LEN strlen(CRC_32)
#define SOCKET_BUFFER 4096

// Common variables
extern char s_error_msg[MAX_BUF_LEN*4];
extern int gameCount;
extern HANDLE SyncUsersMutex;
extern HANDLE freeMutex;
extern HANDLE zlibEngineMutex;

extern char appDir[MAX_LENGTH];
extern const char gameExtension[5];

extern HANDLE parseSafeMutex;
extern HANDLE ListOfConnectedUsersMutex;

//version of software
extern int releaseVersion;
extern int majorVersion;
extern int minorVersion;

// Common function declarations
void TWC_Encode64(unsigned char *data, int n);
void TWC_Decode64(unsigned char *data, int n);
long Get_Time();

typedef struct {            // Size: 256 bytes
  char id[4];              // File ID: "ATTC"
  long time_created;      // Timestamp when created (See TS)
  long ver;               // File version (See VE)
  long sector_cnt;        // Total number of sectors
  long lastUpdateTime;
  long player_cnt;
  long planet_cnt;
  char sessionId[128];
  long stardock;          // StarDock location (-1=Unknown)
  long cls0port_sol;      // Class 0 port: Sol (-1=Unknown)
  long cls0port_alpha;    // Class 0 port: Alpha Centauri (-1=Unknown)
  long cls0port_rylos;    // Class 0 port: Rylos (-1=Unknown)
  char password[40];
  char file_name[1024];
  char reserved[968];      // Reserved for future use (Set to 0)
} TWX_FILE_HEADER;

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
  long lastBusted;			//when you last busted.
  long playerIdBusted;		//who busted
  char avoided;             // avoided? (0=No - 1 = Yes)
  char reserved[27];       // Reserved for future use (Set to 0)
} TWX_SECTOR;


typedef struct {            // 96 bytes
  char id[4];              // STRUCT ID: "PLAY"
  short autonum;           // unique number for each player
  char reserved2;         // the corp number the player is in
  char alliance;           // enemy =1 ally =0
  long experience;        // players experience
  long alignment;         // players alignment
  long lastKnownSector;   // players last known sector
  short corpNumber;       // timestamp from last update
  short reserved3;		  //reserved2
  char shipName[32];       // ships name. max name is 30 characters in TW
  char name[40];           // this is the players name max name is 40 chars in TW
  long last_update;			//time player was last updated
  char reserved[32];       // Reserved for future use (Set to 0)
} TWX_PLAYER;

typedef struct {            // 96 bytes
  char id[4];              // STRUCT ID: "PLAN"
  short autonum;           // unique number for each planet
  short planetNumber;      // planet number give by TW
  long sector;            // sector
  short createdBy;         // individual planet was created by
  short ownedBy;           // individual planet is owned by
  char citadel;            // citadel level (0 means no citadel) 
  char citclass;              // planets class (M,K,O,L,C,H,U or any others)
  char mReaction;          // the military reaction of a planet
  char iGenerator;         // 0 if the interdictor is inactive. 1 if active. should be zero
                            // citadel level is < 6
  short transwarpPower;    // transwarp power of planet
  short transportPower;    // transport power. 0 if a one does not exist or citadel < 1
  char qCannonPower;       // percent the qcannon is powered. 0 if citadel < 3
  char qCannonAtmos;       // percent of atmos blast.  0 if citadel < 3
  char qCannonSector;      // percent of sector blast.  0 if citadel < 3
  char reserved2;           // reserved for future use
  long fighters;          // number of fighters on planet
  long shields;           // number of shields on planet
  long treasury;          // cash in treasury
  long figsPerDay;        // number of figs produced per day
  long totalColonists;    // total number of colonist
  long last_update;       // timestamp from last update
  char name[70];           // name of the planet
  char reserved[90];       // Reserved for future use (Set to 0)
} TWX_PLANET;

extern TWX_FILE_HEADER headerArray[MAX_GAMES];
extern TWX_SECTOR *sectorsArray[MAX_GAMES];
extern TWX_PLAYER *playersArray[MAX_GAMES];
extern TWX_PLANET *planetsArray[MAX_GAMES];
extern CHash *traderHashArray[MAX_GAMES];



#endif
