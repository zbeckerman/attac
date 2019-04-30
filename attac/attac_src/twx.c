// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <winsock2.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "util.h"
#include "twx.h"

int confirmCRCinTWXFile (char *file_name)
{
  FILE *myFile;
  unsigned long crc32=0;
  struct stat buf;

  myFile = fopen(file_name, "rb");
  if (myFile == 0)
  {
    return(2);
  }
  else
  {
    stat(file_name, &buf);
    crc32 = getCRC32(myFile, buf.st_size);
  }
    
  fclose(myFile);
  if (crc32==0)
    return 0;
  else
    return 1;
}


int updateCRCinTWXFile (char *file_name)
{
  FILE *myFile;
  unsigned long crc32=0;
  struct stat buf;

  myFile = fopen(file_name, "r+b");

  if (myFile ==0)
  {
    return(2);
  }
  else
  {
    stat(file_name, &buf);
    crc32 = getCRC32(myFile, buf.st_size);
  }
  printf ("CRC32: %ld\n", crc32);
  writeCRCValue(myFile, crc32);
	
  fclose(myFile);
  return(0);
}


unsigned long CalcCRC32(void *ptr, long size)
{
  unsigned long crc32=0;
  unsigned long *lp;
  int i,n;

  lp=(unsigned long *)ptr;
  n=size/4;
  for (i=0; i<n; i++)
  {
    crc32=crc32^lp[i];
  }
  return crc32;
}

unsigned long getCRC32(FILE *pFile, int length)
{
  char buf[16]; 	  
  unsigned long crc32=0;
  int i=0,n=0;

  // Calculate checksum
  fseek(pFile, 0, SEEK_SET);

  n = length/16;

  for (i=0; i<n; i++)
  {
    fread(buf, 1, 16, pFile);
    crc32=crc32^CalcCRC32(buf, 16);
  }
  // Checksum ok, import file...

  return (ntohl(crc32));
}


void writeCRCValue(FILE *pFile, unsigned long crc32)
{
  int nActual=0;
  unsigned long tmpCRC=0;
  TWX_FILE_EXPORT_HEADER exportHeader;
	
  fseek(pFile, 0, SEEK_SET);

  nActual = fread(&exportHeader, sizeof( TWX_FILE_EXPORT_HEADER), 1, pFile); 

  tmpCRC = htonl(crc32);
  exportHeader.crc32=tmpCRC;
		
  fseek(pFile, 0, SEEK_SET);
  fwrite(&exportHeader, sizeof( TWX_FILE_EXPORT_HEADER ), 1, pFile);	
}

