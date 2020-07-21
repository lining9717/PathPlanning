#ifndef __FWING_DECIDE_ACTION_DSTARLITE_V2_MAIN_IMPL_H__
#define __FWING_DECIDE_ACTION_DSTARLITE_V2_MAIN_IMPL_H__

#include "fwing_decide_action_dstarlite_v2_agentviewgrid_impl.h"
#include "fwing_decide_action_dstarlite_v2_node_impl.h"
#include "fwing_decide_action_dstarlite_v2_priorityqueue_impl.h"
#include "fwing_decide_action_dstarlite_v2_squaregrid_impl.h"

namespace fwing_decide_action_dstarlite_v2_impl
{
class DStarLite
{
private:
    AgentViewGrid grid;
    SquareGrid real_grid;
    bool is_show = false;
    int view_range = 2;
    std::vector<std::pair<Node, Node>> back_pointer;
    int km = 0;
    Node position;
    Node goal;
    Node start;
    std::map<std::pair<int, int>, int> G_VALS;
    std::map<std::pair<int, int>, int> RHS_VALS;
    PriorityQueue frontier;

public:
    DStarLite() {}
    ~DStarLite() {}
    bool init(std::pair<int, int> nstart, std::pair<int, int> ngoal, const char *map, int v_range)
    {
        std::ifstream myfile(map);
        std::string input;
        std::vector<std::string> lab;
        int width = 0, height = 0;
        view_range = v_range;
        km = 0;
        while (getline(myfile, input))
        {
            lab.push_back(input);
            width = std::max(width, (int)input.size());
        }
        height = lab.size();
        real_grid = SquareGrid(width, height);
        grid = AgentViewGrid(width, height);
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                if (lab[i][j] == WALL)
                    real_grid.walls.insert(m_p(j, i));
                if (j == nstart.first and i == nstart.second)
                {
                    position = Node(j, i);
                    start = Node(j, i);
                    // std::cout << "start:(" << start.x << "," << start.y << ")" << std::endl;
                }
                if (j == ngoal.first and i == ngoal.second)
                    goal = Node(j, i);
            }
        }
        if (start.x == -1 and start.y == -1 or goal.x == -1 and goal.y == -1)
        {
            // std::cout << "!!!!NO START AND GOAL IN MAP!!!" << std::endl;
            return false;
        }

