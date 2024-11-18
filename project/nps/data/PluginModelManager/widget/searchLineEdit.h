#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
#include "KLineEdit.h"


class SearchLineEdit : public KLineEdit
{
    Q_OBJECT
public:
    SearchLineEdit(QWidget *parent = 0);
};