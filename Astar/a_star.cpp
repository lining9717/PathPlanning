#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <thread>
#include <vector>

#define MAXMAP 1000
#define EMPTY -1

class Node
{
public:
    int x; //x index of grid
    int y; //y index of grid
    float cost;
    int pind;
    Node(int xx, int yy, float c, int p)
    {
        x = xx;
        y = yy;
        cost = c;
        pind = p;
    }
};

class AStar
{
public:
    AStar(const std::string txt_path, int reso, int rr)
    {
        this->reso = reso;
        this->rr = rr;
        read_data(txt_path);
    }
    ~AStar()
    {
    }

    /**
     * @brief A star path search
     * 
     * @param sx [input] start x position
     * @param sy [input] start y position
     * @param gx [input] goal x position
     * @param gy [input] goal y position
     * @param rx [output] x positions list of the final path
     * @param ry [output] y positions list of the final path
     */
    void planning(float sx, float sy, float gx, float gy, std::vector<float> &rx, std::vector<float> &ry)
    {
        Node nstart(calc_xyindex(sx, minx), calc_xyindex(sy, miny), 0.0, -1);
        Node ngoal(calc_xyindex(gx, minx), calc_xyindex(gy, miny), 0.0, -1);

        //openset: one is Node's id, one is Node set, they both have the same index
        std::vector<int> openset_id;
        std::vector<Node> openset_node;

        //closeset: one is Node's id, one is Node set, they both have the same index
        std::vector<int> closeset_id;
        std::vector<Node> closeset_node;

        //put the start Node into openset
        openset_id.push_back(calc_grid_index(nstart));
        openset_node.push_back(nstart);

        while (1)
        {
            //get min cost node in openset
            int c_id = getMinCostInMap(openset_id, openset_node, ngoal);
            if (c_id == EMPTY)
            {
                std::cout << "Empty in openset" << std::endl;
                return;
            }
            //get the min cost node's index
            int c_index = getIdInVector(c_id, openset_id);
            if (c_index == -1)
            {
                std::cout << "Error openset_id" << std::endl;
                return;
            }
            Node current = openset_node[c_index]; //min cost node in openset

            if (current.x == ngoal.x && current.y == ngoal.y)
            {
                std::cout << "Find goal!" << std::endl;
                ngoal.pind = current.pind;
                ngoal.cost = current.cost;
                break;
            }
            // Remove the item from the open set
            openset_id.erase(openset_id.begin() + c_index);
            openset_node.erase(openset_node.begin() + c_index);

            // Add it to the closed set
            closeset_id.push_back(c_id);
            closeset_node.push_back(current);

            // expand_grid search grid based on motion model
            for (size_t i = 0; i < sizeof(motion) / sizeof(*motion); i++)
            {
                Node node(current.x + motion[i][0], current.y + motion[i][1], current.cost + motion[i][2], c_id);
                int n_id = calc_grid_index(node);

                // If the node is not safe, do nothing
                if (isValueInVector(n_id, closeset_id))
                    continue;

                if (!verify_node(node))
                    continue;

                if (!isValueInVector(n_id, openset_id))
                {
                    // discovered a new node
                    openset_id.push_back(n_id);
                    openset_node.push_back(node);
                }
                else
                {
                    int n_index = getIdInVector(n_id, openset_id);
                    if (openset_node[n_index].cost > node.cost)
                        openset_node[n_index] = node; // This path is the best until now. update it
                }
            }
        }
        calc_finalpath(ngoal, closeset_id, closeset_node, rx, ry);
        print_map(rx, ry);
    }

private:
    /**
     * reso: grid resolution
     * rr: robot radius
     * minx：min x in map
     * miny：min y in map
     * maxx: max x in map
     * maxy:max y in map
     * xwidth: map's width
     * ywidth: map's height
     * map: two-dimensional array
     * motion: eight-way direction and cost
     */
    float reso;
    float rr;
    int minx, miny, maxx, maxy;
    int xwidth, ywidth;
    int map[MAXMAP][MAXMAP];
    float motion[8][3] = {{1.0, 0.0, 1.0},
                          {0.0, 1.0, 1.0},
                          {-1.0, 0.0, 1.0},
                          {0.0, -1.0, 1.0},
                          {-1.0, -1.0, sqrt(2)},
                          {-1.0, 1.0, sqrt(2)},
                          {1.0, -1.0, sqrt(2)},
                          {1.0, 1.0, sqrt(2)}};

    /**
     * @brief 
     * 
     * @param ngoal [input] Node target position
     * @param closeset_id [input] vector<int> close set id
     * @param closeset_node [input] vector<Node> close set node
     * @param rx [output] vector<float> x positions list of the final path
     * @param ry [output] vector<float> y positions list of the final path
     */
    void calc_finalpath(Node &ngoal, std::vector<int> &closeset_id, std::vector<Node> &closeset_node,
                        std::vector<float> &rx, std::vector<float> &ry)
    {
        rx.push_back(float(calc_grid_position(ngoal.x, minx)));
        ry.push_back(float(calc_grid_position(ngoal.y, miny)));

        int pind = ngoal.pind;
        float cost = 0;
        while (pind != -1)
        {
            int index = getIdInVector(pind, closeset_id);
            Node n = closeset_node[index];
            rx.push_back(float(calc_grid_position(n.x, minx)));
            ry.push_back(float(calc_grid_position(n.y, miny)));
            cost += n.cost;
            pind = n.pind;
        }
        std::cout << "Cost:" << (double)cost << std::endl;
    }

