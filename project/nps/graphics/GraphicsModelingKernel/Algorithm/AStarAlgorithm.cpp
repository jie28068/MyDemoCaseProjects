#include "AStarAlgorithm.h"

#include <memory>

AStarAlogrithm::AStarAlogrithm(QObject *parent) : IConnectorWireAlgorithm(parent) { }

void AStarAlogrithm::initAstar(char *mazeData, int width, int height)
{
    this->mazeData = mazeData;
    this->mazeWidth = width;
    this->mazeHeight = height;
}

bool AStarAlogrithm::isValid(const AStar::Node &node)
{
    // 当前点不在二位数组表示的(场景)区域中，那么就算有效，目前允许连接线超出画板区域
    if (node.x < 0 || node.y < 0 || node.x > mazeWidth || node.y > mazeHeight) {
        return true;
    }
    int index = getMazeIndex(node);
    if (index >= 0 && index < mazeWidth * mazeHeight) {
        return mazeData[index] == 0;
    }
    return false;
}

int AStarAlogrithm::getMazeIndex(const AStar::Node &node)
{
    int index = (node.y) / 10 * mazeWidth + node.x / 10;
    return index;
}
