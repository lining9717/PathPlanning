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

struct Node
{
    int x;
    int y;
    int priority;
    Node(int x, int y) : x(x), y(y) { priority = 0; }
    Node(int x, int y, int p) : x(x), y(y), priority(p) {}
    bool operator<(const Node &g) const
    {
        if (priority < g.priority)
            return true;
        return false;
    }
    bool operator>(const Node &g) const
    {
        if (priority < g.priority)
            return false;
        return true;
    }
};

class PriorityQueue
{
private:
    priority_queue<Node, vector<Node>, greater<Node>> p;

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

    void put(Node n, int priority)
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

    int first_key()
    {
        return p.top().priority;
    }

    void deleteNode(Node &n)
    {
        std::vector<Node> v;
        while (!p.empty())
        {
            Node nn = p.top();
            if (nn.x == n.x and nn.y == n.y)
                continue;
            v.push_back(nn);
            p.pop();
        }
        for (Node i : v)
        {
            p.push(i);
        }
    }
};

struct node_key
{
    int x;
    int y;
    node_key(int xx, int yy) : x(xx), y(yy) {}
    bool operator<(const node_key &g) const
    {
        if (x < g.x or
            (x == g.x and y < g.y))
            return true;
        return false;
    }
    bool operator>(const node_key &g) const
    {
        if (x < g.x or
            (x == g.x and y < g.y))
            return false;
        return true;
    }
    bool operator!=(const node_key &g) const
    {
        if (x == g.x and y == g.y)
            return false;
        return true;
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

    std::map<std::pair<int, int>, int> G_VALS;
    cout << G_VALS.count(m_p(2, 3)) << endl;
    cout << G_VALS.size() << endl;
    G_VALS[m_p(2, 3)] = 5;
    G_VALS[m_p(2, 6)] = 5;
    cout << G_VALS.size() << endl;
    cout << G_VALS.count(m_p(2, 3)) << endl;
    cout << G_VALS[m_p(2, 6)] << endl;

    return 0;
}