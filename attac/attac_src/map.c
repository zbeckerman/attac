// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include "attacGlobal.h"
#include "../PUTTY.h"
#include "structs.h"
#include "ztm.h"
#include "map.h"
#include "mapqueue.h"
#include "attacGUI.h"
#include "bubbleFinder.h"

MAP_POINTS mapPoints[GRID_SIZE_X][GRID_SIZE_Y];
MAP_LINES *mapLines;

#define RGBBLACK     RGB(0,0,0)
#define RGBRED       RGB(128,0,0)
#define RGBGREEN     RGB(0,128,0)
#define RGBBLUE      RGB(0,0,128)
#define RGBBROWN     RGB(128,128,0)
#define RGBMAGENTA   RGB(128,0,128)
#define RGBCYAN      RGB(0,128,128)
#define RGBLTGRAY    RGB(192,192,192)
#define RGBGRAY      RGB(128,128,128)
#define RGBLTRED     RGB(255,0,0)
#define RGBLTGREEN   RGB(0,255,0)
#define RGBLTBLUE    RGB(0,0,255)
#define RGBYELLOW    RGB(255,255,0)
#define RGBLTMAGENTA RGB(255,0,255)
#define RGBLTCYAN    RGB(0,255,255)
#define RGBWHITE     RGB(255,255,255)

#define POS_TOP    1
#define POS_BOTTOM 2
#define POS_LEFT   3
#define POS_RIGHT  4

void PaintSector(HDC hDC, int posX, int posY, char* sector, int secLength, char* port, int portLength, COLORREF sectorColor, int offSet, int isDeadEnd)
{
  HPEN hPen, hOldPen;
  HBRUSH hBrush, hOldBrush;

  if (isDeadEnd==1)
	  hPen = CreatePen(PS_DASH, 1, sectorColor);
  else
	  hPen = CreatePen(PS_SOLID, 3, sectorColor);

  hBrush = CreateSolidBrush(RGBWHITE);
  hOldPen = SelectPen(hDC,hPen);
  hOldBrush = SelectBrush(hDC,hBrush);

  Ellipse(hDC,posX+offSet,posY,50+posX+offSet,50+posY);

  hBrush = SelectBrush(hDC,hOldBrush);
  DeleteObject(hBrush);
  hPen = SelectPen(hDC,hOldPen);
  DeleteObject(hPen);

  SetTextColor(hDC, sectorColor);
  SetBkMode(hDC, TRANSPARENT);
  TextOut(hDC, posX+offSet+7, posY+10, sector, secLength);
  if (header.showPortOnMap)
	TextOut(hDC, posX+offSet+7, posY+25, port, portLength);
}

void DrawArrow(HDC hDC, int startX, int startY, int endX, int endY, int startPos, int endPos, int startArrow, int endArrow, int offSet, int oneWayWarp)
{
  double slopy , cosy , siny;
  double Par = 5.0;  //length of Arrow (>)
  slopy = atan2( ( startY - endY ),
    ( startX + offSet - endX - offSet ) );
  cosy = cos( slopy );
  siny = sin( slopy ); //need math.h for these functions

  //draw a line between the 2 endpoint
  MoveToEx(hDC,  startX + offSet, startY , NULL);
  LineTo(hDC,  endX + offSet, endY );
  
  //here is the tough part - actually drawing the arrows
  //a total of 6 lines drawn to make the arrow shape
  if (oneWayWarp != 2)
  {
	MoveToEx(hDC,  startX + offSet, startY, NULL);
	LineTo(hDC,  startX + offSet + (int)( - Par * cosy - ( Par / 2.0 * siny ) ),
	startY + (int)( - Par * siny + ( Par / 2.0 * cosy ) ) );
	LineTo(hDC,  startX + offSet + (int)( - Par * cosy + ( Par / 2.0 * siny ) ),
	startY - (int)( Par / 2.0 * cosy + Par * siny ) );
	LineTo(hDC,  startX + offSet, startY );
  }  

  //-------------similarly the the other end-------------
	MoveToEx(hDC,  endX + offSet, endY , NULL);
	LineTo(hDC,  endX + offSet + (int)( Par * cosy - ( Par / 2.0 * siny ) ),
	endY + (int)( Par * siny + ( Par / 2.0 * cosy ) ) );
	LineTo(hDC,  endX + offSet + (int)( Par * cosy + Par / 2.0 * siny ),
	endY - (int)( Par / 2.0 * cosy - Par * siny ) );
	LineTo(hDC,  endX + offSet, endY );
}


