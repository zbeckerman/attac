#ifndef __structs_h__
#define __structs_h__

#include "attacUtils.h"

#ifndef M_PI
# define M_PI 3.141592654
#endif

#define MAXCYCLES 7
#define MAX_PLAYERS 800
#define MAX_PLANETS 2000
#define BUFSIZE 80

#define MAX_WARPS 6
#define UNDER_CONSTRUCTION 16
#define READ_WRITE_AMOUNT 1
#define MAX_ALLOWED_GAMES 50
#define TOTAL_TRADER_TITLES 47
#define CORP_CONSTANT 10000
#define MAX_MACROS 30
#define INVALID_FILE_HANDLE	-1

typedef struct {            // Size: 256 bytes
  char id[4];              // File ID: "ATTC"
  long time_created;      // Timestamp when created (See TS)
  long ver;               // File version (See VE)
  long sector_cnt;        // Total number of sectors
  long stardock;          // StarDock location (-1=Unknown)
  long cls0port_sol;      // Class 0 port: Sol (-1=Unknown)
  long cls0port_alpha;    // Class 0 port: Alpha Centauri (-1=Unknown)
  long cls0port_rylos;    // Class 0 port: Rylos (-1=Unknown)
  long crc32;             // Checksum (See CH)
  long player_cnt;        // Total number of players
  long planet_cnt;        // Total number of planets
  char name[32];
  char address[64];
  short port;

  short cimUpdateSectorsDefault;
  short cimUpdatePortsDefault;
  short tollTolls;
  short defensiveTolls;
  short daysToClearBust;
  long escapeSector;
  long lastZTMFromOneTillEnd;
  long lastZTMFromEndTillOne;
  long terra;
  long lastBustedSector;
  long lastRobbedSector;
  char fedCommKey[18];
  char reserved2[50];
  long lastAnnouncementStartup;
  short showPortOnMap;
  short selfDestructProtection;
  short blindTwarpProtection;
  short autoUpdateMap;
  short stealFactor;
  short worstFactor;
  char validStardockKeys[48];
  short updateInfoBarStartup;
  short announcementStartup;
  long lastTimeCIMRun;
  short detachWindows;
  short popupComms;
  short autoHaggle;
  short forceNoHaggle;
  long remoteConnectionPort;
  char remoteConnectionPassword[40];
  short minPairPortPercent;
  short popupOnRexxCompletion;
  long lastUpdateTime;
  long lastSyncTimeWithServer;
  char TWXSyncSession[128];
  char TWXSyncPassword[40];
  char TWXSyncServer[82];
  short TWXSyncPort;
  char reserved[708];      // Reserved for future use (Set to 0)
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
  char reserved[24];       // Reserved for future use (Set to 0)
} TWX_EXPORT_SECTOR_V1;

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


typedef struct {
  long sector;            // sector we are viewing
  int deadend;             // 1 if dead end, 0 if not
  int port;                // 1 if port, 0 if not
  char warps;
  long x;                   // x position on map
  long y;                   // y position on map
  long width;               // width of object drawn
  long height;              // height of object drawn
  long warpOne;           // warp one we are viewing
  long warpTwo;           // warp two we are viewing
  long warpThree;         // warp three we are viewing
  long warpFour;          // warp four we are viewing
  long warpFive;          //  warp five we are viewing
  long warpSix;           // warp six we are viewing
  char portInfo;
} MAP_INFO;
  
typedef struct {
  long sector;
  char parentX;
  char parentY;
  char currentLevel;
  /* we store the warps seperate from the parent */
  /* the warps tell us how we draw lines. The parent tells us who they
     are placed closest to relative to someone on the graph */
  long warpOne;  // warp one
  long warpTwo;  // warp two
  long warpThree;  // warp three
  long warpFour;   // warp four
  long warpFive;   //  warp five
  long warpSix;    // warp six  
} MAP_POINTS;

typedef struct {
  int x;
  int y;
} MAP_LINES;

typedef struct {            // Size: 256 bytes
  char id[4];              // File ID: "ATTC"
  long time_created;      // Timestamp when created (See TS)
  long ver;               // File version (See VE)
  long sector_cnt;        // Total number of sectors
  long stardock;          // StarDock location (-1=Unknown)
  long cls0port_sol;      // Class 0 port: Sol (-1=Unknown)
  long cls0port_alpha;    // Class 0 port: Alpha Centauri (-1=Unknown)
  long cls0port_rylos;    // Class 0 port: Rylos (-1=Unknown)
  long crc32;             // Checksum (See CH)
  char reserved[220];      // Reserved for future use (Set to 0)
} TWX_FILE_EXPORT_HEADER;

typedef struct {
	char key[64];
	char data[MAX_LENGTH];
} MACRO_STRUCT;

typedef enum {
  ERROR_NONE = 0,
  ERROR_FILE_ACCESS,
  ERROR_TWX_NOT_FOUND,
  ERROR_INVALID_TWX
} ATTAC_ERRORS;

//for some reason, the ATTACv4.dll does not compile unless these are taken out
#ifndef TWATTAC_EXPORTS
typedef struct planetList *planetListPtr;

struct planetList {
   char name[128];     
   planetListPtr next;
}  *start;

extern TWX_FILE_EXPORT_HEADER export_header;
extern TWX_FILE_HEADER header;
extern TWX_SECTOR *sectors;
extern TWX_PLAYER *players;
extern TWX_PLANET *planets;
extern MACRO_STRUCT *theMacros;
extern long currentSector;
extern int isOffline;
extern int isConnected;
extern int macroCount;
extern int helperActive;

extern planetListPtr **listPlanets;
extern int loadedPlanets;

#endif

ATTAC_ERRORS load_twx(char *);
ATTAC_ERRORS create_new_twx(char *);
int isBustedSector (long theSector);
char* generate_pp(long amount_filter, long percent_filter, long route_sector, int type, int pairType);
char* generate_evil_pp(long amount_filter, long percent_filter, long route_sector, int type);
void showPortTypeData(HWND hPortType, int portType);
void generate_maxed_ports(HANDLE hMaxPort, long maxAmount, int cargoType);
int isEnemy ();
int isPlayerAnEnemy (int figOwner);
int portSellsFuel (int porttype);
int portSellsOrg (int porttype);
int portSellsEquip (int porttype);
void printdebuginfo();
int buyHaggle (long offer2, int haggle);
int sellHaggle (long offer2, int haggle);
BOOL DisplayOS();
BOOL DisplaySystemVersion();

#endif
