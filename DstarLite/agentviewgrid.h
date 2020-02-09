#ifndef __D_STAR_LITE_AGENTVIEWFRID__
#define __D_STAR_LITE_AGENTVIEWFRID__
#include "node.h"
#include "squaregrid.h"

class AgentViewGrid : public SquareGrid
{
public:
    AgentViewGrid() {}
    AgentViewGrid(int w, int h) : SquareGrid(width, height)
    {
        width = w;
        height = h;
    }
    void new_walls(std::vector<std::pair<Node, char>> &observation, std::vector<Node> &results)
    {
        for (std::vector<std::pair<Node, char>>::iterator iter = observation.begin(); iter != observation.end(); ++iter)
        {
            Node n = iter->first;
            if (iter->second == WALL and !is_wall(n))
            {
                results.push_back(n);
            }
        }
    }

    void update_walls(std::vector<Node> &new_walls)
    {
        for (Node n : new_walls)
        {
            walls.insert(m_p(n.x, n.y));
        }
    }
};
#endif