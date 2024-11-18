#include "gettexttel.h"


GetTextTel::GetTextTel()
{

}

QString GetTextTel::GetTel(QString text)
{
    int pos = 0;
    QRegExp rx("1\\d{2}[-\\s]?\\d{4}[-\\s]?\\d{4}");
    QString s;
    while ((pos = rx.indexIn(text, pos)) != -1)
    {
        s += rx.cap(0);
        pos += rx.matchedLength();
    }

    if(s.size() == 13)
    {
        s.remove(3,1);
        s.remove(7,1);
    }
    return s;
}
