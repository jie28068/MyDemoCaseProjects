#ifndef JSONSPLITER_H
#define JSONSPLITER_H
#include <QList>
#include <QByteArray>
#include <QtDebug>

class JsonSpliter
{
public:
    JsonSpliter();
    static QList<QByteArray> splitJsonStream(QByteArray& data)
    {
        int count=0;
        int i=0;
        int startPos;
        QList<QByteArray> retList;

        int startIndx = data.indexOf('{');
        if(startIndx == -1){
            data.clear();
            return QList<QByteArray>();
        }
        data.remove(0,startIndx);
        startPos = 0;

        for(i=0; i<data.size();i++)
        {
            if(data.at(i) == '{')
                count++;
            if(data.at(i) == '}'){
                count--;
                if(count == 0){
                    retList<<data.mid(startPos, i-startPos+1);
                    startPos = data.indexOf('{', i);
                    if(startPos == -1){
                        data.remove(0, i+1);
                        return retList;
                    }
                    count = 1;
                    i = startPos;
                }
            }
        }
        data.remove(0,startPos);
        return retList;
    }
};

#endif // JSONSPLITER_H