void PaintRect(HDC hdc, RECT *rect, COLORREF colour)
{
    COLORREF oldcr = SetBkColor(hdc, colour);
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rect, "", 0, 0);
    SetBkColor(hdc, oldcr);
}




void buildMapDataStructure(HWND hwnd, long sector, int maxLevels, int height, int width)
{
	int k, i;

	/* starting sector */

	for(i=0;i<header.sector_cnt;i++) 
	{
		mapLines[i].x=0;
		mapLines[i].y=0;
	}
	/* init map points */
	for(i=0;i<GRID_SIZE_X;i++) 
		for(k=0;k<GRID_SIZE_X;k++) 
		{ 
			mapPoints[i][k].sector=0;
			mapPoints[i][k].parentX=0;
			mapPoints[i][k].parentY=0;
			mapPoints[i][k].warpOne=0;
			mapPoints[i][k].warpTwo=0;
			mapPoints[i][k].warpThree=0;
			mapPoints[i][k].warpFour=0;
			mapPoints[i][k].warpFive=0;
			mapPoints[i][k].warpSix=0;
		}

	if (sector <=0 || sector>header.sector_cnt)
		return;
	/* load the map up. send sector then parent. starting level is always 0, next
	the ith warp, and the next number is the number of levels to show. */

	loadMap(sector, maxLevels, (width/20), (height/20));

	queueMap2Empty();
}  


void loadMap(long sector, int maxLevel, int parentX, int parentY)
{
	int i=0, currentLevel=1, nextWarp=0;
	long count=0;
	int newX=0, newY=0;
	char pushMap[MAX_MAP_LENGTH], *popMap=NULL, *token=NULL;
	long popSector=0, popParentX=0, popParentY=0, popLevel=0;

	// create first sector
	sprintf (pushMap, "%ld:%d:%d:%d", sector, parentX, parentY, currentLevel);

	//push the first sector onto the queue 
	pushMapQueue(pushMap);
//	printf ("pushing: %s\n", pushMap);
	//run until queue is empty 
	while (!isMapQueueEmpty())
	{
		popMap = popMapQueue();
		if (popMap==NULL)
		{
			printf ("1. Empty popMap\n");
			break;
		}

		token = strtok (popMap, ":");
		while (token != NULL)
		{
			if (count==0)
				popSector=atol(token);
			else if (count==1)
				popParentX=atol(token);
			else if (count==2)
				popParentY=atol(token);
			else if (count==3)
				popLevel=atol(token);

			token = strtok (NULL, ":");
			count++;
		}

		count=0;

		free (popMap);

		if (popLevel > maxLevel)
		{
			while (1);
			{
				popMap = popMapQueue();
				if (popMap==NULL)
				{
					printf ("2. Empty popMap\n");
					break;
				}
				#ifdef P_FREE
					pfree(&popMap);           // free present block        
				#else
					free(popMap);           // free present block        
				#endif		
			}
		}

		// this is a new sector, so add it to the MapPoints and push
		//   its children onto the queue 
		if (mapLines[popSector-1].x==0 && mapLines[popSector-1].y==0)
		{  
			if (popSector==sector)
			{
				newX = parentX;
				newY = parentY;
			}
			else
			{
				getNextValidXY(popParentX, popParentY, &newX, &newY);
			}

			if (newX<0 || newY <0)
				return;
			if (newX>=GRID_SIZE_X || newY >= GRID_SIZE_Y)
				return;

			mapPoints[newX][newY].sector=popSector;
			mapPoints[newX][newY].warpOne=sectors[popSector-1].warp_sect[0];
			mapPoints[newX][newY].warpTwo=sectors[popSector-1].warp_sect[1];
			mapPoints[newX][newY].warpThree=sectors[popSector-1].warp_sect[2];
			mapPoints[newX][newY].warpFour=sectors[popSector-1].warp_sect[3];
			mapPoints[newX][newY].warpFive=sectors[popSector-1].warp_sect[4];
			mapPoints[newX][newY].warpSix=sectors[popSector-1].warp_sect[5];
			//build the lines table for later
			mapLines[popSector-1].x=newX;
			mapLines[popSector-1].y=newY;

			// push his children
			if (popLevel<maxLevel)
			{
				if (sectors[popSector-1].warp_sect[0] != 0)
				{
					// start with first warp
					nextWarp = sectors[popSector-1].warp_sect[0];
				
					for (i=0; i<6 && (nextWarp != 0); i++)
					{
						// create first sector
						sprintf (pushMap, "%d:%d:%d:%d", nextWarp, newX, newY, currentLevel+1);

						//push the first sector onto the queue 
						pushMapQueue(pushMap);

						// get next warp 
						if (i==0)
							nextWarp = sectors[popSector-1].warp_sect[1];
						else if (i==1)
							nextWarp = sectors[popSector-1].warp_sect[2];
						else if (i==2)
							nextWarp = sectors[popSector-1].warp_sect[3];
						else if (i==3)
							nextWarp = sectors[popSector-1].warp_sect[4];
						else if (i==4)
							nextWarp = sectors[popSector-1].warp_sect[5];
					}
				}
			}
		}
	}
}  


