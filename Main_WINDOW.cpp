#include <fstream>
#include <thread>
#include "init.h"
#include "Item.h"
#include "gate.h"
#include "screen.h"
// #include <unistd.h> // <- MAC OS 전용
#include <windows.h> // <- WINDOW 전용

int tmp_x, tmp_y;           // Snake의 tail이 될 후보 (Growth 아이템 먹을 때)

int countGrowth = 0;        // ScoreBoard에 갱신할 것들
int countPosion = 0;
int countGate = 0;
int MaxSize = 3;

int gate_count, gate_snake_size;
int stage = 1;

bool isClear = true;
bool flag_gateUsing = false;
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

void keyControl() // 키 입력 관리 (비동기 입력 / 스네이크 진행 방향)
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
                delwin(LoadingScreen);
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

void using_color() // Curses에 사용될 color pair 호출
{
    start_color();                              // Color 사용 선언 <- 사용 전에 무조건 선언!
    init_pair(1, COLOR_WHITE, COLOR_WHITE);     // 빈 공간
    init_pair(2, COLOR_WHITE, COLOR_BLACK);     // GameBoard 테두리, 글자
    init_pair(3, COLOR_WHITE, COLOR_YELLOW);    // Snake Body, Head
    init_pair(4, COLOR_WHITE, COLOR_BLUE);      // Immune Wall
    init_pair(5, COLOR_WHITE, COLOR_GREEN);     // Wall
    init_pair(6, COLOR_WHITE, COLOR_RED);       // Item
    init_pair(7, COLOR_BLACK, COLOR_WHITE);     // MissionBoard, ScoreBoard
}

void init() // SnakeGame 구동을 위한 초기화 작업
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

void init_draw() //
{
    resize_term(HEIGHT_GB + 4, WIDTH_GB * 2);
    
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
                    mvwaddch(GameBoard, i, j, '*' | COLOR_PAIR(3));
                    break;
                case '4':
                    mvwaddch(GameBoard, i, j, '-' | COLOR_PAIR(3));
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

    wbkgd(ScoreBoard, COLOR_PAIR(7));

    MissionBoard = newwin(HEIGHT_SMB, WIDTH_SMB, 15, 29);
    mvwprintw(MissionBoard, 2, 2, "B : %d (%c)", MissionBody[0], ' ');
    mvwprintw(MissionBoard, 4, 2, "+ : %d (%c)", MissionApple[0], ' ');
    mvwprintw(MissionBoard, 6, 2, "- : %d (%c)", MissionPosion[0], ' ');
    mvwprintw(MissionBoard, 8, 2, "G : %d (%c)", MissionGate[0], ' ');
    wbkgd(MissionBoard, COLOR_PAIR(7));

    refresh();
    wrefresh(GameBoard);
    wrefresh(ScoreBoard);
    wrefresh(MissionBoard);
}

void change_dir_pos(int dir, int new_x, int new_y) //  using_gate 함수에서 사용
{
    x = new_x;
    y = new_y;
    key = dir;
}

void using_gate() // 게이트 입구 -> 출구 동작
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

void collision() // 아이템 충돌 & 게이트 출입 감지
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
            countGate++;

            create_gate();
        }
        gate_count++;
    }
}

void initSetting() // 다음 Stage로 넘어가기 전, 변수 초기화
{
    isClear = true;
    stage++;       
    
    countGate = 0;              // Score와 Mission에 사용된 카운터 초기화
    countGrowth = 0;
    countPosion = 0;

    x = 11, y = 12;             //  스네이크 시작 위치로 재설정

    key = KEY_LEFT;             //  스네이크 초기 방향(LEFT)으로 재설정

    while (snake.size() > 0)    // 이전 스테이지에 남아있는 스네이크의 body 제거
    {
        snake.pop_back();
    }
    MaxSize = 3;

    map[gate_pos[0].first][gate_pos[0].second] = '1'; // 이전 스테이지에 존재하던 Gate 제거
    map[gate_pos[1].first][gate_pos[1].second] = '1';

    gate_pos.pop_back();
    gate_pos.pop_back();
}

int main()
{
    ScreenStart();

    while ((stage < 6))
    {
        if (isClear == false)
        {
            break;
        }
        running = true;
        isClear = false;

        LoadStage(stage - 1);
        ScreenLoading(stage);

        if (stage == 5)
        {
            Sleep(2000); // WINDOW
            // sleep(2); // MAX
            return 0;
        }

        Sleep(2000); // WINDOW
        // sleep(2); // MAC

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
                            mvwaddch(GameBoard, i, j, '*' | COLOR_PAIR(3));
                            break;
                        case '4':
                            mvwaddch(GameBoard, i, j, '-' | COLOR_PAIR(3));
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


            if (snake.size() + 1 >= MaxSize)                        // ScoreBoard 갱신을 위한 MaxSize 계산
            {
                MaxSize = snake.size() + 1;
            } 
            
            ScoreBoard = newwin(HEIGHT_SMB - 1, WIDTH_SMB, 2, 29);  // ScoreBoard 갱신
            mvwprintw(ScoreBoard, 2, 2, "B : %d / %d", snake.size() + 1, MaxSize);
            mvwprintw(ScoreBoard, 4, 2, "+ : %d", countGrowth);
            mvwprintw(ScoreBoard, 6, 2, "- : %d", countPosion);
            mvwprintw(ScoreBoard, 8, 2, "G : %d", countGate);
            wbkgd(ScoreBoard, COLOR_PAIR(7));
    

            MissionBoard = newwin(HEIGHT_SMB, WIDTH_SMB, 15, 29);    // MissionBoard 갱신
            if (MaxSize >= MissionBody[stage - 1])
            {
                mvwprintw(MissionBoard, 2, 2, "B : %d (%c)", MissionBody[stage - 1], 'V');
            }
            else
            {
                mvwprintw(MissionBoard, 2, 2, "B : %d (%c)", MissionBody[stage - 1], ' ');
            }

            if (countGrowth >= MissionApple[stage - 1])
            {
                mvwprintw(MissionBoard, 4, 2, "+ : %d (%c)", MissionApple[stage - 1], 'V');
            }
            else 
            {
                mvwprintw(MissionBoard, 4, 2, "+ : %d (%c)", MissionApple[stage - 1], ' ');
            }

            if (countPosion >= MissionPosion[stage - 1])
            {
                mvwprintw(MissionBoard, 6, 2, "- : %d (%c)", MissionPosion[stage - 1], 'V');
            }
            else
            {
                mvwprintw(MissionBoard, 6, 2, "- : %d (%c)", MissionPosion[stage - 1], ' ');
            }

            if (countGate >= MissionGate[stage - 1])
            {
                mvwprintw(MissionBoard, 8, 2, "G : %d (%c)", MissionGate[stage - 1], 'V');
            }
            else
            {
                mvwprintw(MissionBoard, 8, 2, "G : %d (%c)", MissionGate[stage - 1], ' ');
            }
            wbkgd(MissionBoard, COLOR_PAIR(7));

            refresh();
            wrefresh(ScoreBoard);
            wrefresh(MissionBoard);


            if (MaxSize >= MissionBody[stage - 1] && countGrowth >= MissionApple[stage -1] && countPosion >= MissionPosion[stage - 1] && countGate >= MissionGate[stage -1])
            {
                initSetting();
                running = false;
                break;
            }

            this_thread::sleep_for(chrono::milliseconds(200));
        }

        if ( key_thread.joinable() )
        {
            key_thread.join();
        }
    }
}
