#ifndef __FWING_DECIDE_ACTION_DSTARLITE_V2_PRIORITYQUEUE_H__
#define __FWING_DECIDE_ACTION_DSTARLITE_V2_PRIORITYQUEUE_H__

#include "fwing_decide_action_dstarlite_v2_node_impl.h"

namespace fwing_decide_action_dstarlite_v2_impl
{
class PriorityQueue
{
private:
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> p;

public:
    PriorityQueue() {}
    bool empty()
    {
        return p.size() == 0;
    }
    int size()
    {
        return p.size();
    }

    void put(Node &n, Key &priority)
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

    void deleteNode(Node &n)
    {
        std::vector<Node> v;
        // std::cout << "         delete all nodes" << std::endl;
        while (!p.empty())
        {
            Node nn = p.top();
            if (!(nn.x == n.x and nn.y == n.y))
                v.push_back(nn);
            p.pop();
        }
        // std::cout << "         delete completed" << std::endl;

        for (Node i : v)
            p.push(i);
        v.clear();
        v.shrink_to_fit();
    }
};
}
#endif