void getNextValidXY(int x, int y, int *i, int *j)
{
//	printf ("x %d; y %d -- %ld\n", x, y, mapPoints[x-(1*DISTANCE)][y-(1*DISTANCE)].sector);
  /* these are the closest points. choose them first */
  if (mapPoints[x-(1*DISTANCE)][y-(1*DISTANCE)].sector==0)
    {
      *i=x-(1*DISTANCE);
      *j=y-(1*DISTANCE);
    }
  else if (mapPoints[x+(0*DISTANCE)][y-(2*DISTANCE)].sector==0)
    {
      *i=x+(0*DISTANCE);
      *j=y-(2*DISTANCE);
    }
  else if (mapPoints[x+(1*DISTANCE)][y-(1*DISTANCE)].sector==0)
    {
      *i=x+(1*DISTANCE);
      *j=y-(1*DISTANCE);
    }
  else if (mapPoints[x+(1*DISTANCE)][y+(1*DISTANCE)].sector==0)
    {
      *i=x+(1*DISTANCE);
      *j=y+(1*DISTANCE);
    }
  else if (mapPoints[x+(0*DISTANCE)][y+(2*DISTANCE)].sector==0)
    {
      *i=x+(0*DISTANCE);
      *j=y+(2*DISTANCE);
    }
  else if (mapPoints[x-(1*DISTANCE)][y+(1*DISTANCE)].sector==0)
    {
      *i=x-(1*DISTANCE);
      *j=y+(1*DISTANCE);
    }

  /* now choose ones that are further */

  else if (mapPoints[x-(3*DISTANCE)][y+(1*DISTANCE)].sector==0)
    {
      *i=x-(3*DISTANCE);
      *j=y+(1*DISTANCE);
    }
  else if (mapPoints[x-(3*DISTANCE)][y-(1*DISTANCE)].sector==0)
    {
      *i=x-(3*DISTANCE);
      *j=y-(1*DISTANCE);
    }
  else if (mapPoints[x+(3*DISTANCE)][y+(1*DISTANCE)].sector==0)
    {
      *i=x+(3*DISTANCE);
      *j=y+(1*DISTANCE);
    }
  else if (mapPoints[x+(3*DISTANCE)][y-(1*DISTANCE)].sector==0)
    {
      *i=x+(3*DISTANCE);
      *j=y-(1*DISTANCE);
    }
  else if (mapPoints[x-(1*DISTANCE)][y+(3*DISTANCE)].sector==0)
    {
      *i=x-(1*DISTANCE);
      *j=y+(3*DISTANCE);
    }
  else if (mapPoints[x-(2*DISTANCE)][y+(2*DISTANCE)].sector==0)
    {
      *i=x-(2*DISTANCE);
      *j=y+(2*DISTANCE);
    }
  else if (mapPoints[x+(2*DISTANCE)][y+(2*DISTANCE)].sector==0)
    {
      *i=x+(2*DISTANCE);
      *j=y+(2*DISTANCE);
    }
  else if (mapPoints[x+(1*DISTANCE)][y+(3*DISTANCE)].sector==0)
    {
      *i=x+(1*DISTANCE);
      *j=y+(3*DISTANCE);
    }
  else if (mapPoints[x+(1*DISTANCE)][y-(3*DISTANCE)].sector==0)
    {
      *i=x+(1*DISTANCE);
      *j=y-(3*DISTANCE);
    }
  else if (mapPoints[x+(2*DISTANCE)][y-(2*DISTANCE)].sector==0)
    {
      *i=x+(2*DISTANCE);
      *j=y-(2*DISTANCE);
    }
  else if (mapPoints[x-(2*DISTANCE)][y-(2*DISTANCE)].sector==0)
    {
      *i=x-(2*DISTANCE);
      *j=y-(2*DISTANCE);
    }
  else if (mapPoints[x-(1*DISTANCE)][y-(3*DISTANCE)].sector==0)
    {
      *i=x-(1*DISTANCE);
      *j=y-(3*DISTANCE);
    }

  /* these are the even worse positions */

  else if (mapPoints[x+(3*DISTANCE)][y+(3*DISTANCE)].sector==0)
    {
      *i=x+(3*DISTANCE);
      *j=y+(3*DISTANCE);
    }
  else if (mapPoints[x+(2*DISTANCE)][y+(4*DISTANCE)].sector==0)
    {
      *i=x+(2*DISTANCE);
      *j=y+(4*DISTANCE);
    }
  else if (mapPoints[x-(2*DISTANCE)][y+(4*DISTANCE)].sector==0)
    {
      *i=x-(2*DISTANCE);
      *j=y+(4*DISTANCE);
    }
  else if (mapPoints[x-(3*DISTANCE)][y+(3*DISTANCE)].sector==0)
    {
      *i=x-(3*DISTANCE);
      *j=y+(3*DISTANCE);
    }
  else if (mapPoints[x+(2*DISTANCE)][y-(4*DISTANCE)].sector==0)
    {
      *i=x+(2*DISTANCE);
      *j=y-(4*DISTANCE);
    }
  else if (mapPoints[x+(3*DISTANCE)][y-(3*DISTANCE)].sector==0)
    {
      *i=x+(3*DISTANCE);
      *j=y-(3*DISTANCE);
    }
  else if (mapPoints[x-(3*DISTANCE)][y-(3*DISTANCE)].sector==0)
    {
      *i=x-(3*DISTANCE);
      *j=y-(3*DISTANCE);
    }
  else if (mapPoints[x-(2*DISTANCE)][y-(4*DISTANCE)].sector==0)
    {
      *i=x-(2*DISTANCE);
      *j=y-(4*DISTANCE);
    }
}


