#include "i_ara_star.h"

void I_ARAStar::GetGraphFromTXT(const char *name)
{
    std::ifstream myfile(name);
    std::string input;

    while (getline(myfile, input))
    {
        lab.push_back(input);
        m = std::max(m, (int)input.size());
    }
    int i_new, j_new;
    n = lab.size();
    std::cout << "Map: Rows=" << n << ",Cols=" << m << std::endl;
    graph.resize(n * m);
    map_size = n * m;
    start = 0;
    goal = n * m - 1;

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            if (lab[i][j] != '.')
                continue;
            for (auto it : neigh)
            {
                i_new = i + it.first;
                j_new = j + it.second; 
                if (i_new > -1 and i_new < n and j_new > -1 and j_new < m)
                {
                    if (lab[i_new][j_new] == '.')
                    {
                        //每个节点存放与自身相邻的无障碍节点
                        graph[GetIndex(i, j)].push_back(GetIndex(i_new, j_new));
                    }
                }
            }
        }
    }
}

void I_ARAStar::OpenLogTXT(const char *logfile)
{
    logging.open(logfile);
}
void I_ARAStar::OpenPathsTXT(const char *pathfile)
{
    paths.open(pathfile);
}
void I_ARAStar::OpenOpensTXT(const char *opensfile)
{
    opens.open(opensfile);
}
void I_ARAStar::OpenCloseTXT(const char *closefile)
{
    closes.open(closefile);
}
void I_ARAStar::OpenInconsesTXT(const char *inconsfile)
{
    inconses.open(inconsfile);
}

int I_ARAStar::GetIndex(int x, int y)
{
    return (x * m + y);
}

int I_ARAStar::Heruestic(int x, int y)
{
    int x_x = x / m;
    int x_y = x % m;
    int y_x = y / m;
    int y_y = y % m;
    return (abs(x_x - y_x) + abs(x_y - y_y));
}

double I_ARAStar::GetFvalue(int i)
{
    return std::min((double)INF, g[i] + eps * Heruestic(i, goal));
}

bool I_ARAStar::ImprovePath()
{
    //OPEN size不为0 并且fvalue（goal）> OPEN中的最小fvalue
    while (OPEN.size() and GetFvalue(goal) > GetFvalue(OPEN.begin()->id))
    {
        SetElem s = *OPEN.begin(); //取出OPEN中最小的fvale节点（迭代器）存到CLOSE中
        CLOSED.insert(s);
        OPEN.erase(OPEN.begin()); // 删除OPEN中最小值

        v[s.id] = g[s.id];
        for (auto new_s : graph[s.id]) //访问当前节点s的邻居
        {
            //s和new_s相邻，所以距离为1
            if (g[new_s] > v[s.id] + 1)
            {
                parents[new_s] = s.id; //new_s的父节点为s
                auto next = SetElem(new_s, GetFvalue(new_s));
                if (CLOSED.find(next) == CLOSED.end()) //若new_s不在CLOSE中
                {
                    OPEN.erase(next);       //删除OPEN中的new_s,确保不在OPEN中，若存在则删除
                    g[new_s] = v[s.id] + 1; //更新g[new_s]
                    if (INCONS.find(next) == INCONS.end())
                    {
                        //第一次发现new_s，INCONS和CLSOE中都没有new_s才加入到OPEN中
                        OPEN.insert(SetElem(new_s, GetFvalue(new_s)));
                    }
                    else
                    {
                        INCONS.erase(next);
                        INCONS.insert(SetElem(new_s, GetFvalue(new_s)));
                    }
                }
                else
                {
                    CLOSED.erase(SetElem(new_s, GetFvalue(new_s)));
                    g[new_s] = v[s.id] + 1;
                    INCONS.insert(SetElem(new_s, GetFvalue(new_s)));
                }
            }
        }
    }

    if (g[goal] == INF)
    {
        return false;
    }
    return true;
}

void I_ARAStar::UpdateFvalues(std::set<SetElem> &s)
{

    std::set<SetElem> save;
    for (auto i : s)
    {
        save.insert(SetElem(i.id, GetFvalue(i.id)));
    }
    s = save;
}

bool I_ARAStar::ComputePath()
{
    while (true)
    {
        auto result = ImprovePath();
        if (result == false)
        {
            return false;
        }
        if (eps == 1)
        {
            return true;
        }
        //将INCONS中所有元素加入到OPEN中
        OPEN.insert(INCONS.begin(), INCONS.end());
        INCONS.clear();
        CLOSED.clear();
        eps = fmax(1, eps - step);
        UpdateFvalues(OPEN);
    }
}

void I_ARAStar::Step1()
{
    if (g[start] != v[start])
    {
        OPEN.erase(SetElem(start, GetFvalue(start)));
        INCONS.erase(SetElem(start, GetFvalue(start)));
        g[start] = v[start];
    }
}

