#ifndef GETTEXTFROMPDFORWORD_H
#define GETTEXTFROMPDFORWORD_H

#include <iostream>
#include <QAxObject>
#include <QDebug>
#include "../poppler/poppler-qt5.h"

class GetaText
{
public:
    GetaText();
    ~GetaText();

static QString GetTextfromPdforWord(QString path, QString format)
{
    if(format.endsWith("pdf"))
    {
        //x64库未编译 暂时使用x32库
        QString pdf_text_info;

        Poppler::Document * document = Poppler::Document::load(path); //将pdf文件加载进Document
        if (!document || document->isLocked())
        {

          // ... error message ....

          delete document;
          return pdf_text_info;
        }

        for(int j=0;j<document->numPages();j++)
        {
            QList<Poppler::TextBox * >texts = document->page(j)->textList();//pdfPage是一个Poppler::Page的指针
            for(int i=0;i<texts.count();i++)
            {
               //qDebug() << "Text:" << texts.at(i)->text() ;//输出文本内容和文本框的大小、位置
                pdf_text_info.append(texts.at(i)->text());
            }
        }
        {   //过滤信息
         pdf_text_info.remove(QChar('\n'),Qt::CaseInsensitive);
         pdf_text_info.remove(QChar('\r'),Qt::CaseInsensitive);
         pdf_text_info.remove("ID：1WEQsevL)",Qt::CaseInsensitive);
         pdf_text_info.remove("简历下载于：2022年",Qt::CaseInsensitive);
         pdf_text_info.remove("扫码联系",Qt::CaseInsensitive);
         pdf_text_info.remove("「智联招聘App」扫码或点击此处登录查看详情点击此处联系候选",Qt::CaseInsensitive);
         pdf_text_info.remove("科梁科技有限公司",Qt::CaseInsensitive);
         pdf_text_info.remove("融合产业园",Qt::CaseInsensitive);
         pdf_text_info.remove("184726015",Qt::CaseInsensitive);
         pdf_text_info.remove("分机号",Qt::CaseInsensitive);
         pdf_text_info.remove("(该虚拟号归属地",Qt::CaseInsensitive);
         pdf_text_info.remove("失效并更新)邮箱：840060321@qq.com",Qt::CaseInsensitive);
        }
        delete document;
        qDebug()<<"Get pdf text:";
        qDebug()<<pdf_text_info;
        return  pdf_text_info;

    }
    if(format.endsWith("doc")||format.endsWith("docx"))
    {
        QAxObject  *myword;      //Word应用程序指针
        QAxObject *mydocs;      //文档集指针
        QAxObject *mydoc;       //文档指针
        QAxObject *selection;   //Selection指针;
        QString docx_text_info;
        myword = new QAxObject ("Word.Application");
        mydocs = myword->querySubObject("Documents");

        if (mydocs == nullptr)
        {
        }

        //打开一个Word文档，这里调用的是Documents对象的open方法
        mydocs->dynamicCall("Open(const QVariant&)",QVariant(path));

        //获取当前word文档对象
        mydoc = myword->querySubObject("ActiveDocument");
        if (mydoc == nullptr)
        {
            qDebug() << __func__ << "获取当前文档失败";
            return 0;
        }


        //然后获取选中的word文档内容
        int shapesCount = mydoc->querySubObject("Shapes()")->property("Count").toInt();
        if(0 == shapesCount)    //纯文本文档（不含任何控件）
        {
            //这里是把word文档的内容全部选中。
            //通过不带参数的range方法选中整个文档内容->querySubObject("Shapes(int)", 1)
            selection = mydoc->querySubObject("Range()");
            if (selection != nullptr)
            {
                docx_text_info.append(selection->property("Text").toString());
            }
        }
        else    //文本控件文档（含有控件）
        {
            for (int index = 1; index <= shapesCount; index++)
            {
                if(0 == mydoc->querySubObject("Shapes(int)", index)->property("Name").toString().indexOf("Group"))  //文档中是否有Group控件
                {
                    int itemCount = mydoc->querySubObject("Shapes(int)", index)->querySubObject("GroupItems")->property("Count").toInt();   //获取Group控件中子控件数量
                    for (int itemIndex = 1; itemIndex <= itemCount; itemIndex++)
                    {
//                        qDebug() << __func__ << "name ========= " <<  mydoc->querySubObject("Shapes(int)", index)->querySubObject("GroupItems(int)", itemIndex)->property("Name").toString();

                        if(0 == mydoc->querySubObject("Shapes(int)", index)->querySubObject("GroupItems(int)", itemIndex)->property("Name").toString().indexOf("文本框")
                                || 0 == mydoc->querySubObject("Shapes(int)", index)->querySubObject("GroupItems(int)", itemIndex)->property("Name").toString().indexOf("Text Box"))  //Group控件的子控件中是否有文本框（.doc）或者TextBox（.docx）控件
                        {
                            QAxObject *Shapes = mydoc->querySubObject("Shapes(int)", index)->querySubObject("GroupItems(int)", itemIndex)->querySubObject("TextFrame");//获取文本框（.doc）或者TextBox（.docx）控件对象
                            if (Shapes == nullptr)
                            {
                                qDebug() << __func__ <<"Shapes is null";
                                return 0;
                            }
                            QAxObject *TextFrame = Shapes->querySubObject("ContainingRange");//获取文本对象
                            if (TextFrame != nullptr)
                            {
                                docx_text_info.append(TextFrame->property("Text").toString());  //通过文本对象获取文本框（.doc）或者TextBox（.docx）控件中的文字
                            }
                        }
                    }
                }
                else if(0 == mydoc->querySubObject("Shapes(int)", index)->property("Name").toString().indexOf("文本框")
                        || 0 == mydoc->querySubObject("Shapes(int)", index)->property("Name").toString().indexOf("Text Box"))//文档中是否有文本框（.doc）或者TextBox（.docx）控件
                {
                    QAxObject *Shapes = mydoc->querySubObject("Shapes(int)", index)->querySubObject("TextFrame");//获取文本框（.doc）或者TextBox（.docx）控件对象
                    if (Shapes == nullptr)
                    {
                        qDebug() << __func__ <<"Shapes is null";
                        return 0;
                    }
                    QAxObject *TextFrame = Shapes->querySubObject("ContainingRange");   //获取文本对象
                    if (TextFrame != nullptr)
                    {
                        docx_text_info.append(TextFrame->property("Text").toString());//通过文本对象获取文本框（.doc）或者TextBox（.docx）控件中的文字
                    }
                }
                else    //含表格等其它类型文档
                {
                    selection = mydoc->querySubObject("Range()");
                    if (selection != nullptr)
                    {
                        docx_text_info.append(selection->property("Text").toString());
                    }
                }
            }
        }

                 {
                    docx_text_info.remove("", Qt::CaseInsensitive);
                     docx_text_info.remove(QChar(' '), Qt::CaseInsensitive);
                     docx_text_info.remove(QChar('\t'), Qt::CaseInsensitive);
                     docx_text_info.remove(QChar('\n'), Qt::CaseInsensitive);
                     docx_text_info.remove(QChar('\r'), Qt::CaseInsensitive);         //删除 \n \r
                 }

        //关闭文档
        mydoc->dynamicCall("Close()");
        delete mydoc;
        mydoc = NULL;

        //退出程序
        myword->dynamicCall("Quit()");

        delete myword;
        myword = NULL;

        qDebug()<<"Get word text:";
        qDebug()<<docx_text_info;
        return  docx_text_info;
    }
    return NULL;
};

};
#endif // GETTEXTFROMPDFORWORD_H
