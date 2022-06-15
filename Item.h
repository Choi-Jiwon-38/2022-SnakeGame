#include "init.h"
#include <ctime>
#include <cstdlib>

time_t startTime;
time_t currentTime;

vector<pair<int, int> > itemList;

void create_item()
{
    startTime = time(NULL);

    srand(time(NULL));

    int item_x, item_y;

    while (num_of_item < 3)
    {
        item_x = rand() % WIDTH_GB;
        item_y = rand() % HEIGHT_GB;

        if (map[item_y][item_x] != '0')
        {
            continue;
        }

        if (num_of_growth < total_of_growth)
        {
            map[item_y][item_x] = '5';
            num_of_item++;
            num_of_growth++;
            itemList.push_back(pair<int, int> (item_y, item_x));
            continue;
        }

        map[item_y][item_x] = '6';
        itemList.push_back(pair<int, int> (item_y, item_x));
        num_of_item++;
    }
}

void remove_item()
{
    currentTime = time(NULL);
    
    if (currentTime - startTime > 5)
    {
        for (int i = 0; i < HEIGHT_GB; i++)
        {
            for (int j = 0; j < WIDTH_GB; j++)
            {
                if (map[i][j] == '5' || map[i][j] == '6')
                {
                    map[i][j] = '0';
                }
            }
        }
        for (int i = 0; i < itemList.size(); i++)
        {    
            itemList.pop_back();
        }
        num_of_growth = 0;
        num_of_item = 0;

        create_item();
    }
}