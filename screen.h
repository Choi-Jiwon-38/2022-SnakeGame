#ifndef __SCREEN__
#define __SCREEN__
#include "init.h"

int MissionBody[4] = {4, 4, 5, 6};
int MissionApple[4] = {1, 2, 2, 3};
int MissionPosion[4] = {1, 2, 3, 3};
int MissionGate[4] = {1, 1, 2, 3};

// 게임 Start 화면
void ScreenStart()
{
    initscr();
    start_color();
    resize_term(HEIGHT_GB + 4, WIDTH_GB * 2);
    mvprintw(11, 15,  "2022 CPP - SNAKE GAME");
    mvprintw(14, 15,  " 20193216 / 20213091 ");
    mvprintw(17, 15, "    PRESS ANY KEY    ");
    border('#', '#', '#', '#', '#', '#', '#', '#');
    refresh();
    getch();
    clear();
    endwin();
}


// Stage 설명 화면
void ScreenLoading(int s)
{
    if (s == 5)
    {
        LoadingScreen = newwin(HEIGHT_GB + 4, WIDTH_GB * 2, 0, 0);
        mvwprintw(LoadingScreen, 11, 17, "    GAME CLEAR   ");
        mvwprintw(LoadingScreen, 14, 17, " CONGURATULATIONS");
        mvwprintw(LoadingScreen, 17, 17, "THANKS FOR PLAYING");
        wborder(LoadingScreen, '#', '#', '#', '#', '#', '#', '#', '#');
        wrefresh(LoadingScreen);
        getch();
        delwin(LoadingScreen);
    }
    else
    {
        LoadingScreen = newwin(HEIGHT_GB + 4, WIDTH_GB * 2, 0, 0);
        mvwprintw(LoadingScreen, 11, 12,  "            STAGE %d          ", s);
        mvwprintw(LoadingScreen, 14, 12, "    WE HAVE TO ACCOMPLISH    ");
        mvwprintw(LoadingScreen, 17, 12, "B (%d) / + (%d) / - (%d) / G (%d)", MissionBody[s - 1], MissionApple[s - 1], MissionPosion[s - 1], MissionGate[s - 1]);
        wborder(LoadingScreen, '#', '#', '#', '#', '#', '#', '#', '#');
        wrefresh(LoadingScreen);
    }
}

#endif