int import_twx(char* file_name)
{
  FILE *fp;
  size_t read;
  int i=0;
  int returnVal=0;

  TWX_FILE_EXPORT_HEADER tmp_header;
  TWX_EXPORT_SECTOR_V1 tmp_sectors;

  if (isConnected==0 && isOffline==0)
  {
    attacErrorMessage("No Game Selected. Click File->Connect. Then select a game. Then Work Offline or Connect");
    return (0);
  }

  if(strcmp(file_name, "")==0)
  {
    attacErrorMessage("No TWX file name selected");
    return(0);
  }

  memset (&tmp_header, 0, sizeof (TWX_FILE_EXPORT_HEADER));
  memset (&tmp_sectors, 0, sizeof (TWX_EXPORT_SECTOR_V1));
  
  // check the checksum
  returnVal = confirmCRCinTWXFile (file_name);

  if (returnVal == 1)
  {
    attacErrorMessage("Bad TWX FILE. Incorrect CRC value");
    return(0);
  }

  //read file in and import it
  fp = fopen(file_name, "rb");
  if (fp == NULL)
  {
    printf ("FILE ERROR 1\n");
    // return ERROR_FILE_ACCESS;
  }

  // write twx header
  fseek(fp, 0, SEEK_SET);
  read = fread(&tmp_header, sizeof(TWX_FILE_EXPORT_HEADER), READ_WRITE_AMOUNT, fp);
  
  if (read < READ_WRITE_AMOUNT)
  {
    fclose(fp);
    printf ("FILE ERROR 2\n");
    // return ERROR_FILE_ACCESS;
  }
  
  if ((long) header.sector_cnt != (long) ntohl(tmp_header.sector_cnt))
  {
    attacErrorMessage("Sector size of TWX file doesnt match sector size of database. File NOT imported.");
    return (0);
  }
  
  if (ntohl(tmp_header.ver)<1)
  {
    attacErrorMessage("Incorrect TWX version. File NOT imported.");
    return (0);
  }
  //dont copy the header in. that is bad :) since it is TWEX not ATTC
  //  strcpy(header.id, tmp_header.id);
  header.time_created = ntohl(tmp_header.time_created);
  //header.ver = ntohl(tmp_header.ver);
  header.sector_cnt = ntohl(tmp_header.sector_cnt);
  header.stardock = ntohl(tmp_header.stardock);
  header.cls0port_sol = ntohl(tmp_header.cls0port_sol);
  header.cls0port_alpha = ntohl(tmp_header.cls0port_alpha);
  header.cls0port_rylos = ntohl(tmp_header.cls0port_rylos);
  header.crc32 = ntohl(tmp_header.crc32);
  //not implemented 
  //  header.player_cnt = ntohl(tmp_header.player_cnt);
  //  header.planet_cnt = ntohl(tmp_header.planet_cnt);
  header.player_cnt = MAX_PLAYERS;
  header.planet_cnt = MAX_PLANETS;
  strcpy(header.reserved, tmp_header.reserved);

  //check for validation on the tmp_header
  if (strncmp(tmp_header.id, "TWEX", 4) != 0)
    {
      attacErrorMessage("Invalid file format. Not a valid TWX header.");
      return(0);
    }


  // write sector info
  for (i = 0; i < header.sector_cnt; i++)
  {
    read = fread(&tmp_sectors, sizeof(TWX_EXPORT_SECTOR_V1), READ_WRITE_AMOUNT, fp);
    if (read < READ_WRITE_AMOUNT)
    {
      fclose(fp);
      printf ("FILE ERROR 3\n");
      //return ERROR_INVALID_TWX;
    }

    sectors[i].info = tmp_sectors.info;
    sectors[i].navhaz = tmp_sectors.navhaz;
    sectors[i].reserved2 = ntohs(tmp_sectors.reserved2);
    sectors[i].ftr_type = tmp_sectors.ftr_type;
    sectors[i].anom = tmp_sectors.anom;
    sectors[i].port_per[0] = tmp_sectors.port_per[0];
    sectors[i].port_per[1] = tmp_sectors.port_per[1];
    sectors[i].port_per[2] = tmp_sectors.port_per[2];
    sectors[i].warps = tmp_sectors.warps;
      
    sectors[i].sector_update = ntohl(tmp_sectors.sector_update);
    sectors[i].ftrs = ntohl(tmp_sectors.ftrs);
    sectors[i].port_amt[0] = ntohl(tmp_sectors.port_amt[0]);
    sectors[i].port_amt[1] = ntohl(tmp_sectors.port_amt[1]);
    sectors[i].port_amt[2] = ntohl(tmp_sectors.port_amt[2]);
    sectors[i].warp_sect[0] = ntohl(tmp_sectors.warp_sect[0]);
    sectors[i].warp_sect[1] = ntohl(tmp_sectors.warp_sect[1]);
    sectors[i].warp_sect[2] = ntohl(tmp_sectors.warp_sect[2]);
    sectors[i].warp_sect[3] = ntohl(tmp_sectors.warp_sect[3]);
    sectors[i].warp_sect[4] = ntohl(tmp_sectors.warp_sect[4]);
    sectors[i].warp_sect[5] = ntohl(tmp_sectors.warp_sect[5]);
    sectors[i].port_update = ntohl(tmp_sectors.port_update);
    sectors[i].density = ntohl(tmp_sectors.density);
      
    sectors[i].ftr_owner = ntohs(tmp_sectors.ftr_owner);
    sectors[i].armids = ntohs(tmp_sectors.armids);
    sectors[i].armid_owner = ntohs(tmp_sectors.armid_owner);
    sectors[i].limpets = ntohs(tmp_sectors.limpets);
    sectors[i].limpet_owner = ntohs(tmp_sectors.limpet_owner);
    strcpy(sectors[i].reserved, tmp_sectors.reserved);
  }
  fclose(fp);
  
  printf ("TWX IMPORT WORKED\n");
  return(1);
}

