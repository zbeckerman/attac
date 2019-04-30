#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <iomanip.h>
#include "TWBFind.h"
#include "Globals.hpp"
#include "AbEnds.hpp"

char* BadUsage(char* cause, ULONG type)
{
	char *returnVal=NULL;
	char buffer[1024];

	/*
    cout << endl;
    cout << "Tradewars 2002 Bubble Finder v" << (int)Globals::majorVersion << "."
          << setfill('0') << setw(2) << (int)Globals::minorVersion;
    cout << "; by Mike Ruskai <thannymeister@yahoo.com>" << endl;
    cout << "This software is released to the public domain." << endl << endl;
    cout << "Usage: twbfind.exe [options]" << endl << endl;
    cout << "Options:" << endl << endl;
    cout << "/a  - avoid unexplored sectors when finding bubbles" << endl;
    cout << "/t  - use only one bubble route from primary entrance" << endl;
    cout << "/u  - update binary database only - don't do a bubble search" << endl;
    cout << "/f  - find fake bubbles (one-way sectors in)" << endl;
    cout << "/g  - gobble memory to increase speed during search" << endl;
    cout << "/m# - minimum number of sectors in bubble; default 5" << endl;
    cout << "/x# - maximum number of sectors in bubble; default 50" << endl;
    cout << "/n# - number of sectors in universe (100 to 65535); default 1000" << endl;
    cout << "/e# - maximum number of entrances for bubbles (1 to 50); default 1" << endl;
    cout << "/z# - entrance multiplier for minimum bubble size (2.0 to 1000.0); default 2.0" << endl;
    cout << "/r# - reserve storage for this many bubbles (1 to 4.3 billion); default 1024" << endl;
    #if (defined(__OS2__) || defined(__EMX__))
    cout << "/p# - priority class (1 to 4); default=2" << endl;
    cout << "/d# - priority delta (0 to 31); default=0" << endl;
    #endif
    cout << "/s<filename> - sector data file" << endl;
    cout << "/b<filename> - binary sector database file" << endl;
    cout << "/c<filename> - second binary database to combine info with" << endl;
    cout << "/w<filename> - capture of CIM course plots (zero-turn mapping)" << endl;
    cout << "/o<filename> - report file; default 'bubbles.lst'" << endl;
    cout << endl;
    cout << "Example: twbfind /m6 /x25 /n5000 /ssectors.txt /obubbles.lst";
    cout << endl << endl;
    cout << "The above searches for bubbles with a minimum of 6 sectors and a maximum of";
    cout << endl << "25 sectors, with one entrance, in a 5000-sector universe, with data";
    cout << endl << "read from 'sectors.txt', and found bubbles written to the file";
    cout << endl << "'bubbles.lst'." << endl;
*/
    switch (type)
        {
        case USAGE_HELP:
			sprintf (buffer, "ERROR: Help Shown");
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_MIN_SECTORS:
			sprintf (buffer, "ERROR: Invalid minimum number of sectors: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_MAX_SECTORS:
			sprintf (buffer, "ERROR: Invalid maximum number of sectors: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_SECTOR_FILE:
			sprintf (buffer, "ERROR: Bad sector data filename: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_REPORT_FILE:
			sprintf (buffer, "ERROR: Bad report filename: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_TOTAL_SECTORS:
			sprintf (buffer, "ERROR: Invalid number of sectors: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_PARAMETER:
			sprintf (buffer, "ERROR: Invalid parameter: %s",  cause);
			returnVal = (char *) strdup (buffer);
            break;

        case BAD_MAX_ENTRANCES:
			sprintf (buffer, "ERROR: Invalid maximum number of entrances: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_ENT_MULTIPLIER:
			sprintf (buffer, "ERROR: Invalid entrance multiplier: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_DUPE_OPTION:
			sprintf (buffer, "ERROR: Duplicate option: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case NO_SECTOR_DATA_FILE:
			sprintf (buffer, "ERROR: No sector data file provided");
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_MAX_BUBBLES:
			sprintf (buffer, "ERROR: Invalid maximum number of bubbles: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        #if (defined(__OS2__) || defined(__EMX__))
        case BAD_PRIORITY_CLASS:
			sprintf (buffer, "ERROR: Invalid priority class: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;

        case BAD_PRIORITY_DELTA:
			sprintf (buffer, "ERROR: Invalid priority delta: %s",  cause);
            returnVal = (char *) strdup (buffer);
            break;
        #endif
		default:
			sprintf (buffer, "ERROR: Unknown Bubble Finder Error");
            returnVal = (char *) strdup (buffer);
            break;
        }
	return (returnVal);
}

char* BadSector(char* sectNum, USHORT lineNum, ULONG type)
{
	char *returnVal=NULL;
	char buffer[1024];

//    cout << endl << endl;
//    cout << "Bad sector data on line " << lineNum << ": " << sectNum << endl;
//    cout << endl;
    switch (type)
        {
        case BADSECT_UNSPECIFIED:
			sprintf (buffer, "ERROR: Unspecified Bad Sector Error");
            returnVal = (char *) strdup (buffer);
            break;

        case BADSECT_TOO_LARGE:
			sprintf (buffer, "ERROR: Exceeds number of sectors (%d)", Globals::totalSectors);
            returnVal = (char *) strdup (buffer);
            break;

        case BADSECT_NOT_INTEGER:
			sprintf (buffer, "ERROR: Not a valid integer");
            returnVal = (char *) strdup (buffer);
            break;

        case BADSECT_DUP_DATA:
			sprintf (buffer, "ERROR: Duplicate sector data");
            returnVal = (char *) strdup (buffer);
            break;

        case BADSECT_MALFORMED_PLOT:
			sprintf (buffer, "ERROR: Malformed plot data");            
			returnVal = (char *) strdup (buffer);
            break;
		default:
			sprintf (buffer, "ERROR: Unknown Bad Sector Error");
            returnVal = (char *) strdup (buffer);
            break;
        }
	return (returnVal);
}

char* FileError(ULONG type)
{
	char *returnVal=NULL;
	char buffer[1024];

//    cout << endl << endl;
    switch (type)
        {
        case SECTOR_READ:
			sprintf (buffer, "ERROR: Error reading sector file");
            returnVal = (char *) strdup (buffer);
            break;

        case REPORT_WRITE:
			sprintf (buffer, "ERROR: Error writing to report file");
            returnVal = (char *) strdup (buffer);
            break;

        case BINARY_WRITE:
			sprintf (buffer, "ERROR: Error writing to binary sector file");
            returnVal = (char *) strdup (buffer);
            break;

        case INVALID_BINARY:
			sprintf (buffer, "ERROR: Specified binary file is not valid");
            returnVal = (char *) strdup (buffer);
            break;

        case INCOMPATIBLE_INFO:
			sprintf (buffer, "ERROR: More than 6 warps for a given sector were found");
            returnVal = (char *) strdup (buffer);
        }
    return (returnVal);
}


char* OutOfMemory(void)
{
    char *returnVal=NULL;
    returnVal = (char *) strdup ("Out of Memory");
    return (returnVal);
}
