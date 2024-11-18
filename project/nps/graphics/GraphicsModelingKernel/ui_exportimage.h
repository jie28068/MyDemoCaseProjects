/********************************************************************************
** Form generated from reading UI file 'exportimage.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPORTIMAGE_H
#define UI_EXPORTIMAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExportImage
{
public:
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QGridLayout *gridLayout;
    QCheckBox *m_isfull;
    QCheckBox *m_isline;
    QCheckBox *m_isborad;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ExportImage)
    {
        if (ExportImage->objectName().isEmpty())
            ExportImage->setObjectName(QString::fromUtf8("ExportImage"));
        ExportImage->resize(507, 386);
        horizontalLayout_2 = new QHBoxLayout(ExportImage);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ExportImage);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_isfull = new QCheckBox(ExportImage);
        m_isfull->setObjectName(QString::fromUtf8("m_isfull"));

        gridLayout->addWidget(m_isfull, 2, 0, 1, 1);

        m_isline = new QCheckBox(ExportImage);
        m_isline->setObjectName(QString::fromUtf8("m_isline"));

        gridLayout->addWidget(m_isline, 0, 0, 1, 1);

        m_isborad = new QCheckBox(ExportImage);
        m_isborad->setObjectName(QString::fromUtf8("m_isborad"));

        gridLayout->addWidget(m_isborad, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 0, 1, 1);

        horizontalLayout->addLayout(gridLayout);

        horizontalLayout->setStretch(0, 5);
        horizontalLayout->setStretch(1, 1);

        horizontalLayout_2->addLayout(horizontalLayout);

        retranslateUi(ExportImage);

        QMetaObject::connectSlotsByName(ExportImage);
    } // setupUi

    void retranslateUi(QWidget *ExportImage)
    {
        ExportImage->setWindowTitle(
                QApplication::translate("ExportImage", "ExportImage", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        m_isfull->setText(
                QApplication::translate("ExportImage", "\346\225\264\345\233\276", 0, QApplication::UnicodeUTF8));
        m_isline->setText(QApplication::translate("ExportImage", "\347\275\221\346\240\274\347\272\277", 0,
                                                  QApplication::UnicodeUTF8));
        m_isborad->setText(
                QApplication::translate("ExportImage", "\350\276\271\346\241\206", 0, QApplication::UnicodeUTF8));
    } // retranslateUi
};

namespace Ui {
class ExportImage : public Ui_ExportImage
{
};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXPORTIMAGE_H