int export_twx(char* file_name)
{
  FILE *fp;
  size_t write;
  int i=0;
  int returnVal=0;

  TWX_FILE_EXPORT_HEADER tmp_header;

  TWX_EXPORT_SECTOR_V1 tmp_sectors;

  if (isConnected==0 && isOffline==0)
  {
    attacErrorMessage("No Game Selected. Click File->Connect. Then select a game. Then Work Offline or Connect");
    return (0);
  }

  if(strcmp(file_name, "")==0)
  {
    attacErrorMessage("No TWX file name selected");
    return (0);
  }

  memset (&tmp_sectors, 0, sizeof (TWX_EXPORT_SECTOR_V1));
  memset (&tmp_header, 0, sizeof (TWX_FILE_EXPORT_HEADER));
  
  fp = fopen(file_name, "wb");
  if (fp == NULL)
  {
    printf ("FILE ERROR 1\n");
    // return ERROR_FILE_ACCESS;
  }
  strcpy(tmp_header.id, "TWEX");
  tmp_header.time_created = htonl(header.time_created);
  tmp_header.ver = htonl(1);
  //tmp_header.ver = htonl(header.ver);
  tmp_header.sector_cnt = htonl(header.sector_cnt);
  tmp_header.stardock = htonl(header.stardock);
  tmp_header.cls0port_sol = htonl(header.cls0port_sol);
  tmp_header.cls0port_alpha = htonl(header.cls0port_alpha);
  tmp_header.cls0port_rylos = htonl(header.cls0port_rylos);
  tmp_header.crc32 = 0;
//  tmp_header.player_cnt = htonl(header.player_cnt);
//  tmp_header.planet_cnt = htonl(header.planet_cnt);
  memset (&tmp_header.reserved, 0, sizeof (tmp_header.reserved));
  
  // write twx header
  fseek(fp, 0, SEEK_SET);
  write = fwrite(&tmp_header, sizeof(TWX_FILE_EXPORT_HEADER), READ_WRITE_AMOUNT, fp);
  
  if (write < READ_WRITE_AMOUNT)
  {
    fclose(fp);
    printf ("FILE ERROR 2\n");
    // return ERROR_FILE_ACCESS;
  }
  
  // write sector info
  for (i = 0; i < header.sector_cnt; i++)
  {
    tmp_sectors.info = sectors[i].info;
    tmp_sectors.navhaz = sectors[i].navhaz;
    tmp_sectors.reserved2 = htons(sectors[i].reserved2);
    tmp_sectors.ftr_type = sectors[i].ftr_type;
    tmp_sectors.anom = sectors[i].anom;
    tmp_sectors.port_per[0] = sectors[i].port_per[0];
    tmp_sectors.port_per[1] = sectors[i].port_per[1];
    tmp_sectors.port_per[2] = sectors[i].port_per[2];
    tmp_sectors.warps = sectors[i].warps;
      
    tmp_sectors.sector_update = htonl(sectors[i].sector_update);
    tmp_sectors.ftrs = htonl(sectors[i].ftrs);
    tmp_sectors.port_amt[0] = htonl(sectors[i].port_amt[0]);
    tmp_sectors.port_amt[1] = htonl(sectors[i].port_amt[1]);
    tmp_sectors.port_amt[2] = htonl(sectors[i].port_amt[2]);
    tmp_sectors.warp_sect[0] = htonl(sectors[i].warp_sect[0]);
    tmp_sectors.warp_sect[1] = htonl(sectors[i].warp_sect[1]);
    tmp_sectors.warp_sect[2] = htonl(sectors[i].warp_sect[2]);
    tmp_sectors.warp_sect[3] = htonl(sectors[i].warp_sect[3]);
    tmp_sectors.warp_sect[4] = htonl(sectors[i].warp_sect[4]);
    tmp_sectors.warp_sect[5] = htonl(sectors[i].warp_sect[5]);
    tmp_sectors.port_update = htonl(sectors[i].port_update);
    tmp_sectors.density = htonl(sectors[i].density);
      
    tmp_sectors.ftr_owner = htons(sectors[i].ftr_owner);
    tmp_sectors.armids = htons(sectors[i].armids);
    tmp_sectors.armid_owner = htons(sectors[i].armid_owner);
    tmp_sectors.limpets = htons(sectors[i].limpets);
    tmp_sectors.limpet_owner = htons(sectors[i].limpet_owner);
    memset (&tmp_sectors.reserved, 0, sizeof (tmp_sectors.reserved));
      
    write = fwrite(&tmp_sectors, sizeof(TWX_EXPORT_SECTOR_V1), READ_WRITE_AMOUNT, fp);
    if (write < READ_WRITE_AMOUNT)
    {
      fclose(fp);
      printf ("FILE ERROR 3\n");
      //return ERROR_INVALID_TWX;
    }
  }
  fclose(fp);

  returnVal = updateCRCinTWXFile (file_name);

  if (returnVal==0)
    {
      printf ("TWX EXPORT WORKED\n");
      return (1);
    }
  else
    {
      printf ("TWX EXPORT DIDNT WORK. CODE: %d\n", returnVal);
      return(0);
    }
}
