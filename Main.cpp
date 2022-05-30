// 20213091 최지원

#include <iostream>
#include <ncurses.h>

#define WIDTH_GB 25        // GameBoard 가로 길이
#define HEIGHT_GB 25       // GameBoard 세로 길이
#define WIDTH_SMB 19       // ScoreBoard, MissionBoard 가로 길이
#define HEIGHT_SMB 12      // ScoreBoard, MissionBoard 세로 길이

using namespace std;


int main()
{
    WINDOW *GameBoard;
    WINDOW *ScoreBoard;
    WINDOW *MissionBoard;

    initscr();                                  // Curses 모드 시작

    curs_set(0);                                // 커서 제거 (커서 깜빡임)

    resize_term(HEIGHT_GB + 4, WIDTH_GB * 2);
    start_color();                              // Color 사용 선언 <- 사용 전에 무조건 선언!
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_BLUE);

    mvprintw(1, 2, "GAME BOARD");
    mvprintw(1, 29, "SCORE BOARD");
    mvprintw(14, 29, "MISSION BOARD");

    bkgd(COLOR_PAIR(2));
    border('#', '#', '#', '#', '#', '#', '#', '#');
    refresh();

    GameBoard = newwin(HEIGHT_GB, WIDTH_GB, 2, 2); // 행 크기, 열 크기, 시작 y좌표, 시작 x좌표
    wborder(GameBoard, '@', '@', '@', '@', '@', '@', '@', '@');
    wbkgd(GameBoard, COLOR_PAIR(3));

    ScoreBoard = newwin(HEIGHT_SMB - 1, WIDTH_SMB, 2, 29);
    wbkgd(ScoreBoard, COLOR_PAIR(4));

    MissionBoard = newwin(HEIGHT_SMB, WIDTH_SMB, 15, 29);
    wbkgd(MissionBoard, COLOR_PAIR(4));

    wrefresh(GameBoard);
    wrefresh(ScoreBoard);
    wrefresh(MissionBoard);

    getch();
    endwin();

    return 0;
}