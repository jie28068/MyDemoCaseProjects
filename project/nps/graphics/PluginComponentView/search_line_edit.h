#pragma once

#include "KLineEdit.h"
#include <QLabel>
#include <QPushButton>

class SearchLineEdit : public KLineEdit
{
    Q_OBJECT
public:
    SearchLineEdit(QWidget *parent = 0);
};