#include "dstarlite.h"

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
