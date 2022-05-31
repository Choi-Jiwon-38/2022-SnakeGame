#include <fstream>
#include <thread>
#include "init.h"
#include "Item.h"
#include "gate.h"

int tmp_x, tmp_y;

void LoadStage(int stage)
{
    ifstream fin;
    fin.open("data/map/stage_" + to_string(stage) + ".txt");

    char temp[WIDTH_GB + 1];
    for (int i = 0; i < WIDTH_GB; i++)
    {
        fin.getline(temp, 26);
        for (int j = 0; j < HEIGHT_GB; j++)
        {
            map[i][j] = temp[j];
        }
    }
    fin.close();
}
void keyControl()
{
    int input;

    while(running)
    {
        input = getch();

        if(input == KEY_UP || input == KEY_DOWN || input == KEY_LEFT || input == KEY_RIGHT)
        {
        // 입력 진행과 반대 방향의 키를 입력하면 게임 종료
            if((input == KEY_UP && key == KEY_DOWN) || (input == KEY_DOWN && key == KEY_UP)
                || (input == KEY_LEFT && key == KEY_RIGHT) || (input == KEY_RIGHT && key == KEY_LEFT))
                {
                    running = false;
                }
            key = input;
        }
        else if(input == 27)
        {
            running = false;
        }
    }
}
void using_color()
{
    start_color();                              // Color 사용 선언 <- 사용 전에 무조건 선언!
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_BLUE);
    init_pair(5, COLOR_GREEN, COLOR_GREEN);
    init_pair(6, COLOR_RED, COLOR_RED);
}
void init()
{
    initscr();                                  // Curses 모드 시작
    keypad(stdscr, TRUE);
    curs_set(0);                                // 커서 제거 (커서 깜빡임)
    noecho();
    timeout(0);
    resize_term(HEIGHT_GB + 4, WIDTH_GB * 2);
    
    using_color();

    // Snake 기본 설정
    map[y][x] = 3;
    snake.push_back(pair<int, int>(y, x + 1));
    snake.push_back(pair<int, int>(y, x + 2));

    for (int i = 0; i < snake.size(); i++)
    {
        map[snake[i].first][snake[i].second] = '4';
    }
}


void init_draw()
{
    delwin(GameBoard);
    delwin(ScoreBoard);
    delwin(MissionBoard);
    endwin();

    mvprintw(1, 2, "GAME BOARD");
    mvprintw(1, 29, "SCORE BOARD");
    mvprintw(14, 29, "MISSION BOARD");

    bkgd(COLOR_PAIR(2));
    border('#', '#', '#', '#', '#', '#', '#', '#');
    refresh();

    create_item();

    GameBoard = newwin(HEIGHT_GB, WIDTH_GB, 2, 2); // 행 크기, 열 크기, 시작 y좌표, 시작 x좌표
    for(int i = 0; i < HEIGHT_GB; i++)
    {
        for(int j = 0; j < WIDTH_GB; j++)
        {
            switch (map[i][j])
            {
                case '0':
                    mvwaddch(GameBoard, i, j, ' ' | COLOR_PAIR(1));
                    break;
                case '1':
                    mvwaddch(GameBoard, i, j, '#' | COLOR_PAIR(5));
                    break;
                case '2':
                    mvwaddch(GameBoard, i, j, '*' | COLOR_PAIR(4));
                    break;
                case '3':
                    mvwaddch(GameBoard, i, j, '*' | COLOR_PAIR(6));
                    break;
                case '4':
                    mvwaddch(GameBoard, i, j, '-' | COLOR_PAIR(6));
                    break;
                case '5':
                    mvwaddch(GameBoard, i, j, 'G' | COLOR_PAIR(6));
                    break;
                case '6':
                    mvwaddch(GameBoard, i, j, 'P' | COLOR_PAIR(6));
                    break;
            }
        }
        wrefresh(GameBoard);
    }


    ScoreBoard = newwin(HEIGHT_SMB - 1, WIDTH_SMB, 2, 29);
    mvwprintw(ScoreBoard, 2, 2, "B : 6");
    mvwprintw(ScoreBoard, 4, 2, "+ : 0");
    mvwprintw(ScoreBoard, 6, 2, "- : 0");
    mvwprintw(ScoreBoard, 8, 2, "G : 0");

    wbkgd(ScoreBoard, COLOR_PAIR(4));

    MissionBoard = newwin(HEIGHT_SMB, WIDTH_SMB, 15, 29);
    wbkgd(MissionBoard, COLOR_PAIR(4));

    refresh();
    wrefresh(GameBoard);
    wrefresh(ScoreBoard);
    wrefresh(MissionBoard);
}
void collision()
{   
    for (int i = 0; i < itemList.size(); i++)   // 아이템 충돌
    {
        if (y == itemList[i].first && x == itemList[i].second)
        {
            if (map[y][x] == '5')
            {
                snake.push_back(pair<int, int> (tmp_y, tmp_x));
                itemList.erase(itemList.begin() + i);
                break;
            }
            else if (map[y][x] == '6')
            {
                snake.pop_back();
                itemList.erase(itemList.begin() + i);
                break;
            }
        }
    }

    if (map[x][y] == '1' || map[x][y] == '2' || map[x][y] == '4')
    {
        running = false;
    }
}


int main()
{
    LoadStage(0);

    init();
    init_draw();

    thread key_thread(keyControl);
    create_gate();

    while (running)
    {  
        map[y][x] = '0';
        for (int i = 0; i < snake.size(); i++)
        {
            map[snake[i].first][snake[i].second] = '0';
        }

        tmp_y = snake[snake.size() - 1].first;
        tmp_x = snake[snake.size() - 1].second;

        for(int i = snake.size() - 1; i > 0; i--)
        {
            snake[i] = snake[i-1];
        }
        snake[0].first = y;
        snake[0].second = x;

        if (key == KEY_LEFT)
            x--;
        else if (key == KEY_RIGHT)
            x++;
        else if (key == KEY_UP)
            y--;
        else if (key == KEY_DOWN)
            y++;

        remove_item();
        
        collision();

        map[y][x] = '3';

        for (int i = 0; i < snake.size(); i++)
        {
            map[snake[i].first][snake[i].second] = '4';
        }

        refresh();

        

        GameBoard = newwin(HEIGHT_GB, WIDTH_GB, 2, 2); // 행 크기, 열 크기, 시작 y좌표, 시작 x좌표

        for(int i = 0; i < HEIGHT_GB; i++)
        {
            for(int j = 0; j < WIDTH_GB; j++)
            {
                switch (map[i][j])
                {
                    case '0':
                        mvwaddch(GameBoard, i, j, ' ' | COLOR_PAIR(1));
                        break;
                    case '1':
                        mvwaddch(GameBoard, i, j, '#' | COLOR_PAIR(5));
                        break;
                    case '2':
                        mvwaddch(GameBoard, i, j, '*' | COLOR_PAIR(4));
                        break;
                    case '3':
                        mvwaddch(GameBoard, i, j, '*' | COLOR_PAIR(6));
                        break;
                    case '4':
                        mvwaddch(GameBoard, i, j, '-' | COLOR_PAIR(6));
                        break;
                    case '5':
                        mvwaddch(GameBoard, i, j, 'G' | COLOR_PAIR(6));
                        break;
                    case '6':
                        mvwaddch(GameBoard, i, j, 'P' | COLOR_PAIR(6));
                        break;
                }
            }
            wrefresh(GameBoard);
        }

        this_thread::sleep_for(chrono::milliseconds(200));
    }
    if ( key_thread.joinable() )
        key_thread.join();

    return 0;
}