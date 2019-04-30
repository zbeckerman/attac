#ifndef __map_h__
#define __map_h__

#define DISTANCE 5
#define STARTING_X_GRID 25
#define STARTING_Y_GRID 28
#define GRID_SIZE_X 150
#define GRID_SIZE_Y 150
#define CIRCLE_RADIUS 40

extern MAP_POINTS mapPoints[GRID_SIZE_X][GRID_SIZE_Y];
extern MAP_LINES *mapLines;

void buildMapDataStructure(HWND hwnd, long sector, int maxLevels, int height, int width);
void loadMap(long sector, int maxLevel, int parentX, int parentY);
void getNextValidXY(int, int, int*, int*);
void drawMap(HDC hDC, HWND hwnd, long sector, int Offset);
char * getPortType (long sect);
int isDeadEnd (long sector);
long getClickMapSector (int x, int y);
void DrawArrow(HDC hDC, int startX, int startY, int endX, int endY, int startPos, int endPos, int startArrow, int endArrow, int offSet, int oneWayWarp);


#endif
