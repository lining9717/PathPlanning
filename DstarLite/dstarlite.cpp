#include "dstarlite.h"

void DStarLite::init(const char *map, int v_range)
{
    std::ifstream myfile(map);
    std::string input;

    int width = 0, height = 0;
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
            if (lab[i][j] == 'A')
                position = Node(j, i);
            if (lab[i][j] == 'Z')
                goal = Node(j, i);
        }
    }
    // std::cout << real_grid.walls.size() << std::endl;
    real_grid.print_walls();
    view_range = v_range;
    frontier = PriorityQueue();
    frontier.put(goal, calculate_key(goal));
    back_pointer.push_back(m_p(goal, Node(-1, -1)));
}

int DStarLite::heuristic(Node &a, Node &b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

Key DStarLite::calculate_key(Node &n)
{
    int g_rhs = std::min(g(n), rhs(n));
    return m_p(g_rhs + heuristic(n, position) + km, g_rhs);
}

bool DStarLite::is_key1_lowerthan_key2(Key k1, Key k2)
{
    if (k1.first < k2.first or
        (k1.first == k2.first and k1.second < k2.second))
        return true;
    return false;
}

int DStarLite::rhs(Node &n)
{
    if (n.x == goal.x and n.y == goal.y)
        return 0;

    if (RHS_VALS.count(m_p(n.x, n.y)) == 0)
        return INFI;
    return RHS_VALS[m_p(n.x, n.y)];
}

int DStarLite::g(Node &n)
{
    if (G_VALS.count(m_p(n.x, n.y)) == 0)
        return INFI;
    return G_VALS[m_p(n.x, n.y)];
}

int DStarLite::different_nodes_number(std::deque<Node> &nodes)
{
    std::deque<Node>::iterator it1;
    std::vector<std::pair<int, int>> v;
    for (it1 = nodes.begin(); it1 != nodes.end(); it1++)
        v.push_back(m_p(it1->x, it1->y));
    std::set<std::pair<int, int>> st(v.begin(), v.end());
    v.assign(st.begin(), st.end());
    return v.size();
}

Node DStarLite::lowest_cost_neighbour(Node &n)
{
    std::vector<Node> neighbours = grid.neighbors(n);
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
    return lowerest_neighbour;
}

int DStarLite::calculate_rhs(Node &n)
{
    std::vector<Node> neighbours = grid.neighbors(n);
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
    return min_cost;
}

void DStarLite::update_node(Node &n)
{
    if (!(n.x == goal.x and n.y == goal.y))
        RHS_VALS[m_p(n.x, n.y)] = calculate_rhs(n);
    frontier.deleteNode(n);
    if (g(n) != rhs(n))
    {
        frontier.put(n, calculate_key(n));
    }
}

void DStarLite::update_nodes(std::vector<Node> &nodes)
{
    if (nodes.size() == 0)
        return;
    for (Node n : nodes)
    {
        update_node(n);
    }
}

void DStarLite::draw_map(std::vector<Node> &path)
{
    bool isWrited = false;
    for (int i = 0; i < real_grid.height; ++i)
    {
        for (int j = 0; j < real_grid.width; ++j)
        {
            for (Node n : path)
            {
                if (n.x == j and n.y == i and !isWrited)
                {
                    std::cout << " @ ";
                    isWrited = true;
                }
            }
            if (!isWrited)
            {
                if (lab[i][j] == WALL)
                    std::cout << "###";
                else
                    std::cout << " " << lab[i][j] << " ";
            }
            isWrited = false;
        }
        std::cout << std::endl;
    }
}
void DStarLite::draw_map()
{
    for (int i = 0; i < real_grid.height; ++i)
    {
        for (int j = 0; j < real_grid.width; ++j)
        {
            if (lab[i][j] == WALL)
                std::cout << "###";
            else
                std::cout << " " << lab[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void DStarLite::compute_shortest_path()
{
    std::deque<Node> last_nodes;
    std::cout << "frontier size:" << frontier.size() << std::endl;
    std::cout << "frontier.first_key:(" << frontier.first_key().first << "," << frontier.first_key().second << ")" << std::endl;
    while (is_key1_lowerthan_key2(frontier.first_key(), calculate_key(position)) or g(position) != rhs(position))
    {
        Key k_old = frontier.first_key();
        Node node = frontier.pop();
        last_nodes.push_back(node);
        if (last_nodes.size() == 10 and different_nodes_number(last_nodes) < 3)
            std::cout << "[FAIL]:Stuck in a loop!" << std::endl;
        Key k_new = calculate_key(node);
        std::cout << "11111111111111111111111111111111111" << std::endl;
        if (is_key1_lowerthan_key2(k_old, k_new))
            frontier.put(node, k_new);
        else if (g(node) > rhs(node))
        {
            G_VALS[m_p(node.x, node.y)] = RHS_VALS[m_p(node.x, node.y)];
            std::vector<Node> neighs = grid.neighbors(node);
            update_nodes(neighs);
        }
        else
        {
            G_VALS[m_p(node.x, node.y)] = INFI;
            std::vector<Node> neighs = grid.neighbors(node);
            neighs.push_back(node);
            update_nodes(neighs);
        }
    }
}

void DStarLite::move_to_goal(std::vector<Node> &path)
{
    std::vector<std::pair<Node, char>> observation = real_grid.observe(position, view_range);
    std::vector<std::pair<Node, char>>::iterator iter;
    std::cout << "observation size:" << observation.size() << std::endl;
    // for (iter = observation.begin(); iter != observation.end(); iter++)
    // {
    //     std::cout << "(" << iter->first.x << "," << iter->first.y << "):" << iter->second << std::endl;
    // }

    std::vector<Node> walls = grid.new_walls(observation);
    grid.update_walls(walls);
    std::cout << "grid walls size:" << grid.walls.size() << std::endl;
    compute_shortest_path();
    Node last_node = position;
    path.push_back(position);
    while (!(position.x == goal.x and position.y == goal.y))
    {
        std::cout << "position node:";
        position.print_position();
        if (g(position) == INFI)
        {
            std::cout << "No Path!" << std::endl;
            return;
        }
        position = lowest_cost_neighbour(position);
        observation = real_grid.observe(position, view_range);
        std::vector<Node> new_walls = grid.new_walls(observation);

        if (new_walls.size())
        {
            grid.update_walls(new_walls);
            km += heuristic(last_node, position);
            last_node = position;
            for (Node wallnode : new_walls)
            {
                std::vector<Node> neigh_nodes = grid.neighbors(wallnode);
                for (Node node : neigh_nodes)
                    if (!grid.is_wall(node))
                        update_node(node);
            }
            compute_shortest_path();
        }
        path.push_back(position);
    }
}

int main(int argc, char const *argv[])
{
    DStarLite dsl;
    dsl.init("/home/ln/WorkSpace/c++/pathplanning/DstarLite/map.txt", 2);
    std::cout << "The graph (A=Start, Z=Goal):" << std::endl;
    dsl.draw_map();
    std::vector<Node> path;
    dsl.move_to_goal(path);
    std::cout << "Path taken (@ symbols):" << std::endl;
    dsl.draw_map(path);
    return 0;
}