void drawMap(HDC hDC, HWND hwnd, long sector, int Offset)
{
	int width, height;
	int i=0, k=0, j=0, oneWayWarp=2;
	long warpTo=0;
	char buffer[10], *type;
	int isDeadEndSector;
	RECT rect;
    int count=0, pos=0, correctBubble=0;
	char *token, bubbleChar[MAX_LENGTH];
	char bubble_sectors[MAX_BUBBLE_SECTORS][10];
	int inBubble=0;

	GetClientRect(hwnd, &rect);

	rect.left=Offset;

	if (Offset>0)
	{
		rect.right=rect.right-10;
		rect.top=rect.top+10;
		rect.bottom=rect.bottom-10;
	}
    PaintRect(hDC, &rect, RGBWHITE);


	for (i=0; i<MAX_BUBBLE_SECTORS; i++)
	{
		strcpy (bubble_sectors[i], "");
	}

	count=0;
	for (i=0; bubbleCharDataStorage != NULL && i< (int) strlen(bubbleCharDataStorage) && correctBubble==0; i++)
	{
		if (bubbleCharDataStorage[i]==';')
		{
			bubbleChar[j]=0;
			pos=0;
			j=0;

			token = strtok(bubbleChar, ":");
			while (token != NULL)
			{
				strcpy (bubble_sectors[pos], token);
				token = strtok(NULL, ":");
				pos++;
			}	
			if (sector == atol(bubble_sectors[1]))
				correctBubble=1;
		}
		else
			bubbleChar[j++] = bubbleCharDataStorage[i];
	}

	width=rect.right;
	height=rect.bottom;

//	printf ("width - %ld : height - %ld\n", width, height);
	for (i=0; i<width/10; i++)
		for (k=0; k<height/10; k++)
		{
			if (mapPoints[i][k].sector != 0)
			{
				sprintf (buffer, "%ld", mapPoints[i][k].sector);
//				printf ("%s\n", buffer);
				
				type = getPortType(atol(buffer));

				inBubble=0;
				for (j=2; j<pos && inBubble==0; j++)
				{
					if (atol(bubble_sectors[j]) == atol(buffer))
						inBubble=1;
					else
						inBubble=0;
				}

				isDeadEndSector = isDeadEnd(mapPoints[i][k].sector);
				if (inBubble)
					PaintSector(hDC, (i*10)-(18), (k*10)-25, buffer, strlen (buffer), type, strlen (type), RGBBROWN, Offset, isDeadEndSector);
				else if (sector==mapPoints[i][k].sector)
					PaintSector(hDC, (i*10)-(18), (k*10)-25, buffer, strlen (buffer), type, strlen (type), RGBBROWN, Offset, isDeadEndSector);
				else
				{
					if (sectors[mapPoints[i][k].sector-1].ftr_owner==-1 || sectors[mapPoints[i][k].sector-1].ftrs==-1)
					{
						PaintSector(hDC, (i*10)-(18), (k*10)-25, buffer, strlen (buffer), type, strlen (type), RGBGRAY, Offset, isDeadEndSector);
					}
					else if (sectors[mapPoints[i][k].sector-1].ftr_owner==0 || (sectors[mapPoints[i][k].sector-1].ftr_owner-CORP_CONSTANT)==players[0].corpNumber)
					{
						PaintSector(hDC, (i*10)-(18), (k*10)-25, buffer, strlen (buffer), type, strlen (type), RGBGREEN, Offset, isDeadEndSector);
					}
					else if (isPlayerAnEnemy(sectors[mapPoints[i][k].sector-1].ftr_owner)==0)
					{
						PaintSector(hDC, (i*10)-(18), (k*10)-25, buffer, strlen (buffer), type, strlen (type), RGBBLUE, Offset, isDeadEndSector);
					}
					else if (isPlayerAnEnemy(sectors[mapPoints[i][k].sector-1].ftr_owner)==1)
					{
						PaintSector(hDC, (i*10)-(18), (k*10)-25, buffer, strlen (buffer), type, strlen (type), RGBRED, Offset, isDeadEndSector);
					}
					else
					{
						PaintSector(hDC, (i*10)-(18), (k*10)-25, buffer, strlen (buffer), type, strlen (type), RGBGRAY, Offset, isDeadEndSector);
					}
				}

				#ifdef P_FREE
					pfree(&type);           // free present block        
				#else
					free(type);           // free present block        
				#endif		

				// here we draw the lines, we iterate 6 times since there
				//   are 6 warps :) 

				// get the first warp 
				warpTo = mapPoints[i][k].warpOne;
				
				for (j=0; j<=6 && (warpTo != 0) && (mapLines[warpTo-1].x != 0)  && (mapLines[warpTo-1].y !=0); j++)
				{
					// see if it a one way warp
					oneWayWarp = isOneWayWarp(mapPoints[i][k].sector, warpTo);
				

					 // draw the lines. there are 3 types of lines. positive lines,
					 //  negative lines and lines that are 0 :)
					 //  for each of those, there is a case so we can make the line
					 //  touch the circle on the outside 
					if ((((mapLines[warpTo-1].x)*10)-((mapLines[mapPoints[i][k].sector-1].x)*10))==0)
					  {
						if (mapLines[warpTo-1].y > mapLines[mapPoints[i][k].sector-1].y)
						  {
							DrawArrow(hDC, ((mapLines[mapPoints[i][k].sector-1].x)*10),
										   ((mapLines[mapPoints[i][k].sector-1].y)*10)+(int) (CIRCLE_RADIUS/1.5),
										   ((mapLines[warpTo-1].x)*10),
										   ((mapLines[warpTo-1].y)*10)-(int)(CIRCLE_RADIUS/1.5), 
										   POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);
						  }
						else
						  {
							DrawArrow(hDC, ((mapLines[mapPoints[i][k].sector-1].x)*10),
										   ((mapLines[mapPoints[i][k].sector-1].y)*10)-(int)(CIRCLE_RADIUS/1.5),
										   ((mapLines[warpTo-1].x)*10),
										   ((mapLines[warpTo-1].y)*10)+(int)(CIRCLE_RADIUS/1.5), 
										   POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);
						  }
						
					  }
					else if ((((mapLines[warpTo-1].y)*10)-((mapLines[mapPoints[i][k].sector-1].y)*10))==0)
					{
						if (mapLines[warpTo-1].x > mapLines[mapPoints[i][k].sector].x)
						{
							DrawArrow(hDC,  ((mapLines[mapPoints[i][k].sector-1].x)*10)+(int) (CIRCLE_RADIUS/1.5),
											((mapLines[mapPoints[i][k].sector-1].y)*10),
											((mapLines[warpTo-1].x)*10)-(int)(CIRCLE_RADIUS/2.3),
											((mapLines[warpTo-1].y)*10), 
										    POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);
						}
						else
						{
							DrawArrow(hDC,  ((mapLines[mapPoints[i][k].sector-1].x)*10)-(int)(CIRCLE_RADIUS/2.3),
											((mapLines[mapPoints[i][k].sector-1].y)*10),
											((mapLines[warpTo-1].x)*10)+(int)(CIRCLE_RADIUS/1.5),
											((mapLines[warpTo-1].y)*10), 
										    POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);
						}
					}
					else if ((double)((double)(((mapLines[warpTo-1].y)*10)-((mapLines[mapPoints[i][k].sector-1].y)*10))/(double)(((mapLines[warpTo-1].x)*10)-((mapLines[mapPoints[i][k].sector-1].x)*10)))>0.0)
					{
						if (mapLines[mapPoints[i][k].sector-1].x > mapLines[warpTo-1].x)
						{  
							DrawArrow(hDC,  (((mapLines[mapPoints[i][k].sector-1].x)*10)-(int)(CIRCLE_RADIUS/2.6)),
										    (((mapLines[mapPoints[i][k].sector-1].y)*10)-(int)(CIRCLE_RADIUS/2.6)),
										    (((mapLines[warpTo-1].x)*10)+(int)(CIRCLE_RADIUS/1.9)),
										    (((mapLines[warpTo-1].y)*10)+(int)(CIRCLE_RADIUS/1.9)), 
											POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);			
						}
						else
						{
							DrawArrow(hDC, (((mapLines[mapPoints[i][k].sector-1].x)*10)+ (int)(CIRCLE_RADIUS/1.9)),
										   (((mapLines[mapPoints[i][k].sector-1].y)*10)+(int) (CIRCLE_RADIUS/1.9)),
										   (((mapLines[warpTo-1].x)*10)-(int)(CIRCLE_RADIUS/2.6)),
										   (((mapLines[warpTo-1].y)*10)-(int)(CIRCLE_RADIUS/2.6)), 
											POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);
						}
					}
			
					else if ((double)((double)(((mapLines[warpTo-1].y)*10)-((mapLines[mapPoints[i][k].sector-1].y)*10))/(double)(((mapLines[warpTo-1].x)*10)-((mapLines[mapPoints[i][k].sector].x)*10)))<0.0)
					{
						if (mapLines[mapPoints[i][k].sector-1].x > mapLines[warpTo-1].x)
						{
							DrawArrow(hDC, (((mapLines[mapPoints[i][k].sector-1].x)*10)-(int)(CIRCLE_RADIUS/2.6)),
										   (((mapLines[mapPoints[i][k].sector-1].y)*10)+(int)(CIRCLE_RADIUS/2.6)),
										   (((mapLines[warpTo-1].x)*10)+(int)(CIRCLE_RADIUS/1.9)),
										   (((mapLines[warpTo-1].y)*10)-(int)(CIRCLE_RADIUS/1.9)), 
											POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);
						}
						else
						{
							DrawArrow(hDC, (((mapLines[mapPoints[i][k].sector-1].x)*10)+(int)(CIRCLE_RADIUS/1.9)),
										   (((mapLines[mapPoints[i][k].sector-1].y)*10)-(int)(CIRCLE_RADIUS/1.9)),
										   (((mapLines[warpTo-1].x)*10)-(int)(CIRCLE_RADIUS/2.6)),
										   (((mapLines[warpTo-1].y)*10)+(int)(CIRCLE_RADIUS/2.6)), 
											POS_BOTTOM, POS_TOP, 1, 1, Offset, oneWayWarp);
						}
					}

					// decide which warp to use to build the line
					if (j==1)
						warpTo = mapPoints[i][k].warpTwo;
					else if (j==2)
						warpTo = mapPoints[i][k].warpThree;
					else if (j==3)
						warpTo = mapPoints[i][k].warpFour;
					else if (j==4)
						warpTo = mapPoints[i][k].warpFive;
					else if (j==5)
						warpTo = mapPoints[i][k].warpSix;
				}
			}
		}
}

