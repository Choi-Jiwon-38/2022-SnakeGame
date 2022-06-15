#ifndef __GATE__
#define __GATE__
#include "init.h"
#include <cstdlib>
#include <ctime>

vector<pair<int, int> > gate_pos;

bool g_using;

void create_gate()
{
    srand(time(NULL));

    int gate_x1, gate_y1;
    int gate_x2, gate_y2;

    while (1)                               // Gate 한쪽 먼저 설정
    {
        gate_x1 = rand() % WIDTH_GB;
        gate_y1 = rand() % HEIGHT_GB;
        
        if (map[gate_y1][gate_x1] != '1')
        {
            continue;
        }
        map[gate_y1][gate_x1] = '7';
        gate_pos.push_back(pair<int, int> (gate_y1, gate_x1));
        break;
    }
    while (1)
    {   
        gate_x2 = rand() % WIDTH_GB;
        gate_y2 = rand() % HEIGHT_GB;
        if (map[gate_y2][gate_x2] != '1' || (gate_x1 == gate_x2 && gate_y1 == gate_y2))
        {
            continue;
        }
        map[gate_y2][gate_x2] = '7';
        gate_pos.push_back(pair<int, int> (gate_y2, gate_x2));
        break;
    }
}
#endif