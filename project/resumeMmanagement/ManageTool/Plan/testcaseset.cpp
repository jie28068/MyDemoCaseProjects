//#include "testcaseset.h"

//#include <QDir>
//#include <QCoreApplication>
//#include <QDebug>
//#include <QTest>

//#define TestLogDir "testlogdir"
//TestCaseSet::TestCaseSet(QObject *parent) : QObject(parent),mTestCaseResult(0)
//{
//    QDir dir;
//    if(!dir.exists(TestLogDir))
//    {
//        if(!dir.mkdir(TestLogDir))
//        {
//            qDebug()<<"create TestlogDir is faill";
//        }
//    }

//}

//void TestCaseSet::addTestCase(QObject *test)
//{
//    test->setParent( this );
//    testCaseList.append( test );
//}


//bool TestCaseSet::runTestCase()
//{
//    foreach (QObject *test, testCaseList) {
//        QStringList argument;
//        argument.append(QCoreApplication::arguments().at(0));
//        argument.append("-o");
//        argument.append(TestLogDir"/"+QString("%1").arg(test->metaObject()->className())+".txt");
//        mTestCaseResult |= QTest::qExec(test,argument);
//    }
//    return mTestCaseResult == 0;
//}
