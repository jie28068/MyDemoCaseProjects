#pragma once

#include "Global.h"
#include "IConnectorWireAlgorithm.h"

#include <QObject>
#include <math.h>
#include <queue>
#include <vector>

namespace AStar {
class Node
{
public:
    int x;
    int y;
    int g;
    int h;
    int f;
    Node *parent;

    Node(int x, int y, int g, int h, Node *parent)
    {
        this->x = x;
        this->y = y;
        this->g = g;
        this->h = h;
        this->f = g + h;
        this->parent = parent;
    }

    /// @brief 运算符重载，用于在优先级队列中排序
    /// @param other
    /// @return
    bool operator<(const Node &other) const { return f > other.f; }

    /// @brief 运算符重载，判断两个node是否相等
    /// @param other
    /// @return
    bool operator==(const Node &other) const
    {
        if (this == &other) {
            return true;
        }
        return this->x == other.x && this->y == other.y;
    }

    int distance(const Node &other) const { return std::abs(other.x - this->x) + std::abs(other.y - this->y); }

    /// @brief 定义哈希函数，用于unordered_map
    struct Hash {
        size_t operator()(const Node *node) const { return std::hash<int>()(node->x) ^ std::hash<int>()(node->y); }
    };

    struct Equal {
        bool operator()(const Node *node1, const Node *node2) const
        {
            return (node1->x == node2->x && node1->y == node2->y);
        }
    };
};

};

class AStarAlogrithm : public IConnectorWireAlgorithm
{
    Q_OBJECT
public:
    AStarAlogrithm(QObject *parent = nullptr);

    void initAstar(char *mazeData, int width, int height);

private:
    bool isValid(const AStar::Node &node);

    int getMazeIndex(const AStar::Node &node);

private:
    /// @brief 抽象的网格数据，包含了障碍物数据。如果对应的索引值为1表示此处有障碍物
    char *mazeData;

    /// @brief mazeData数组的大小
    int mazeWidth;

    int mazeHeight;

    std::priority_queue<AStar::Node *> openNodes;
};