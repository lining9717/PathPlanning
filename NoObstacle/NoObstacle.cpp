#include <iostream>
#include <vector>
#include <cstring>
#include <memory>
#include <fstream>

#define m_p(a, b) std::make_pair(a, b)

class NoObstacle
{
private:
    int sx, sy, gx, gy;

public:
    NoObstacle() {}
    ~NoObstacle() {}

    void findPath(int sx, int sy, int gx, int gy, std::vector<std::pair<int, int>> &result)
    {
        bool flag1 = true, flag2 = true; //flag1:sx<gx, flag2:sy<gy
        if (sx > gx)
            flag1 = false;

        if (sy > gy)
            flag2 = false;

        if (flag1 and flag2)
        {
            for (int i = sx; i <= gx; i++)
            {
                result.push_back(m_p(i, sy));
            }
            result.pop_back();

            for (int j = sy; j <= gy; j++)
            {
                result.push_back(m_p(gx, j));
            }
        }

        if (!flag1 and !flag2)
        {
            for (int i = sx; i >= gx; i--)
            {
                result.push_back(m_p(i, sy));
            }
            result.pop_back();

            for (int j = sy; j >= gy; j--)
            {
                result.push_back(m_p(gx, j));
            }
        }

        if (flag1 and !flag2)
        {
            for (int i = sx; i <= gx; i++)
            {
                result.push_back(m_p(i, sy));
            }
            result.pop_back();

            for (int j = sy; j >= gy; j--)
            {
                result.push_back(m_p(gx, j));
            }
        }

        if (!flag1 and flag2)
        {
            for (int i = sx; i >= gx; i--)
            {
                result.push_back(m_p(i, sy));
            }
            result.pop_back();

            for (int j = sy; j <= gy; j++)
            {
                result.push_back(m_p(gx, j));
            }
        }
    }
};

#ifdef __cplusplus
extern "C"
{
    int getpath(int *data, int sx, int sy, int gx, int gy)
    {
        std::shared_ptr<NoObstacle> p_noob = std::make_shared<NoObstacle>();
        std::vector<std::pair<int, int>> result;
        p_noob->findPath(sx, sy, gx, gy, result);
        int length = result.size();
        int res[2 * length] = {-1};
        int j = 0;

        for (auto i = 0; i < 2 * length; i += 2)
        {
            res[i] = result[j].first;
            res[i + 1] = result[j].second;
            j++;
        }
        std::memcpy(data, res, sizeof(res));
        return length * 2;
    }
}
#endif

int main(int argc, char const *argv[])
{
    int sx = 10;
    int sy = 0;
    int gx = 2;
    int gy = 14;
    std::shared_ptr<NoObstacle> p_noob = std::make_shared<NoObstacle>();
    std::vector<std::pair<int, int>> result;
    p_noob->findPath(sx, sy, gx, gy, result);
    int length = result.size();
    std::cout << "Final Path:[Start=";
    for (int i = 0; i < length; i++)
    {
        std::cout << "(" << result[i].first << "," << result[i].second << ")";
        if (i + 1 != length)
            std::cout << "->";
    }
    std::cout << "=Goal]" << std::endl;
    return 0;
}