int isDeadEnd (long sector)
{
  long warpOne=0, warpTwo=0;

  if (isValidSectorOneOffset(sector)){
	  warpOne = sectors[sector-1].warp_sect[0];
	  warpTwo = sectors[sector-1].warp_sect[1];

	  /* does it have one warp one */
	  if (warpOne !=0 && warpTwo==0)
		{
		  /* make sure it is not a one way */
		  if (isValidSectorOneOffset(warpOne) && 
			  (sectors[warpOne-1].warp_sect[0]==sector ||
			  sectors[warpOne-1].warp_sect[1]==sector ||
			  sectors[warpOne-1].warp_sect[2]==sector ||
			  sectors[warpOne-1].warp_sect[3]==sector ||
			  sectors[warpOne-1].warp_sect[4]==sector ||
			  sectors[warpOne-1].warp_sect[5]==sector))
			return (1);
		}
  }
  return (0);
}

char * getPortType (long sect)
{
	char *type=NULL;
	
	if (sect>0 && sect<=header.sector_cnt) {
		if(isValidSectorOneOffset(sect) && sectors[sect-1].info==1)
		  type = (char *) strdup ("BBS");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==2)
		  type = (char *) strdup ("BSB");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==3)
		  type = (char *) strdup ("SBB");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==4)
		  type = (char *) strdup ("SSB");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==5)
		  type = (char *) strdup ("SBS");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==6)
		  type = (char *) strdup ("BSS");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==7)
		  type = (char *) strdup ("SSS");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==8)
		  type = (char *) strdup ("BBB");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==9)
		  type = (char *) strdup ("SD");
		else if(isValidSectorOneOffset(sect) && sectors[sect-1].info==0)
		  type = (char *) strdup ("Cls 0");
		else 
		  type = (char *) strdup (" ");
	} else {
		type = (char *) strdup (" ");
	}
	return (type);
}

long getClickMapSector (int x, int y)
{
	int newX, newY, i, j;

	newX = x/10;
	newY = y/10;

	for (i=newX-2; i< newX+2; i++)
		for (j=newY-2; j< newY+2; j++)
		{
			if (mapPoints[i][j].sector != 0)
			{
				return (mapPoints[i][j].sector);
			}
		}
	return 0;

}
