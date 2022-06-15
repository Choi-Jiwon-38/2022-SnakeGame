#ifndef __INIT__
#define __INIT__

#include <vector>
#include <ncurses.h>

#define WIDTH_GB 25        // GameBoard 가로 길이
#define HEIGHT_GB 25       // GameBoard 세로 길이
#define WIDTH_SMB 19       // ScoreBoard, MissionBoard 가로 길이
#define HEIGHT_SMB 12      // ScoreBoard, MissionBoard 세로 길이

using namespace std;

int num_of_item = 0;
int total_of_growth = rand() % 3 + 1;
int num_of_growth = 0;

int x = 11, y = 12;

vector<pair<int, int> > snake;

char map[25][25];

bool running = true;

WINDOW *GameBoard;
WINDOW *ScoreBoard;
WINDOW *MissionBoard;
WINDOW *LoadingScreen;

static int key = KEY_LEFT; 

#endif