    /**
     * @brief calculate the distance from n1 to n2
     * 
     * @param n1 
     * @param n2 
     * @return float 
     */
    float calc_heuristic(Node &n1, Node &n2)
    {
        float w = 1.0;
        //absolute distance
        float d = w * sqrt((n1.x - n2.x) * (n1.x - n2.x) + (n1.y - n2.y) * (n1.y - n2.y));
        return d;
    }

    /**
     * @brief read map from a txt file
     * 
     * @param txt_path std::string map file path
     */
    void read_data(const std::string txt_path)
    {
        std::ifstream read(txt_path);
        if (read.fail())
        {
            std::cout << "Open file fail" << std::endl;
            read.close();
            return;
        }
        std::string line;
        int rowcount = 0;
        int colcount = 0;
        int i = 0, j = 0;
        while (getline(read, line))
        {
            char *cstr = new char[line.length() + 1];
            std::strcpy(cstr, line.c_str());
            bool flag = true;
            char *p = std::strtok(cstr, " ");
            j = 0;
            while (p != NULL)
            {
                colcount++;
                map[i][j] = atoi(p);
                p = std::strtok(NULL, " ");
                j++;
            }
            i++;
            rowcount++;
            delete[] p;
            delete[] cstr;
        }
        read.close();

        std::cout << "col count:" << colcount / rowcount << std::endl;
        std::cout << "row count:" << rowcount << std::endl;
        ywidth = rowcount;
        xwidth = colcount / rowcount;
        minx = 0;
        miny = 0;
        maxx = xwidth;
        maxy = ywidth;
    }

    void print_map(std::vector<float> &rx, std::vector<float> &ry)
    {
        bool isWrited = false;
        for (int i = 0; i < ywidth; i++)
        {
            for (int j = 0; j < xwidth; j++)
            {
                for (int s = 0; s < rx.size(); s++)
                {

                    if (rx[s] == j && ry[s] == i && !isWrited)
                    {
                        std::cout << "*"
                                  << " ";
                        isWrited = true;
                    }
                }
                if (!isWrited)
                {
                    std::cout << map[i][j] << " ";
                }
                isWrited = false;
            }
            std::cout << std::endl;
        }
    }

    /**
     * @brief get grid position in map
     * 
     * @param index 
     * @param minp :minx or miny
     * @return int :position
     */
    int calc_grid_position(int index, int minp)
    {
        return index * reso + minp;
    }

    /**
     * @brief calculate node's index
     * 
     * @param n Node
     * @return int  index
     */
    int calc_grid_index(Node &n)
    {
        return (n.y - miny) * xwidth + (n.x - minx);
    }

    /**
     * @brief 
     * 
     * @param position 
     * @param minp 
     * @return int real position index 
     */
    int calc_xyindex(float position, int minp)
    {
        return round_number((position - minp) / reso);
    }

    /**
     * @brief vertify whether the node is free
     * 
     * @param n Node
     * @return true is free
     * @return false not free
     */
    bool verify_node(Node &n)
    {
        int px = calc_grid_position(n.x, minx);
        int py = calc_grid_position(n.y, miny);
        if (px < minx)
            return false;
        else if (py < miny)
            return false;
        else if (px >= maxx)
            return false;
        else if (py >= maxy)
            return false;

        if (map[(int)n.y][(int)n.x] == 1)
            return false;

        return true;
    }

    /**
     * @brief Get the Min Cost In Map object
     * 
     * @param v_id 
     * @param v_node 
     * @return int 
     */
    int getMinCostInMap(std::vector<int> &v_id, std::vector<Node> &v_node, Node &ngoal)
    {
        if (v_id.empty() || v_node.empty())
        {
            return EMPTY;
        }
        int size = v_id.size();
        int id = 0;
        int isFisrt = true;
        float mincost;
        for (int i = 0; i < size; i++)
        {
            //cost is composed of the cost from start node to v_node[i] and from v_node[i] to goal node
            float cost = v_node[i].cost + calc_heuristic(v_node[i], ngoal);
            if (isFisrt)
            {
                mincost = cost;
                id = v_id[i];
                isFisrt = false;
            }
            if (mincost > cost)
            {
                mincost = cost;
                id = v_id[i];
            }
        }
        return id;
    }

    int getIdInVector(int value, std::vector<int> &v_id)
    {
        for (int i = 0; i < v_id.size(); i++)
        {
            if (value == v_id[i])
                return i;
        }
        return -1;
    }
    bool isValueInVector(int value, std::vector<int> &v)
    {
        for (int i = 0; i < v.size(); i++)
        {
            if (value == v[i])
                return true;
        }
        return false;
    }

    float getMinFromVector(std::vector<float> &v)
    {
        float min_v = v[0];
        for (auto i = 0; i < v.size(); i++)
        {
            if (min_v > v[i])
            {
                min_v = v[i];
            }
        }
        return min_v;
    }

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
};

int main(int argc, char const *argv[])
{
    std::string txt_path = "/home/ln/WorkSpace/c++/pathplanning/map_generator/map1.txt";
    float sx = 2.0;
    float sy = 2.0;
    float gx = 50.0;
    float gy = 50.0;

    float grid_size = 1.0;
    float robot_radius = 1.0;

    std::vector<float> rx, ry;

    AStar astar(txt_path, grid_size, robot_radius);
    astar.planning(sx, sy, gx, gy, rx, ry);

    for (size_t i = 0; i < rx.size(); i++)
    {
        std::cout << "(" << rx[i] << "," << ry[i] << ")"
                  << "<---";
    }
    std::cout << "Go!" << std::endl;
    return 0;
}
