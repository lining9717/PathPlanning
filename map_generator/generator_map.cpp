#include <fstream>
#include <thread>
#include <math.h>
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;

float grid_size = 1.0;
float robot_radius = 1.0;

float getMaxFromVector(std::vector<float> &v)
{
    float max_v = v[0];
    for (auto i = 0; i < v.size(); i++)
    {
        if (max_v < v[i])
        {
            max_v = v[i];
        }
    }
    return max_v;
}

int round_number(double num)
{
    return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
}

int calc_position(int index, int minp)
{
    return index * grid_size + minp;
}

void generat_map(vector<float> ox, vector<float> oy)
{
    int minx = 0;
    int miny = 0;
    int maxx = round_number(getMaxFromVector(ox));
    int maxy = round_number(getMaxFromVector(oy));
    std::cout << "minx:" << minx << std::endl;
    std::cout << "maxx:" << maxx << std::endl;
    std::cout << "miny:" << miny << std::endl;
    std::cout << "maxy:" << maxy << std::endl;

    int xwidth = round_number((maxx - minx + 1) / grid_size);
    int ywidth = round_number((maxy - miny + 1) / grid_size);

    std::cout << "xwidth:" << xwidth << std::endl;
    std::cout << "ywidth:" << ywidth << std::endl;

    int map[xwidth][ywidth];
    for (int i = 0; i < xwidth; i++)
        for (int j = 0; j < ywidth; j++)
            map[i][j] = 0;

    for (int ix = 0; ix < xwidth; ix++)
    {
        int x = calc_position(ix, minx);
        for (int iy = 0; iy < ywidth; iy++)
        {
            int y = calc_position(iy, miny);
            for (auto t = 0; t < ox.size(); t++)
            {

                float d = sqrt((ox[t] - x) * (ox[t] - x) + (oy[t] - y) * (oy[t] - y));
                if (d < robot_radius)
                {
                    map[ix][iy] = 1;
                    break;
                }
            }
        }
    }

    ofstream writeToFile("map2.txt", ios::app);

    if (writeToFile.fail())
    {
        std::cout << "Open file fail" << std::endl;
        writeToFile.close();
    }
    for (int i = 0; i < xwidth; i++)
    {
        for (int j = 0; j < ywidth; j++)
        {
            writeToFile << map[i][j] << " ";
        }
        writeToFile << endl;
    }
    writeToFile.close();
}

void mapGenerator()
{
    vector<float> ox, oy;
    int height = 6;
    int width = 6;
    for (int i = 0; i < height; i++)
    {
        ox.push_back(i);
        oy.push_back(0.0);
    }

    for (int i = 0; i < width; i++)
    {
        ox.push_back(height - 1);
        oy.push_back(i);
    }

    for (int i = 0; i < height; i++)
    {
        ox.push_back(i);
        oy.push_back(width - 1);
    }

    for (int i = 0; i < width; i++)
    {
        ox.push_back(0.0);
        oy.push_back(i);
    }

    /*****************obstacle************/
    for (int i = 0; i < 3; i++)
    {
        ox.push_back(3);
        oy.push_back(i);
    }
    // for (int i = 0; i < 45; i++)
    // {
    //     ox.push_back(19.0);
    //     oy.push_back(i);
    // }

    // for (int i = 0; i < 30; i++)
    // {
    //     ox.push_back(i);
    //     oy.push_back(50);
    // }

    // for (int i = 0; i < 33; i++)
    // {
    //     ox.push_back(38.0);
    //     oy.push_back(width - i - 1);
    //     ox.push_back(39.0);
    //     oy.push_back(width - i - 1);
    //     ox.push_back(37.0);
    //     oy.push_back(width - i - 1);
    // }

    // for (int i = 0; i < 20; i++)
    // {
    //     ox.push_back(height - i - 1);
    //     oy.push_back(12);
    //     ox.push_back(height - i - 1);
    //     oy.push_back(13);

    // }

    // for (int i = 0; i < 15; i++)
    // {
    //     ox.push_back(height - 5 - i - 1);
    //     oy.push_back(20 + i);
    // }

    // for (int i = 25; i <= 30; i++)
    // {
    //     for (int j = 25; j <= 30; j++)
    //     {
    //         ox.push_back(i);
    //         oy.push_back(j);
    //     }
    // }

    generat_map(ox, oy);
}

int main(int argc, char const *argv[])
{
    mapGenerator();
    return 0;
}
