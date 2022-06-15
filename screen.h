#ifndef __SCREEN__
#define __SCREEN__
#include "init.h"

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
void ScreenLoading(int stage)
{
    LoadingScreen = newwin(HEIGHT_GB + 4, WIDTH_GB * 2, 0, 0);
    mvwprintw(LoadingScreen, 11, 12,  "            STAGE 1          ");
    mvwprintw(LoadingScreen, 14, 12, "    WE HAVE TO ACCOMPLISH    ");
    mvwprintw(LoadingScreen, 17, 12, "B (3) / + (3) / - (3) / G (3)");
    wborder(LoadingScreen, '#', '#', '#', '#', '#', '#', '#', '#');
    wrefresh(LoadingScreen);
}

// 게임 Clear 화면
#endif