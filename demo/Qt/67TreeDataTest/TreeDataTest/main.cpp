#include <QCoreApplication>
#include <QDebug>
#include "TreeItem.h"

void print(QList<TreeItem*> items)
{
    for (int i = 0; i < items.size(); i++)
    {
        QString name = items.at(i)->name();
        qDebug() << "Item" << i << ":" << name;
    }
    qDebug() << "Print Over, Total Item Count :" << items.size();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /*
     *                China
     *      /           |                   \
     *   SiChuan      BeiJing            GuangDong
     *   /      \                 /          |         \
     * ChengDu YiBin           GuangZhou  ShenZhen   FoShan
    */
    // 四川
    TreeItem* sichuan = new TreeItem("SiChuan");
    TreeItem* chengdu = new TreeItem("ChengDu");
    TreeItem* yibin = new TreeItem("YiBin");
    sichuan->appendChild(chengdu);
    sichuan->appendChild(yibin);

    // 北京
    TreeItem* beijing = new TreeItem("BeiJing");

    // 广东
    TreeItem* guangdong = new TreeItem("GuangDong");
    TreeItem* guangzhou = new TreeItem("GuangZhou");
    TreeItem* shenzhen = new TreeItem("ShenZhen");
    TreeItem* foshan = new TreeItem("FoShan");
    guangdong->appendChild(guangzhou);
    guangdong->appendChild(shenzhen);
    guangdong->appendChild(foshan);

    // China
    TreeItem* china = new TreeItem("China");
    china->appendChild(sichuan);
    china->appendChild(beijing);
    china->appendChild(guangdong);

    // 查看China下所有节点
    QList<TreeItem*> items = china->grandchildren();
    print(items);

    // 查看SiChuan下所有节点
    items = sichuan->grandchildren();
    print(items);

    // 查看GuangDong下所有节点
    items = TreeItem::grandchildren(guangdong);
    print(items);

    delete china;
    return a.exec();
}
