#ifndef __D_STAR_LITE_DSTARLITE__
#define __D_STAR_LITE_DSTARLITE__

#include "node.h"
#include "agentviewgrid.h"
#include "squaregrid.h"
#include "priorityqueue.h"

#define IS_PRINT_CONSTRUCTING_PROCESS 1

class DStarLite
{
private:
    AgentViewGrid grid;
    SquareGrid real_grid;

    int view_range = 2;
    std::vector<std::pair<Node, Node>> back_pointer;
    int km;
    Node position;
    Node goal;
    Node start;
    std::map<std::pair<int, int>, int> G_VALS;
    std::map<std::pair<int, int>, int> RHS_VALS;
    PriorityQueue frontier;

public:
    DStarLite() {}
    bool init(const char *map, int v_range);
    int heuristic(Node &a, Node &b);
    Key calculate_key(Node &n);
    bool is_key1_lowerthan_key2(Key k1, Key k2);
    int rhs(Node &n);
    int g(Node &n);
    int different_nodes_number(std::deque<Node> &nodes);
    Node lowest_cost_neighbour(Node &n);
    int calculate_rhs(Node &n);
    void update_node(Node &n);
    void update_nodes(std::vector<Node> &nodes);
    void draw_map(SquareGrid graph, std::vector<Node> &path);
    void draw_map(SquareGrid graph);
    int compute_shortest_path();
    int move_to_goal(std::vector<Node> &path);
};

#endif