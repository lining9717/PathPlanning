#include "dstarlite.h"

bool DStarLite::init(const char *map, int v_range)
{
    std::ifstream myfile(map);
    std::string input;
    std::vector<std::string> lab;
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
            if (lab[i][j] == START)
            {
                position = Node(j, i);
                start = Node(j, i);
            }
            if (lab[i][j] == GOAL)
                goal = Node(j, i);
        }
    }
    if (start.x == -1 and start.y == -1 or goal.x == -1 and goal.y == -1)
    {
        std::cout << "!!!!NO START(A) AND GOAL(Z) IN MAP!!!" << std::endl
                  << "Must use symbol A to represent Start point and use symbol Z to represent Goal point" << std::endl;
        return false;
    }
    view_range = v_range;
    Key goal_key = calculate_key(goal);
    frontier = PriorityQueue();
    frontier.put(goal, goal_key);
    back_pointer.push_back(m_p(goal, Node(-1, -1)));
    return true;
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

    if (RHS_VALS.size() == 0 or RHS_VALS.count(m_p(n.x, n.y)) == 0)
        return INFI;
    return RHS_VALS[m_p(n.x, n.y)];
}

int DStarLite::g(Node &n)
{
    if (G_VALS.size() == 0 or G_VALS.count(m_p(n.x, n.y)) == 0)
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
    int size = v.size();
    v.clear();
    v.shrink_to_fit();
    return size;
}

Node DStarLite::lowest_cost_neighbour(Node &n)
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

int DStarLite::calculate_rhs(Node &n)
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

void DStarLite::update_node(Node &n)
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

void DStarLite::update_nodes(std::vector<Node> &nodes)
{
    int size = nodes.size();
    if (size == 0)
        return;
    for (int i = 0; i < size; i++)
        update_node(nodes[i]);
}

void DStarLite::draw_map(SquareGrid graph, std::vector<Node> &path)
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
                    std::cout << WALL << WALL << WALL;
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

void DStarLite::draw_map(SquareGrid graph)
{
    for (int i = 0; i < graph.height; ++i)
    {
        for (int j = 0; j < graph.width; ++j)
        {
            if (graph.walls.count(m_p(j, i)))
                std::cout << WALL << WALL << WALL;
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

int DStarLite::compute_shortest_path()
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
            std::cout << "[FAIL]:Stuck in a loop!" << std::endl;
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

int DStarLite::move_to_goal(std::vector<Node> &path)
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
        if (IS_PRINT_CONSTRUCTING_PROCESS == 1)
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

int main(int argc, char const *argv[])
{
    DStarLite dsl;
    if (!dsl.init("/home/ln/WorkSpace/c++/pathplanning/DstarLite/map_extend.txt", 2))
        return 0;

    std::vector<Node> path;
    int status = dsl.move_to_goal(path);
    if (status == HAVE_PATH)
    {
        std::cout << "Final Path:[Start=";
        int length = path.size();
        for (int i = 0; i < length; i++)
        {
            std::cout << "(" << path[i].x << "," << path[i].y << ")";
            if (i + 1 != length)
                std::cout << "->";
        }
        std::cout << "=Goal]" << std::endl;
        std::cout<<"length:"<<length<<std::endl;
    }
    else
        std::cout << "No Path!" << std::endl;

    return 0;
}