#ifndef __FWING_DECIDE_ACTION_DSTARLITE_V2_AGENTVIEWGRID_H__
#define __FWING_DECIDE_ACTION_DSTARLITE_V2_AGENTVIEWGRID_H__

#include "fwing_decide_action_dstarlite_v2_node_impl.h"
#include "fwing_decide_action_dstarlite_v2_squaregrid_impl.h"

namespace fwing_decide_action_dstarlite_v2_impl
{
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
}
#endif