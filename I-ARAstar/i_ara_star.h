#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#define m_p(a, b) std::make_pair(a, b)
#define INF 1e8 + 7

struct SetElem
{
    int id;
    double fvalue;
    SetElem(int id, double fv) : id(id), fvalue(fv) {}
    SetElem()
    {
        id = -1;
        fvalue = INF;
    }

    bool operator<(const SetElem &d) const
    {
        return (fvalue != d.fvalue) ? (fvalue < d.fvalue) : (id < d.id);
    }
};

class I_ARAStar
{
public:
    I_ARAStar(){}

    //从文本文件输入图形并获取其大小
    void GetGraphFromTXT(const char *name);

    void OpenLogTXT(const char *logfile);
    void OpenPathsTXT(const char *pathfile);
    void OpenOpensTXT(const char *opensfile);
    void OpenCloseTXT(const char *closefile);
    void OpenInconsesTXT(const char *inconsfile);

    void StartIAra();

private:
    //创建算法所需的集合
    std::set<SetElem> OPEN;
    std::set<SetElem> CLOSED;
    std::set<SetElem> INCONS;
    std::set<int> DELETED;

    //随机产生目标点所用
    std::set<int> visited;

    //地图
    std::vector<std::vector<int>> graph;
    //四联通
    std::vector<std::pair<int, int>> neigh = {m_p(0, 1), m_p(0, -1), m_p(1, 0), m_p(-1, 0)};
    double eps_max = 1.8;
    double eps = eps_max;

    // I-ara* 步骤
    double step = 0.3;
    // 迷宫顶点编号，起始变量，上次访问
    int map_size, start, goal, last_start;
    //迷宫的大小。 输入后更改，m为列数，n为行数
    int m = 0;
    int n = 0;
    //文字图
    std::vector<std::string> lab;

    int path_len = INF;
    int old_goal = INF;
    //可视化日志
    std::ofstream logging;
    std::ofstream paths;
    std::ofstream opens;
    std::ofstream closes;
    std::ofstream inconses;
    //重定向的运行时间
    clock_t time_cur;
    //图顶点的v值和g值
    std::vector<int> v;
    std::vector<int> g;
    //还原路径的父向量
    std::vector<int> parents;
    //最后找到的路径
    std::vector<int> path;
    //迭代时间
    std::vector<int> times;
    //
    std::mt19937 rd{2019};

    //将迷宫中的坐标转换为顶点编号
    int GetIndex(int x, int y);

    //获取曼哈顿距离
    int Heruestic(int x, int y);

    //计算顶点优先级（迭代启发式）
    double GetFvalue(int i);

    //ARA*算法的迭代
    bool ImprovePath();

    //更新所选SetElem fvalue的函数
    void UpdateFvalues(std::set<SetElem> &s);

    //在更改迭代epsilon的同时启动ARA *迭代的函数
    bool ComputePath();

    //删除
    void DfsDelete(int current);

    void Step1();
    void Step2();
    void Step3();
    void Step4();

    // 获取路径
    void GetCurentPath();

    //产生目标运动
    char GetSell(int ind);

    //随机产生下一个目标点
    inline int GetNextGoal(int goal);

    int sum(std::vector<int> &v);
};