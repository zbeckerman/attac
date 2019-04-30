#ifndef __util_h__
#define __util_h__

#include "attacUtils.h"
#include "structs.h"
#include "writeMemory.h"
#include "hash.h"

#define MAX_COLUMNS 120
#define TOLLSFIGPAY 0
#define TOLLSFIGKILL 1
#define TOLLSFIGNOTHING 2

#define DEFENSIVEFIGKILL 0
#define DEFENSIVEFIGNOTHING 1

void resetGame(HWND tHwnd);
void startCIM();
void attac_send (char message[]);
void attac_send_char (char message);
void parseCIMInformation(int type, char *line);
void exportCIMPortData_Text();
void exportCIMSectorData_Text();
void exportAllCIMData_Text(int sector, int port);
void listview_insert_column(HWND hlistTmp, int column, int col_width, int pos, char *text);
void listview_insert(HWND hlistTmp, int row, char *port1, char *port2, char *minamount, char *minpercent, char *dist);
void listview_insert_porttype(HWND hlistTmp, int row, char *sector, char *fuelamount, char *fuelpercent, char* orgamount, char* orgpercent, char* equipamount, char* equippercent, char* dist);
void parseAndEnterUserFigs(char *fig_data);
void loadTraderTitles();
int getTradeNumber (char * traderData);
int getTradeNumberWithoutAdding (char * traderData);
int parseTradeNumber (char * traderData);
int isValidRegCode (char* user1, char* user2, char* user3, char* user4, char* reg, char* twxReg, int useRegTxtFile, char* userHandle);
char* strdelimit (char *string, char delimiter, char new_delimiter);
int isValidPairPort(long firstPort, long secondPort);
void setProgessBar (int newPosition, HANDLE progress);
long findNearestFigs (long theSector);
void parseTraderListing (char *str);
int sectorExistsInDatabase(long newSector, long currentSector);
void DSM_ProcessInbound (unsigned char c, int logmode);

void addToMainWindow(char *message, int forecolor, int backcolor, int bold, int enter);
//void addToMessageCenter (char* msg, int fore, int back, int bold);
void attacErrorMessage(char *msg);
void attacInfoMessage(char *msg);
void attacScriptMessage(char *header, char *msg, int fore, int back, int bold);
void attacScriptNotify(char *msg);

void attacFedCommMessage(char *msg, char *from);
void attacSubSpaceMessage(char *msg, char *from, long channel);
void attacHailMessage(char *msg, char *from);
void attacSentMessage(char *msg);
char* createANSIText(char *message, int forecolor, int backcolor, int bold, int enter);
long Get_Time();
char* EncryptCommMessage(char *text, char *name);
char* DecryptCommMessage(char *text, char *name);
char* getTraderName(char *tName);
void pfree (void **x);


/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorOneOffset(long theSector);
/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorZeroOffset(long theSector);


/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorOneOffsetString(char* theSector);

/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorZeroOffsetString(char* theSector);


#define ATTAC_BLACK 0
#define ATTAC_RED 1
#define ATTAC_GREEN 2
#define ATTAC_YELLOW 3
#define ATTAC_BLUE 4
#define ATTAC_MAGENTA 5
#define ATTAC_CYAN 6
#define ATTAC_WHITE 7

extern char theUserId[100];
extern int validTWXSyncRegCode;  //if twxsync code is valid
extern int validRegCode;  //if code is valid
extern int isRegistered;  //if user ids match. only check if validRegCode=1

//extern hash_table hTable;
extern int hashInUse;

extern HANDLE Data_Mutex;
extern char DSM_Buffer[MAX_LENGTH];
extern char DSM_BufferParseData[MAX_LENGTH];
extern char DSM_BufferStatus[MAX_LENGTH];

extern int countParens;
extern int gettingTWXData;

extern int GetCIMData;
extern int parseCIMData;
extern int CIMParseType;

extern int parseZTMData;

extern int recordFastMacro;

#define FASTMACROLEN 128
extern char fastMacroString[FASTMACROLEN];

extern char gameName[MAXAPPDIRLEN];

extern long numFigs;
extern long numShields;
extern long numTurns;
extern long numExp;
extern long numAlign;
extern long numCredits;
extern long numFuel;
extern long numOrgs;
extern long numEquip;
extern long numColos;
extern long numEmptyHolds;
extern long numTotalHolds;
extern long numSector;
extern long numCloaks;
extern long numPhotons;
extern long numProbes;
extern long numAMines;
extern long numLMines;

extern int Keyboard_Locked;
extern char traderTitle[TOTAL_TRADER_TITLES][24];
extern int buyingSelling;
extern int buyingSellingAmount;
extern int validDoubleY;
extern int commandPrompt; 
extern int passToREXX;
extern int runningTWXScript;

extern HANDLE RemoveCRsMutex;
extern HANDLE PFreeMutex;
extern HANDLE CleanLineMutex;
extern HANDLE UnCommaMutex;
extern HANDLE SyncDataMutex;
extern HANDLE MessageMutex;

typedef int (*ENCMSGFUNC)(unsigned char *data, int len, unsigned char *key, const char *name);
typedef int (*DECMSGFUNC)(unsigned char *data, int len, unsigned char *key, const char *name);

ENCMSGFUNC DLL_EncryptMessage;
DECMSGFUNC DLL_DecryptMessage;


#endif
