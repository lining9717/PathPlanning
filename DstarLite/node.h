#ifndef __D_STAR_LITE_NODE__
#define __D_STAR_LITE_NODE__
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <deque>
#include <limits>
#include <math.h>
#include <memory>

#define Key std::pair<int, int>
#define m_p(a, b) std::make_pair(a, b)
#define INFI 1e8 + 7
#define WALL '#'
#define PASSABLE '.'

struct Node
{
    int x;
    int y;
    Key priority;
    Node()
    {
        x = y;
        priority = m_p(0, 0);
    }
    Node(int x, int y) : x(x), y(y)
    {
        priority = m_p(0, 0);
    }
    Node(int x, int y, Key p) : x(x), y(y), priority(p)
    {
    }
    bool operator<(const Node &g) const
    {
        if (priority.first < g.priority.first or
            (priority.first == g.priority.first and priority.second < g.priority.second))
            return true;
        return false;
    }
    bool operator>(const Node &g) const
    {
        if (priority.first < g.priority.first or
            (priority.first == g.priority.first and priority.second < g.priority.second))
            return false;
        return true;
    }

    bool operator!=(const Node &g) const
    {
        if (priority.first == g.priority.first and priority.second == g.priority.second)
            return false;
        return true;
    }
    void print_position()
    {
        std::cout << "(" << x << "," << y << ")" << std::endl;
    }
};
#endif