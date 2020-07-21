#include <cstring>
#include <ctime>
#include "fwing_decide_action_dstarlite_v2_main_impl.h"
using namespace fwing_decide_action_dstarlite_v2_impl;

const char *map_filepath = "/home/ln/WorkSpace/c++/pathplanning/map.txt";
const int view_range = 2;

#ifdef __cplusplus
extern "C"
{
    int getpath(int *data, int sx, int sy, int gx, int gy)
    {
        std::shared_ptr<DStarLite> ptr_dsl = std::make_shared<DStarLite>();
        if (!ptr_dsl->init(m_p(sx, sy), m_p(gx, gy), map_filepath, view_range))
        {
            std::cout << "init fail!" << std::endl;
            std::cout << "start(" << sx << "," << sy << ")"<< std::endl;
            std::cout << "goal(" << gx << "," << gy << ")"<< std::endl;
            return -1;
        }
        std::vector<Node> path;
        ptr_dsl->setIsShow(false);

        int status = ptr_dsl->move_to_goal(path);
        if (status == HAVE_PATH)
        {
            int length = path.size();
            int res[2 * length] = {-1};
            int j = 0;
            for (auto i = 0; i < 2 * length; i += 2)
            {
                res[i] = int(path[j].x);
                res[i + 1] = int(path[j].y);
                j++;
            }
            std::memcpy(data, res, sizeof(res));
            return length * 2;
        }
        else
        {
            // std::cout << "No Path!" << std::endl;
            return -1;
        }
    }

    int get_time_path(double *data, int sx, int sy, int gx, int gy)
    {
        clock_t start, end;
        start = clock();
        std::shared_ptr<DStarLite> ptr_dsl = std::make_shared<DStarLite>();
        if (!ptr_dsl->init(m_p(sx, sy), m_p(gx, gy), map_filepath, view_range))
        {
            // std::cout << "init fail!" << std::endl;
            return -1;
        }
        std::vector<Node> path;
        ptr_dsl->setIsShow(false);

        int status = ptr_dsl->move_to_goal(path);
        end = clock();
        if (status == HAVE_PATH)
        {
            int length = path.size();
            double time[1] = {-1};
            int j = 0;
            time[0] = (double)(end - start) / CLOCKS_PER_SEC;
            std::memcpy(data, time, sizeof(time));
            return length;
        }
        else
        {
            // std::cout << "No Path!" << std::endl;
            return -1;
        }
    }
}

#endif

int main(int argc, char const *argv[])
{
    int sx = 56;
    int sy = 13;
    int gx = 59;
    int gy = 19;

    std::shared_ptr<DStarLite> ptr_dsl = std::make_shared<DStarLite>();
    if (!ptr_dsl->init(m_p(sx, sy), m_p(gx, gy), map_filepath, view_range))
    {
        std::cout << "init fail!" << std::endl;
        return 0;
    }

    std::vector<Node> path;
    ptr_dsl->setIsShow(true);

    int status = ptr_dsl->move_to_goal(path);
    if (status == HAVE_PATH)
    {
        int length = path.size();
        std::cout << length << std::endl;
        for (int i = 0; i < length; i++)
        {
            std::cout << "(" << path[i].x << "," << path[i].y << ")";
            if (i + 1 != length)
                std::cout << "->";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "No Path!" << std::endl;
    }

    return 0;
}
