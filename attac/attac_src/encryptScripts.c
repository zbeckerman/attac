// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <sys/types.h>
//#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

#include "attacGlobal.h"
#include "encrypt.h"
#include "../PUTTY.H"

int main()
{
	struct stat buf;
	int existsFiles;
	char *fileData, *eString;
	char fileName[255];
	char newFileName[255];
	FILE *fp, *fp1;
	int choice=0;

	printf ("Do you want to (Select the number)\n1. Encrypt\n2. Decrypt\n3. Exit\n?");
	scanf ("%d", &choice);

	if (choice ==1)
	{
		printf ("Enter Filename to Encrypt\n?");
		scanf ("%s", fileName);

		strcpy (newFileName, fileName);
		newFileName[strlen(newFileName)-3]='e';

		existsFiles = stat(fileName, &buf);
		fileData = (char *) malloc (buf.st_size+1);

		fp = fopen (fileName, "r+b");
		if (fp==NULL)
		{
			printf ("Error opening file %s\n", fileName);
			exit(0);
		}
  
		fseek(fp, 0, SEEK_SET);
		fread (fileData, buf.st_size, 1, fp);	
		fileData[buf.st_size]=0;

		eString = encrypt_string ("pokd(3ldk394010193kkJJ*+2TDKJ3ie", fileData);

		fp1 = fopen (newFileName, "w+b");
		if (fp1==NULL)
		{
			printf ("Error opening file %s\n", newFileName);
			exit(0);
		}
		fprintf (fp1, "%s", eString);
		fclose(fp1);

		free(fileData);
		free(eString);
		printf ("\nFile Encrypted as %s\n", newFileName);
	}
	else if (choice==2)
	{
		printf ("Enter Filename to Decrypt\n?");
		scanf ("%s", fileName);

		eString = getFileAndDecrypt(fileName);

		printf ("%s", eString);
		free(eString);
	}
	else if (choice==3)
		printf ("Exiting ... \n");
	else
		printf ("Choice not understood.\nExiting ...\n");
	return (0);
}


