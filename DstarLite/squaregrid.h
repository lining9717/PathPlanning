#ifndef __D_STAR_LITE_SQUAREGRID__
#define __D_STAR_LITE_SQUAREGRID__
#include "node.h"
class SquareGrid
{
public:
    int width;
    int height;
    std::set<std::pair<int, int>> walls;
    std::vector<std::pair<int, int>> neigh = {m_p(1, 0), m_p(0, -1), m_p(-1, 0), m_p(0, 1)};
    SquareGrid() {}
    SquareGrid(int w, int h)
    {
        width = w;
        height = h;
    }
    bool in_bound(Node &n)
    {
        return 0 <= n.x and n.x < width and 0 <= n.y and n.y < height;
    }

    bool is_wall(Node &n)
    {
        for (std::set<std::pair<int, int>>::iterator it = walls.begin(); it != walls.end(); it++)
        {
            if ((*it).first == n.x and (*it).second == n.y)
                return true;
        }
        return false;
    }

    int cost(Node &from_node, Node &to_node)
    {
        if (is_wall(from_node) or is_wall(to_node))
            return INFI;
        return 1;
    }

    std::vector<Node> neighbors(Node &n)
    {
        std::vector<Node> results;
        for (auto it : neigh)
        {
            Node nn(n.x + it.first, n.y + it.second);
            if (in_bound(nn))
                results.push_back(nn);
        }
        return results;
    }

    std::vector<std::pair<Node, char>> observe(Node &position, int obs_range)
    {
        std::vector<std::pair<Node, char>> results;
        for (int x = position.x - obs_range;
             x <= position.x + obs_range; x++)
        {
            for (int y = position.y - obs_range;
                 y <= position.y + obs_range; y++)
            {
                Node n(x, y);
                if (in_bound(n))
                {
                    if (is_wall(n))
                        results.push_back(std::pair<Node, char>(n, WALL));
                    else
                        results.push_back(std::pair<Node, char>(n, PASSABLE));
                }
            }
        }
        return results;
    }
    void print_walls()
    {
        std::cout << "Walls:";
        for (std::pair<int, int> n : walls)
        {
            std::cout << "(" << n.first << "," << n.second << ") ";
        }
        std::cout << std::endl;
    }
};
#endif