#include <QCoreApplication>
#include "JlCompress.h"

/**
 * 注意(win下)：
 * 本程序调用quazip库实现压缩解压测试，实际该库底下，调用了zip库，库均位于quazipTest\quazip\lib\win。
 * zip库：zlibwapi.dll，直接使用的是release版；
 * quazip库：quazipd.dll、quazipd.lib和quazip.dll、quazip.lib，分为debug、release版；
 * quazip库分别用于程序debug、release版的链接，因为debug程序调用release版的quazip库dll的话，会报错。
 *
 * debug版程序运行时，依赖：quazipd.dll、zlibwapi.dll
 * release版程序运行时，依赖：quazip.dll、zlibwapi.dll
 *
 * 本例子中的zip库，采用VS2017编译为64位库。
 * 本例子中的quazip库，采用QtCreator+MSVC编译为64位库。
 *
 * 在linux下时，亦类似。
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 将程序当前目录进行压缩，压缩文件为myTest.zip
    JlCompress::compressDir("D:/myTest.zip", "./");

    // 将myTest.zip解压到D:/myTest目录下
    JlCompress::extractDir("D:/myTest.zip", "D:/myTest");

    return a.exec();
}