void I_ARAStar::DfsDelete(int current)
{
    INCONS.erase(SetElem(current, GetFvalue(current)));
    OPEN.erase(SetElem(current, GetFvalue(current)));
    v[current] = INF;
    g[current] = INF;
    parents[current] = -1;
    DELETED.insert(current);
    for (auto i : graph[current])
    {
        if (parents[i] == current)
        {
            DfsDelete(i);
        }
    }
}

void I_ARAStar::Step2()
{
    if (start != last_start)
    {
        parents[start] = -1;
        DfsDelete(last_start);
    }
}

void I_ARAStar::Step3()
{
    for (auto i : DELETED)
    {
        for (auto j : graph[i])
        {
            if (g[i] > v[j] + 1)
            {
                g[i] = v[j] + 1;
                parents[i] = j;
            }
        }
        if (g[i] != INF)
        {
            OPEN.insert(SetElem(i, GetFvalue(i)));
        }
    }
    UpdateFvalues(INCONS);
    OPEN.insert(INCONS.begin(), INCONS.end());
    for (auto i : CLOSED)
    {
        closes << i.id << ' ';
    }
    closes << std::endl;
    CLOSED.clear();
    INCONS.clear();
    DELETED.clear();
}

void I_ARAStar::Step4()
{
    if (GetFvalue(goal) > OPEN.begin()->fvalue)
    {
        eps = eps_max;
    }
    else
    {
        eps = fmax(eps - step, 1);
    }
    UpdateFvalues(OPEN);
}

void I_ARAStar::GetCurentPath()
{
    path.clear();
    int current = goal;
    paths << current << ' ';

    while (current != start)
    {
        current = parents[current];
        paths << current << ' ';
        path.push_back(current);
    }
    path_len = path.size();
}

char I_ARAStar::GetSell(int ind)
{
    return lab[ind / m][ind % m];
}

inline int I_ARAStar::GetNextGoal(int goal)
{
    // static std::set<int> visited;
    visited.insert(goal);

    if (goal - 1 > 0 and goal - 1 < map_size and (goal % m != 0) and
        GetSell(goal - 1) == '.' and !visited.count(goal - 1) == 1)
    {
        return goal - 1;
    }
    else if (goal - n > 0 and goal - n < map_size and GetSell(goal - n) == '.' and !visited.count(goal - n) == 1)
    {
        return goal - n;
    }
    else if (goal + 1 > 0 and goal + 1 < map_size and (goal % m != m - 1) and
             GetSell(goal + 1) == '.' and !visited.count(goal + 1) == 1)
    {
        return goal + 1;
    }
    else if (goal + n > 0 and goal + n < map_size and GetSell(goal + n) == '.' and !visited.count(goal + n) == 1)
    {
        return goal + n;
    }
    return graph[goal][rd() % graph[goal].size()];
}

int I_ARAStar::sum(std::vector<int> &v)
{
    int summ = 0;
    for (auto i : v)
    {
        summ += i;
    }
    return summ;
}

void I_ARAStar::StartIAra()
{
    last_start = -1;
    // 用无穷大填充v值g值
    v.resize(map_size, INF);
    g.resize(map_size, INF);
    parents.resize(map_size, -1);
    // 运行算法
    g[start] = 0;
    OPEN.insert(SetElem(start, GetFvalue(start)));
    while (start != goal)
    {
        time_cur = clock();
        logging << start << ' ' << goal << std::endl;

        if (ComputePath() == false)
        {
            std::cout << "Get target!" << std::endl;
            break;
        }

        //获取找到的路径
        GetCurentPath();
        paths << std::endl;
        std::cout << "Find path[start:(" << start % m << "," << start / m << "), goal:("
                  << goal % m << "," << goal / m << ")]" << std::endl;
        last_start = start;
        start = *(path.rbegin() + 1); //取计算出的path中倒数第二个节点
        old_goal = goal;
        goal = GetNextGoal(goal); //获取变化后的目标点
        if (start == goal)
        {
            std::cout << "Get target!" << std::endl;
            break;
        }
        UpdateFvalues(OPEN);
        Step1();
        Step2();
        Step3();
        Step4();
        for (auto i : OPEN)
        {
            opens << i.id << ' ';
        }
        opens << std::endl;

        times.push_back(clock() - time_cur);
    }
    opens << std::endl;
    logging << start << ' ' << goal << std::endl;
    closes << std::endl;
    paths << std::endl;
    std::cout << "Number of search:" << times.size() << std::endl;
    std::cout << "Spend time:" << (double)sum(times) / 1000 << "ms" << std::endl;
}

int main(int argc, char const *argv[])
{
    I_ARAStar *i_ara_star = new I_ARAStar();
    i_ara_star->GetGraphFromTXT("./map.txt");
    i_ara_star->OpenLogTXT("./log.txt");
    i_ara_star->OpenPathsTXT("./paths.txt");
    i_ara_star->OpenOpensTXT("./opens.txt");
    i_ara_star->OpenCloseTXT("./closes.txt");
    i_ara_star->OpenInconsesTXT("./inconses.txt");

    i_ara_star->StartIAra();

    return 0;
}