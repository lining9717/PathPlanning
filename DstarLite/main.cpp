#include "dstarlite.h"

int main(int argc, char const *argv[])
{
    DStarLite dsl;
    if (!dsl.init("./map1.txt", 4))
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
    }
    else
        std::cout << "No Path!" << std::endl;

    return 0;
}