        Key goal_key = calculate_key(goal);
        frontier = PriorityQueue();
        frontier.put(goal, goal_key);
        back_pointer.push_back(m_p(goal, Node(-1, -1)));
        return true;
    }

    void setStart(std::pair<int, int> n)
    {
        position = Node(n.first, n.second);
        start = Node(n.first, n.second);
        // std::cout << "start:(" << start.x << "," << start.y << ")" << std::endl;
    }

    void setGoal(std::pair<int, int> n)
    {
        goal = Node(n.first, n.second);
    }

    void setIsShow(bool value)
    {
        is_show = value;
    }

    int heuristic(Node &a, Node &b)
    {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }

    Key calculate_key(Node &n)
    {
        int g_rhs = std::min(g(n), rhs(n));
        return m_p(g_rhs + heuristic(n, position) + km, g_rhs);
    }

    bool is_key1_lowerthan_key2(Key k1, Key k2)
    {
        if (k1.first < k2.first or
            (k1.first == k2.first and k1.second < k2.second))
            return true;
        return false;
    }

    int rhs(Node &n)
    {
        if (n.x == goal.x and n.y == goal.y)
            return 0;

        if (RHS_VALS.size() == 0 or RHS_VALS.count(m_p(n.x, n.y)) == 0)
            return INFI;
        return RHS_VALS[m_p(n.x, n.y)];
    }

    int g(Node &n)
    {
        if (G_VALS.size() == 0 or G_VALS.count(m_p(n.x, n.y)) == 0)
            return INFI;
        return G_VALS[m_p(n.x, n.y)];
    }

    int different_nodes_number(std::deque<Node> &nodes)
    {
        std::deque<Node>::iterator it1;
        std::vector<std::pair<int, int>> v;
        for (it1 = nodes.begin(); it1 != nodes.end(); it1++)
            v.push_back(m_p(it1->x, it1->y));
        std::set<std::pair<int, int>> st(v.begin(), v.end());
        v.assign(st.begin(), st.end());
        int size = v.size();
        v.clear();
        v.shrink_to_fit();
        return size;
    }

    Node lowest_cost_neighbour(Node &n)
    {
        std::vector<Node> neighbours;
        grid.neighbors(n, neighbours);
        Node lowerest_neighbour = neighbours[0];
        int min_cost = INFI;
        for (Node neighbour : neighbours)
        {
            int cost = g(neighbour) + grid.cost(neighbour, n);
            if (min_cost > cost)
            {
                min_cost = cost;
                lowerest_neighbour = neighbour;
            }
        }
        neighbours.clear();
        neighbours.shrink_to_fit();
        return lowerest_neighbour;
    }

    int calculate_rhs(Node &n)
    {
        std::vector<Node> neighbours;
        grid.neighbors(n, neighbours);
        Node lowerest_neighbour = neighbours[0];
        int min_cost = INFI;
        for (Node neighbour : neighbours)
        {
            int cost = g(neighbour) + grid.cost(neighbour, n);
            if (min_cost > cost)
            {
                min_cost = cost;
                lowerest_neighbour = neighbour;
            }
        }
        back_pointer.push_back(m_p(n, lowerest_neighbour));
        neighbours.clear();
        neighbours.shrink_to_fit();
        return min_cost;
    }

    void update_node(Node &n)
    {
        if (!(n.x == goal.x and n.y == goal.y))
            RHS_VALS[m_p(n.x, n.y)] = calculate_rhs(n);
        frontier.deleteNode(n);
        if (g(n) != rhs(n))
        {
            Key k = calculate_key(n);
            frontier.put(n, k);
        }
    }

    void update_nodes(std::vector<Node> &nodes)
    {
        int size = nodes.size();
        if (size == 0)
            return;
        for (int i = 0; i < size; i++)
            update_node(nodes[i]);
    }

    void draw_map(SquareGrid graph, std::vector<Node> &path)
    {
        bool isWrited = false;
        for (int i = 0; i < graph.height; ++i)
        {
            for (int j = 0; j < graph.width; ++j)
            {
                for (Node n : path)
                {
                    if (n.x == j and n.y == i and !isWrited and
                        !(j == start.x and i == start.y) and !(j == goal.x and i == goal.y))
                    {
                        std::cout << " " << PATH << " ";
                        isWrited = true;
                    }
                }
                if (!isWrited)
                {
                    if (graph.walls.count(m_p(j, i)))
                        std::cout << " " << WALL << " ";
                    else if (j == start.x and i == start.y)
                        std::cout << " " << START << " ";
                    else if (j == goal.x and i == goal.y)
                        std::cout << " " << GOAL << " ";
                    else
                        std::cout << " " << PASSABLE << " ";
                }
                isWrited = false;
            }
            std::cout << std::endl;
        }
    }

    void draw_map(SquareGrid graph)
    {
        for (int i = 0; i < graph.height; ++i)
        {
            for (int j = 0; j < graph.width; ++j)
            {
                if (graph.walls.count(m_p(j, i)))
                    std::cout << " " << WALL << " ";
                else if (j == start.x and i == start.y)
                    std::cout << " " << START << " ";
                else if (j == goal.x and i == goal.y)
                    std::cout << " " << GOAL << " ";
                else
                    std::cout << " " << PASSABLE << " ";
            }
            std::cout << std::endl;
        }
    }

    int compute_shortest_path()
    {
        std::deque<Node> last_nodes;
        std::vector<Node> neighs;
        while (is_key1_lowerthan_key2(frontier.first_key(), calculate_key(position)) or g(position) != rhs(position))
        {
            if (frontier.size() == 0)
                return NO_PATH;
            Key k_old = frontier.first_key();
            Node node = frontier.pop();
            last_nodes.push_back(node);
            if (last_nodes.size() == 10 and different_nodes_number(last_nodes) < 3)
            {
                std::cout << "[DStarLite FAIL]:Stuck in a loop!" << std::endl;
                return NO_PATH;
            }

            Key k_new = calculate_key(node);

            if (is_key1_lowerthan_key2(k_old, k_new))
                frontier.put(node, k_new);
            else if (g(node) > rhs(node))
            {
                G_VALS[m_p(node.x, node.y)] = RHS_VALS[m_p(node.x, node.y)];
                grid.neighbors(node, neighs);
                update_nodes(neighs);
                neighs.clear();
                neighs.shrink_to_fit();
            }
            else
            {
                G_VALS[m_p(node.x, node.y)] = INFI;
                grid.neighbors(node, neighs);
                neighs.push_back(node);
                update_nodes(neighs);
                neighs.clear();
                neighs.shrink_to_fit();
            }
        }
        return HAVE_PATH;
    }

    int move_to_goal(std::vector<Node> &path)
    {
        // std::cout << "The real graph (A=Start, Z=Goal):" << std::endl;
        // draw_map(real_grid);
        std::vector<std::pair<Node, char>> observation;
        real_grid.observe(position, view_range, observation);
        std::vector<Node> walls;
        grid.new_walls(observation, walls);
        grid.update_walls(walls);
        int status = compute_shortest_path();
        if (status == NO_PATH)
            return NO_PATH;

        Node last_node = position;
        path.push_back(position);
        std::vector<Node> new_walls;

        while (!(position.x == goal.x and position.y == goal.y))
        {
            if (is_show)
            {
                std::cout << "******************************************************" << std::endl;
                std::cout << "constructing graph (A=Start, Z=Goal, @=path):" << std::endl;
                draw_map(grid, path);
            }
            if (g(position) == INFI)
            {
                std::cout << "No Path!!!" << std::endl;
                return NO_PATH;
            }
            position = lowest_cost_neighbour(position);
            std::vector<std::pair<Node, char>>().swap(observation);
            real_grid.observe(position, view_range, observation);

            grid.new_walls(observation, new_walls);

            if (new_walls.size())
            {
                grid.update_walls(new_walls);
                km += heuristic(last_node, position);
                last_node = position;
                for (Node wallnode : new_walls)
                {
                    std::vector<Node> neigh_nodes;
                    grid.neighbors(wallnode, neigh_nodes);
                    for (Node node : neigh_nodes)
                        if (!grid.is_wall(node))
                            update_node(node);
                }
                status = compute_shortest_path();
                if (status == NO_PATH)
                    return NO_PATH;
            }
            path.push_back(position);
            std::vector<Node>().swap(new_walls);
        }
        // std::cout << "******************************************************" << std::endl;
        // std::cout << "Final Path, taken (@ symbols):" << std::endl;
        // draw_map(real_grid, path);
        return HAVE_PATH;
    }
};
}
#endif