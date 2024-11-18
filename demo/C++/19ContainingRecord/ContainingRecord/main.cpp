#include <QCoreApplication>
#include <QDebug>

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//

#define CONTAINING_RECORD(address, type, field) ((type *)( \
                                                 (qint8*)(address) - \
                                                 (quint64)(&((type *)0)->field)))
// 轮子
class Wheel
{
public:
    Wheel(int count, int color)
        : count(count),
          color(color)
    {}

private:
    int count; // 数量
    int color; // 颜色
};

// 汽车
class Car
{
public:
    Car(int seat, Wheel wheel)
        : seat(seat),
          wheel(wheel)
    {}

    Wheel* getWheel()
    {
        return &wheel;
    }

    // 将轮子转换为对应的汽车
    static Car *wheelToCar(Wheel *pWheel)
    {
        return CONTAINING_RECORD(pWheel, Car, wheel);
    }

private:
    int seat; // 座位数量
    Wheel wheel; // 轮子
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Wheel wheel(4, 1); // 4个轮子，1种颜色
    Car myCar(5, wheel); // 5个座位，1套轮子

    Car* pCar = Car::wheelToCar(myCar.getWheel());
    qDebug() << &myCar;
    qDebug() << pCar;

    return a.exec();
}
