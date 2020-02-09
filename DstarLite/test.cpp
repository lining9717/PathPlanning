#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <math.h>
using namespace std;
#define m_p(a, b) std::make_pair(a, b)
#define Key std::pair<int, int>

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

class PriorityQueue
{
private:
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> p;

public:
    PriorityQueue()
    {
    }
    bool empty()
    {
        return p.size() == 0;
    }
    int size()
    {
        return p.size();
    }

    void put(Node n, Key priority)
    {
        n.priority = priority;
        p.push(n);
    }

    Node top()
    {
        return p.top();
    }

    Node pop()
    {
        Node n = p.top();
        p.pop();
        return n;
    }

    Key first_key()
    {
        return p.top().priority;
    }

    void deleteNode(Node n)
    {
        std::vector<Node> v;
        std::cout << "         delete all nodes" << std::endl;
        while (!p.empty())
        {
            Node nn = p.top();
            if (!(nn.x == n.x and nn.y == n.y))
                v.push_back(nn);
            p.pop();
        }
        std::cout << "         delete completed" << std::endl;

        for (Node i : v)
        {
            p.push(i);
        }
    }
};

int main()
{
    // std::ifstream myfile("./map.txt");
    // std::string input;
    // std::vector<std::string> lab;
    // int width, height;
    // while (getline(myfile, input))
    // {
    //     lab.push_back(input);
    //     width = std::max(width, (int)input.size());
    // }
    // height = lab.size();
    // cout << width << "," << height << endl;

    // std::map<std::pair<int, int>, int> G_VALS;
    // cout << G_VALS.count(m_p(2, 3)) << endl;
    // cout << G_VALS.size() << endl;
    // G_VALS[m_p(2, 3)] = 5;
    // G_VALS[m_p(2, 6)] = 5;
    // cout << G_VALS.size() << endl;
    // cout << G_VALS.count(m_p(2, 3)) << endl;
    // cout << G_VALS[m_p(2, 6)] << endl;

    PriorityQueue p;
    p.put(Node(2, 3), m_p(2, 2));
    p.put(Node(2, 32), m_p(2, 4));
    p.put(Node(21, 3), m_p(2, 4));
    p.put(Node(25, 3), m_p(24, 34));
    p.put(Node(221, 3), m_p(5, 0));
    p.put(Node(6, 77), m_p(9, 3));
    cout << p.size() << endl;
    cout << "top:(" << p.top().x << "," << p.top().y << ")" << endl;
    p.deleteNode(Node(221, 3));
    p.pop();
    cout << "top:(" << p.top().x << "," << p.top().y << ")" << endl;
    p.pop();
    cout << "top:(" << p.top().x << "," << p.top().y << ")" << endl;
    p.pop();
    cout << "top:(" << p.top().x << "," << p.top().y << ")" << endl;
    p.pop();
    cout << "top:(" << p.top().x << "," << p.top().y << ")" << endl;
    p.deleteNode(Node(4, 3));
    cout << "top:(" << p.top().x << "," << p.top().y << ")" << endl;
    return 0;
}