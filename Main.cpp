#include <fstream>
#include <thread>
#include "init.h"
#include "Item.h"
#include "gate.h"
#include "stage.h"

int tmp_x, tmp_y;

// ScoreBoatd에 갱신할 것들
int countGrowth = 0;
int countPosion = 0;
int countGate = 0;
int MaxSize = 3;

bool flag = false;

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
        if (flag == false)
        {
            input = getch();

            if(input == KEY_UP || input == KEY_DOWN || input == KEY_LEFT || input == KEY_RIGHT)
            {
                if((input == KEY_UP && key == KEY_DOWN) || (input == KEY_DOWN && key == KEY_UP)
                    || (input == KEY_LEFT && key == KEY_RIGHT) || (input == KEY_RIGHT && key == KEY_LEFT))
                    {
                        running = false;
                    }
                key = input;
            }
            else if(input == 27)                // ESC 버튼을 눌렀을 경우
            {
                running = false;
            }
        }
        else
        {
            flag = false;
            continue;
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
    map[y][x] = '3';
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
    mvwprintw(ScoreBoard, 2, 2, "B : 3 / 3");
    mvwprintw(ScoreBoard, 4, 2, "+ : %d", countGrowth);
    mvwprintw(ScoreBoard, 6, 2, "- : %d", countPosion);
    mvwprintw(ScoreBoard, 8, 2, "G : %d", countGate);

    wbkgd(ScoreBoard, COLOR_PAIR(4));

    MissionBoard = newwin(HEIGHT_SMB, WIDTH_SMB, 15, 29);
    mvwprintw(MissionBoard, 2, 2, "B : %d (%c)", MissionBody[0], ' ');
    mvwprintw(MissionBoard, 4, 2, "+ : %d (%c)", MissionApple[0], ' ');
    mvwprintw(MissionBoard, 6, 2, "- : %d (%c)", MissionPosion[0], ' ');
    mvwprintw(MissionBoard, 8, 2, "G : %d (%c)", MissionGate[0], ' ');
    wbkgd(MissionBoard, COLOR_PAIR(4));

    refresh();
    wrefresh(GameBoard);
    wrefresh(ScoreBoard);
    wrefresh(MissionBoard);
}
void change_dir_pos(int dir, int new_x, int new_y)
{
    x = new_x;
    y = new_y;
    key = dir;
}

int gate_count, gate_snake_size;
bool flag_gateUsing = false;

void using_gate()
{
    int tp_x, tp_y;

    if (y == gate_pos[0].first && x == gate_pos[0].second)
    {
        tp_y = gate_pos[1].first;
        tp_x = gate_pos[1].second;
        flag = true;
    }
    else if (y == gate_pos[1].first && x == gate_pos[1].second)
    {
        tp_y = gate_pos[0].first;
        tp_x = gate_pos[0].second;
        flag = true;
    }

    if (flag)
    {   
        countGate++;
        flag_gateUsing = true;
        gate_count = 0;
        gate_snake_size = snake.size();

        if (tp_x == 0)
        {   
            change_dir_pos(KEY_RIGHT, tp_x + 1, tp_y);
        }
        else if (tp_x == WIDTH_GB - 1)
        {
            change_dir_pos(KEY_LEFT, tp_x - 1, tp_y);
        }
        else if (tp_y == 0)
        {
            change_dir_pos(KEY_DOWN, tp_x ,tp_y + 1);
        }
        else if (tp_y == HEIGHT_GB - 1)
        {  
            change_dir_pos(KEY_UP, tp_x, tp_y - 1);
        }
        else
        {
            if (key == KEY_RIGHT)
            {
                if (map[tp_y][tp_x + 1] != '1' && map[tp_y][tp_x + 1] != '2') { change_dir_pos(KEY_RIGHT, tp_x + 1, tp_y); }
                else if (map[tp_y + 1][tp_x] != '1' && map[tp_y + 1][tp_x] != '2') { change_dir_pos(KEY_DOWN, tp_x, tp_y + 1); }
                else if (map[tp_y - 1][tp_x] != '1' && map[tp_y - 1][tp_x] != '2') { change_dir_pos(KEY_UP, tp_x, tp_y - 1); }
                else if (map[tp_y - 1][tp_x] != '1' && map[tp_y - 1][tp_x] != '2') { change_dir_pos(KEY_LEFT, tp_x - 1, tp_y); }
            }
            else if (key == KEY_DOWN)
            {
                if (map[tp_y + 1][tp_x] != '1' && map[tp_y + 1][tp_x] != '2') { change_dir_pos(KEY_DOWN, tp_x, tp_y + 1); }
                else if (map[tp_y][tp_x - 1] != '1' && map[tp_y][tp_x - 1] != '2') { change_dir_pos(KEY_LEFT, tp_x - 1, tp_y); }
                else if (map[tp_y][tp_x + 1] != '1' && map[tp_y][tp_x + 1] != '2') { change_dir_pos(KEY_RIGHT, tp_x + 1, tp_y); }
                else if (map[tp_y - 1][tp_x] != '1' && map[tp_y - 1][tp_x] != '2') { change_dir_pos(KEY_UP, tp_x, tp_y - 1); }
            }
            else if (key == KEY_LEFT)
            {
                if (map[tp_y][tp_x - 1] != '1' && map[tp_y][tp_x - 1] != '2') { change_dir_pos(KEY_LEFT, tp_x - 1, tp_y); }
                else if (map[tp_y - 1][tp_x] != '1' && map[tp_y - 1][tp_x] != '2') { change_dir_pos(KEY_UP, tp_x, tp_y - 1); }
                else if (map[tp_y + 1][tp_x] != '1' && map[tp_y + 1][tp_x] != '2') { change_dir_pos(KEY_DOWN, tp_x, tp_y + 1); }
                else if (map[tp_y][tp_x + 1] != '1' && map[tp_y][tp_x + 1] != '2') { change_dir_pos(KEY_RIGHT, tp_x + 1, tp_y); }
            }
            else if (key == KEY_UP)
            {
                if (map[tp_y - 1][tp_x] != '1' && map[tp_y - 1][tp_x] != '2') { change_dir_pos(KEY_UP, tp_x, tp_y - 1); }
                else if (map[tp_y][tp_x + 1] != '1' && map[tp_y][tp_x + 1] != '2') { change_dir_pos(KEY_RIGHT, tp_x + 1, tp_y); }
                else if (map[tp_y][tp_x + 1] != '1' && map[tp_y][tp_x - 1] != '2') { change_dir_pos(KEY_LEFT, tp_x - 1, tp_y); }
                else if (map[tp_y + 1][tp_x] != '1' && map[tp_y + 1][tp_x] != '2') { change_dir_pos(KEY_DOWN, tp_x, tp_y + 1); }
            }
        }   
    }

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
                countGrowth++;
                break;
            }
            else if (map[y][x] == '6')
            {
                snake.pop_back();
                itemList.erase(itemList.begin() + i);
                countPosion++;
                if (snake.size() < 2)
                {
                    running = false;
                }
                break;
            }
        }
    }
    using_gate();

    if (flag_gateUsing)
    {
        if (gate_count > gate_snake_size)
        {
        
            map[gate_pos[0].first][gate_pos[0].second] = '1';
            map[gate_pos[1].first][gate_pos[1].second] = '1';

            gate_pos.pop_back();
            gate_pos.pop_back();

            flag_gateUsing = false;

            create_gate();
        }
        gate_count++;
    }
}

