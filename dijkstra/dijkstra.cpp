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
    int x;
    int y;
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

class Dijkstra
{

public:
    Dijkstra(const std::string txt_path, int reso, int rr)
    {
        this->reso = reso;
        this->rr = rr;
        read_data(txt_path);
    }

    ~Dijkstra()
    {
    }

    void planning(float sx, float sy, float gx, float gy, std::vector<float> &rx, std::vector<float> &ry)
    {
        Node nstart(calc_xyindex(sx, minx), calc_xyindex(sy, miny), 0.0, -1);
        Node ngoal(calc_xyindex(gx, minx), calc_xyindex(gy, miny), 0.0, -1);

        std::vector<int> openset_id;
        std::vector<Node> openset_node;

        std::vector<int> closeset_id;
        std::vector<Node> closeset_node;

        openset_id.push_back(calc_index(nstart));
        openset_node.push_back(nstart);

        while (1)
        {
            int c_id = getMinCostInMap(openset_id, openset_node);
            if (c_id == EMPTY)
            {
                std::cout << "Empty in openset" << std::endl;
                return;
            }

            int c_index = getIdInVector(c_id, openset_id);
            if (c_index == -1)
            {
                std::cout << "Error openset_id" << std::endl;
                return;
            }
            Node current = openset_node[c_index];

            if (current.x == ngoal.x && current.y == ngoal.y)
            {
                std::cout << "Find goal!" << std::endl;
                ngoal.pind = current.pind;
                ngoal.cost = current.cost;
                break;
            }

            openset_id.erase(openset_id.begin() + c_index);
            openset_node.erase(openset_node.begin() + c_index);

            closeset_id.push_back(c_id);
            closeset_node.push_back(current);

            for (size_t i = 0; i < sizeof(motion) / sizeof(*motion); i++)
            {
                Node node(current.x + motion[i][0], current.y + motion[i][1], current.cost + motion[i][2], c_id);
                int n_id = calc_index(node);

                if (isValueInVector(n_id, closeset_id))
                    continue;

                if (!verify_node(node))
                    continue;

                if (!isValueInVector(n_id, openset_id))
                {
                    openset_id.push_back(n_id);
                    openset_node.push_back(node);
                }
                else
                {
                    int n_index = getIdInVector(n_id, openset_id);
                    if (openset_node[n_index].cost > node.cost)
                    {
                        openset_node[n_index] = node;
                    }
                }
            }
        }
        calc_finalpath(ngoal, closeset_id, closeset_node, rx, ry);
        print_map(rx, ry);
    }

private:
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
    
    void calc_finalpath(Node &ngoal, std::vector<int> &closeset_id, std::vector<Node> &closeset_node,
                        std::vector<float> &rx, std::vector<float> &ry)
    {
        rx.push_back(float(calc_position(ngoal.x, minx)));
        ry.push_back(float(calc_position(ngoal.y, miny)));

        int pind = ngoal.pind;
        float cost = 0;
        while (pind != -1)
        {
            int index = getIdInVector(pind, closeset_id);
            Node n = closeset_node[index];
            rx.push_back(float(calc_position(n.x, minx)));
            ry.push_back(float(calc_position(n.y, miny)));
            cost += n.cost;
            pind = n.pind;
        }
        std::cout << "Cost:" << (double)cost << std::endl;
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

    bool verify_node(Node &n)
    {
        int px = calc_position(n.x, minx);
        int py = calc_position(n.y, miny);
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

    int calc_index(Node &n)
    {
        return (n.y - miny) * xwidth + (n.x - minx);
    }

    int calc_xyindex(float position, int minp)
    {
        return round_number((position - minp) / reso);
    }

    int calc_position(int index, int minp)
    {
        return index * reso + minp;
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

    int getMinCostInMap(std::vector<int> &v_id, std::vector<Node> &v_node)
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
            if (isFisrt)
            {
                mincost = v_node[i].cost;
                id = v_id[i];
                isFisrt = false;
            }
            if (mincost > v_node[i].cost)
            {
                mincost = v_node[i].cost;
                id = v_id[i];
            }
        }
        return id;
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

void dijstra_plan(const std::string txt_path)
{
    float sx = 2.0;
    float sy = 2.0;
    float gx = 50.0;
    float gy = 50.0;

        float grid_size = 1.0;
    float robot_radius = 1.0;

    std::vector<float> rx, ry;

    Dijkstra dj(txt_path, grid_size, robot_radius);
    dj.planning(sx, sy, gx, gy, rx, ry);

    for (size_t i = 0; i < rx.size(); i++)
    {
        std::cout << "(" << rx[i] << "," << ry[i] << ")"
                  << "<---";
    }
    std::cout << "Go!" << std::endl;
}

int main(int argc, char const *argv[])
{

    dijstra_plan("/home/ln/WorkSpace/pathplanning/map_generator/map1.txt");

    return 0;
}
