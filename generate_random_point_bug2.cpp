#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>
#include <set>
#include <cstring>
#include <ctime>
#include <memory>

#define m_p(a, b) std::make_pair(a, b)

#define BUG 5
#define PATH 2
#define START 3
#define GOAL 4
#define MLINE 6
#define WALL 1
#define PASSABLE 0

#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

#define BUG1 0x6666
#define BUG2 0x6667

class Grid
{
private:
    int *gridArr;
    int width = 0;
    int length = 0;

public:
    Grid() {}
    Grid(const char *name)
    {
        std::vector<std::string> lab;
        std::ifstream myfile(name);
        std::string input;

        while (getline(myfile, input))
        {
            lab.push_back(input);
            this->width = std::max(this->width, (int)input.size());
        }
        this->length = lab.size();
        gridArr = new int[width * length];
        for (int i = 0; i < length; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                if (lab[i][j] == '.')
                    setValue(i, j, PASSABLE);
                if (lab[i][j] == '#')
                    setValue(i, j, WALL);
            }
        }
    }
    int getWidth()
    {
        return this->width;
    }
    int getLength()
    {
        return this->length;
    }
    int index(int row, int col)
    {
        return col + row * width;
    }
    void setValue(int row, int col, int value)
    {
        gridArr[index(row, col)] = value;
    }
    int getValue(int row, int col)
    {
        return gridArr[index(row, col)];
    }
    static int getOppositeCoordinate(int coordinate)
    {
        switch (coordinate)
        {
        case NORTH:
            return SOUTH;
        case SOUTH:
            return NORTH;
        case EAST:
            return WEST;
        case WEST:
            return EAST;
        default:
            return -1;
        }
    }
    static int getNextCoordinate(int coordinate)
    {
        switch (coordinate)
        {
        case NORTH:
            return EAST;
        case EAST:
            return SOUTH;
        case SOUTH:
            return WEST;
        case WEST:
            return NORTH;
        default:
            return -1;
        }
    }
    static int getPrevCoordinate(int coordinate)
    {
        switch (coordinate)
        {
        case NORTH:
            return WEST;
        case EAST:
            return NORTH;
        case SOUTH:
            return EAST;
        case WEST:
            return SOUTH;
        default:
            return -1;
        }
    }
    void printGrid()
    {
        for (int i = 0; i < this->length; i++)
        {
            for (int j = 0; j < this->width; j++)
            {
                if (gridArr[index(i, j)] == WALL)
                    std::cout << "# ";
                else if (gridArr[index(i, j)] == PASSABLE)
                    std::cout << ". ";
                else if (gridArr[index(i, j)] == BUG)
                    std::cout << "A ";
                else if (gridArr[index(i, j)] == GOAL)
                    std::cout << "Z ";
                else if (gridArr[index(i, j)] == PATH)
                    std::cout << "@ ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

class Bug
{

public:
    Grid grid;
    int stepsMoved = 0;
    int rowPos = 0, colPos = 0;
    int goalRow, goalCol;
    std::vector<std::pair<int, int>> path;
    bool displayGrid = false;
    void printGrid()
    {
        // std::cout << "goal:(" << goalRow << "," << goalCol << ")" << std::endl;
        if (displayGrid)
        {
            grid.printGrid();
        }
    }
    int pathVal = PATH;
    Bug() {}
    Bug(int rowPos, int colPos, int goalRow, int goalCol, Grid &grid)
    {
        this->rowPos = rowPos;
        this->colPos = colPos;
        this->goalRow = goalRow;
        this->goalCol = goalCol;
        this->grid = grid;
        // path.push_back(m_p(rowPos, colPos));
        grid.setValue(rowPos, colPos, BUG);
        grid.setValue(goalRow, goalCol, GOAL);

        printGrid();
    }

    void setGoal(int x, int y)
    {
        grid.setValue(goalRow, goalCol, PASSABLE);
        this->goalRow = x;
        this->goalCol = y;
        grid.setValue(goalRow, goalCol, GOAL);
    }

    void printpath()
    {
        for (std::pair<int, int> p : path)
        {
            std::cout << "(" << p.first << "," << p.second << ")"
                      << "--->";
        }
    }

    std::pair<int, int> move(int direction, int flag)
    {
        if (flag == BUG2)
        {
            grid.setValue(rowPos, colPos, pathVal);
        }

        switch (direction)
        {
        case NORTH:
            if (rowPos == grid.getLength())
            {
                break;
            }
            rowPos++;
            break;
        case EAST:
            if (colPos == grid.getWidth())
            {
                break;
            }
            colPos++;
            break;
        case SOUTH:
            if (rowPos == 0)
            {
                break;
            }
            rowPos--;
            break;
        case WEST:
            if (colPos == 0)
            {
                break;
            }
            colPos--;
            break;
        default:
            break;
        }
        if (flag == BUG2)
        {
            stepsMoved++;
            grid.setValue(rowPos, colPos, BUG);
            path.push_back(m_p(rowPos, colPos));
            printGrid();
        }

        return m_p(rowPos, colPos);
    }
    int sense(int direction)
    {
        switch (direction)
        {
        case NORTH:
            if (rowPos == grid.getLength())
            {
                break;
            }
            return grid.getValue(rowPos + 1, colPos);
        case EAST:
            if (colPos == grid.getWidth())
            {
                break;
            }
            return grid.getValue(rowPos, colPos + 1);
        case SOUTH:
            if (rowPos == 0)
            {
                break;
            }
            return grid.getValue(rowPos - 1, colPos);
        case WEST:
            if (colPos == 0)
            {
                break;
            }
            return grid.getValue(rowPos, colPos - 1);
        default:
            return -1;
        }
        return -1;
    }
    int calculateDirection()
    {
        int xDist = goalCol - colPos;
        int yDist = goalRow - rowPos;

        int xChanges[5] = {0, 0, -1, 0, 1};
        int yChanges[5] = {0, -1, 0, 1, 0};
        double shortestDist = sqrt(xDist * xDist + yDist * yDist);
        int direction = 0;
        for (int i = 0; i < 5; i++)
        {
            if (sqrt((xDist + xChanges[i]) * (xDist + xChanges[i]) + (yDist + yChanges[i]) * (yDist + yChanges[i])) < shortestDist)
            {
                shortestDist = sqrt((xDist + xChanges[i]) * (xDist + xChanges[i]) + (yDist + yChanges[i]) * (yDist + yChanges[i]));
                direction = i;
            }
        }

        return direction;
    }
    double calculateDistance()
    {
        int xDist = goalRow - rowPos;
        int yDist = goalCol - colPos;
        return sqrt(xDist * xDist + yDist * yDist);
    }
    void setDisplayGrid(bool displayGrid)
    {
        this->displayGrid = displayGrid;
    }
    int getStepsMoved()
    {
        return stepsMoved;
    }
    int getRowPos()
    {
        return rowPos;
    }
    int getColPos()
    {
        return colPos;
    }
    int getPathVal()
    {
        if (displayGrid)
        {
            grid.printGrid();
        }
    }
    void setPathVal(int pathVal)
    {
        this->pathVal = pathVal;
    }
};

class DecideRandomTarget
{
public:
    Grid bugGrid;
    Bug bug;
    bool onWall;
    bool is_first_while = true;
    bool flag = true;
    int direction;
    bool atShortest;
    int startRow;
    int startCol;
    int is_at_start = 1;
    bool is_back = false;
    double shortestDistance;
    int shortestRow;
    int shortestCol;

    int nextDirection;
    int prevDirection;
    int oppDirection;
    DecideRandomTarget(int startx, int starty, const char *name)
    {
        bugGrid = Grid(name);
        int goalRow = 0, goalCol = bugGrid.getWidth() - 1;
        // std::cout << goalRow << "," << goalCol << std::endl;
        int startRow = startx, startCol = starty;
        bug = Bug(startRow, startCol, goalRow, goalCol, bugGrid);
        bug.setDisplayGrid(false);
        direction = bug.calculateDirection();
        onWall = bug.sense(direction) == 1;
    }
    ~DecideRandomTarget() {}

    void setGoal(int x, int y)
    {
        bug.setGoal(x, y);
    }

    std::pair<int, int> getNextGoal()
    {
        std::pair<int, int> result = m_p(-1, -1);
        if (bug.calculateDirection() == 0)
        {
            if (flag)
            {
                bug.goalCol = 0;
                bug.goalRow = 0;
            }
            else
            {
                bug.goalCol = bugGrid.getWidth() - 1;
                bug.goalRow = 0;
            }
            flag = !flag;
        }
        while (bug.calculateDirection() != 0)
        {
            if (onWall)
            {
                if (is_at_start == 1)
                {
                    atShortest = false;
                    startRow = bug.getRowPos();
                    startCol = bug.getColPos();

                    shortestDistance = bug.calculateDistance();
                    shortestRow = bug.getRowPos();
                    shortestCol = bug.getColPos();

                    nextDirection = Grid::getNextCoordinate(direction);
                    prevDirection = Grid::getPrevCoordinate(direction);
                    oppDirection = Grid::getOppositeCoordinate(direction);
                    is_at_start = 2;
                    is_back = false;
                }

                //逆时针绕圈
                while (is_first_while)
                {
                    if (bug.getRowPos() == startRow && bug.getColPos() == startCol)
                    {
                        if (is_back)
                        {
                            is_first_while = false;
                            break;
                        }
                        else
                            is_back = !is_back;
                    }
                    if (bug.sense(direction) == 1 && bug.sense(nextDirection) != 1)
                    {
                        double dist = bug.calculateDistance();
                        if (dist < shortestDistance)
                        {
                            shortestDistance = dist;
                            shortestRow = bug.getRowPos();
                            shortestCol = bug.getColPos();
                        }
                        result = bug.move(nextDirection, BUG1);
                    }
                    else if (bug.sense(direction) != 1)
                    {
                        double dist = bug.calculateDistance();
                        if (dist < shortestDistance)
                        {
                            shortestDistance = dist;
                            shortestRow = bug.getRowPos();
                            shortestCol = bug.getColPos();
                        }
                        result = bug.move(direction, BUG1);
                        nextDirection = direction;
                        oppDirection = Grid::getNextCoordinate(direction);
                        prevDirection = Grid::getOppositeCoordinate(direction);
                        direction = Grid::getPrevCoordinate(direction);
                    }
                    else if (bug.sense(direction) == 1 && bug.sense(nextDirection) == 1 && bug.sense(oppDirection) != 1)
                    {
                        double dist = bug.calculateDistance();
                        if (dist < shortestDistance)
                        {
                            shortestDistance = dist;
                            shortestRow = bug.getRowPos();
                            shortestCol = bug.getColPos();
                        }
                        result = bug.move(oppDirection, BUG1);
                        nextDirection = Grid::getOppositeCoordinate(direction);
                        oppDirection = Grid::getPrevCoordinate(direction);
                        prevDirection = direction;
                        direction = Grid::getNextCoordinate(direction);
                    }
                    else if (bug.sense(direction) == 1 && bug.sense(nextDirection) == 1 && bug.sense(oppDirection) == 1)
                    {
                        double dist = bug.calculateDistance();
                        if (dist < shortestDistance)
                        {
                            shortestDistance = dist;
                            shortestRow = bug.getRowPos();
                            shortestCol = bug.getColPos();
                        }
                        result = bug.move(prevDirection, BUG1);
                        nextDirection = Grid::getPrevCoordinate(direction);
                        prevDirection = Grid::getNextCoordinate(direction);
                        oppDirection = direction;
                        direction = Grid::getOppositeCoordinate(direction);
                    }
                }

                //到达距离目标最短的点
                while (!atShortest)
                {
                    if (bug.sense(direction) == 1 && bug.sense(nextDirection) != 1 && !atShortest)
                    {
                        result = bug.move(nextDirection, BUG2);
                        if (bug.getColPos() == shortestCol && bug.getRowPos() == shortestRow)
                            atShortest = true;
                        return result;
                    }
                    else if (bug.sense(direction) != 1 && !atShortest)
                    {

                        result = bug.move(direction, BUG2);
                        if (bug.getColPos() == shortestCol && bug.getRowPos() == shortestRow)
                            atShortest = true;
                        nextDirection = direction;
                        oppDirection = Grid::getNextCoordinate(direction);
                        prevDirection = Grid::getOppositeCoordinate(direction);
                        direction = Grid::getPrevCoordinate(direction);
                        return result;
                    }
                    else if (bug.sense(direction) == 1 && bug.sense(nextDirection) == 1 && bug.sense(oppDirection) != 1 && !atShortest)
                    {
                        double dist = bug.calculateDistance();
                        if (bug.getColPos() == shortestCol && bug.getRowPos() == shortestRow)
                            atShortest = true;
                        result = bug.move(oppDirection, BUG2);
                        nextDirection = Grid::getOppositeCoordinate(direction);
                        oppDirection = Grid::getPrevCoordinate(direction);
                        prevDirection = direction;
                        direction = Grid::getNextCoordinate(direction);
                        return result;
                    }
                    else if (bug.sense(direction) == 1 && bug.sense(nextDirection) == 1 && bug.sense(oppDirection) == 1)
                    {

                        result = bug.move(prevDirection, BUG2);
                        if (bug.getColPos() == shortestCol && bug.getRowPos() == shortestRow)
                            atShortest = true;
                        nextDirection = Grid::getPrevCoordinate(direction);
                        prevDirection = Grid::getNextCoordinate(direction);
                        oppDirection = direction;
                        direction = Grid::getOppositeCoordinate(direction);
                        return result;
                    }
                }
                if (atShortest == true)
                {
                    is_first_while = true;
                    onWall = false;
                    is_at_start = 1;
                }
            }
            else
            {
                direction = bug.calculateDirection();
                if (bug.sense(direction) == 1)
                {
                    onWall = true;
                    continue;
                }
                result = bug.move(direction, BUG2);
                return result;
            }
        }
    }
};

#ifdef __cplusplus
extern "C"
{
    int get_target_path(int *data, int sx, int sy)
    {
        std::vector<int> path;
        std::shared_ptr<DecideRandomTarget> diar = std::make_shared<DecideRandomTarget>(sy + 1, sx + 1, "/home/ln/WorkSpace/c++/pathplanning/map_random.txt");
        diar->setGoal(20, 60);
        std::pair<int, int> po = diar->getNextGoal();
        path.push_back(po.second - 1);
        path.push_back(po.first - 1);
        while (po != m_p(20, 60))
        {

            po = diar->getNextGoal();
            if (po.first == 0 or po.first == 21 or po.second == 0 or po.second == 61)
                continue;
            path.push_back(po.second - 1);
            path.push_back(po.first - 1);
        }
        int length = path.size();
        int res[length] = {-1};
        for (auto i = 0; i < length; i++)
            res[i] = path[i];
        std::memcpy(data, res, sizeof(res));
        return length;
    }
    //g++ -o bug2_time.so  -std=c++11 -lpthread -shared -fPIC generate_random_point_bug2.cpp 
    int get_time_path(double *data, int sx, int sy, int gx, int gy)
    {
        clock_t start, end;
        start = clock();
        std::vector<int> path;
        std::shared_ptr<DecideRandomTarget> diar = std::make_shared<DecideRandomTarget>(sy + 1, sx + 1, "/home/ln/WorkSpace/c++/pathplanning/map_random.txt");
        diar->setGoal(gy + 1, gx + 1);
        std::pair<int, int> po = diar->getNextGoal();
        if (po == m_p(-1, -1))
        {
            return -1;
        }
        path.push_back(po.second - 1);
        path.push_back(po.first - 1);
        while (po != m_p(gy + 1, gx + 1))
        {

            po = diar->getNextGoal();
            if (po == m_p(-1, -1))
            {
                return -1;
            }
            if (po.first == 0 or po.first == 60 or po.second == 0 or po.second == 60)
                continue;
            path.push_back(po.second - 1);
            path.push_back(po.first - 1);
            end = clock();
            if ((double)(end - start) / CLOCKS_PER_SEC >= 3.0)
            {
                return -1;
            }
        }
        end = clock();
        int length = path.size();
        double res[1] = {-1};
        res[0] = (double)(end - start) / CLOCKS_PER_SEC;
        std::memcpy(data, res, sizeof(res));
        return length;
    }
}
#endif

int main(int argc, char const *argv[])
{
    DecideRandomTarget *diar = new DecideRandomTarget(17 + 1, 17 + 1, "/home/ln/WorkSpace/c++/pathplanning/map_random.txt");
    diar->setGoal(20, 60);
    std::pair<int, int> po = diar->getNextGoal();
    int count = 0;
    std::cout << "(" << po.second - 1 << "," << po.first - 1 << ")"
              << ",";
    while (po != m_p(20, 60))
    {
        count++;
        po = diar->getNextGoal();
        if (po.first == 0 or po.first == 21 or po.second == 0 or po.second == 61)
            continue;
        std::cout << "(" << po.second - 1 << "," << po.first - 1 << ")"
                  << ",";
        // if (count-- == 0)
        // {
        //     break;
        // }
    }
    std::cout << std::endl;
    return 0;
}