int main()
{
    LoadStage(3);

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

        remove_item();

        if (key == KEY_LEFT)
            x--;
        else if (key == KEY_RIGHT)
            x++;
        else if (key == KEY_UP)
            y--;
        else if (key == KEY_DOWN)
            y++;
        

        if (map[y][x] == '1')
        {
            running = false;
            break;
        }
        collision();

        map[y][x] = '3';

        for(int i = 0; i < snake.size(); i++)
            {
            if (y == snake[i].first && x == snake[i].second)
            {
                running = false;
                break;
            }
        }

        for (int i = 0; i < snake.size(); i++)
        {
            map[snake[i].first][snake[i].second] = '4';
        }

        refresh();

        GameBoard = newwin(HEIGHT_GB, WIDTH_GB, 2, 2); // 행 크기, 열 크기, 시작 y좌표, 시작 x좌표

        for (int i = 0; i < HEIGHT_GB; i++)
        {
            for (int j = 0; j < WIDTH_GB; j++)
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
                        mvwaddch(GameBoard, i, j, '/' | COLOR_PAIR(4));
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

        if (snake.size() + 1 >= MaxSize)
        {
            MaxSize = snake.size() + 1;
        } 
        ///
        ScoreBoard = newwin(HEIGHT_SMB - 1, WIDTH_SMB, 2, 29);
        mvwprintw(ScoreBoard, 2, 2, "B : %d / %d", snake.size() + 1, MaxSize);
        mvwprintw(ScoreBoard, 4, 2, "+ : %d", countGrowth);
        mvwprintw(ScoreBoard, 6, 2, "- : %d", countPosion);
        mvwprintw(ScoreBoard, 8, 2, "G : %d", countGate);

        wbkgd(ScoreBoard, COLOR_PAIR(4));
        ///

        ///
        MissionBoard = newwin(HEIGHT_SMB, WIDTH_SMB, 15, 29);
        if (MaxSize >= MissionBody[0])
        {
            mvwprintw(MissionBoard, 2, 2, "B : %d (%c)", MissionBody[0], 'V');
        }
        else
        {
            mvwprintw(MissionBoard, 2, 2, "B : %d (%c)", MissionBody[0], ' ');
        }

        if (countGrowth >= MissionApple[0])
        {
            mvwprintw(MissionBoard, 4, 2, "+ : %d (%c)", MissionApple[0], 'V');
        }
        else 
        {
            mvwprintw(MissionBoard, 4, 2, "+ : %d (%c)", MissionApple[0], ' ');
        }

        if (countPosion >= MissionPosion[0])
        {
            mvwprintw(MissionBoard, 6, 2, "- : %d (%c)", MissionPosion[0], 'V');
        }
        else
        {
            mvwprintw(MissionBoard, 6, 2, "- : %d (%c)", MissionPosion[0], ' ');
        }

        if (countGate >= MissionGate[0])
        {
            mvwprintw(MissionBoard, 8, 2, "G : %d (%c)", MissionGate[0], 'V');
        }
        else
        {
            mvwprintw(MissionBoard, 8, 2, "G : %d (%c)", MissionGate[0], ' ');
        }
        wbkgd(MissionBoard, COLOR_PAIR(4));

        refresh();
        wrefresh(ScoreBoard);
        wrefresh(MissionBoard);
        ///

        this_thread::sleep_for(chrono::milliseconds(200));
    }

    if ( key_thread.joinable() )
    {
        key_thread.join();
    }
    
    return 0;
}