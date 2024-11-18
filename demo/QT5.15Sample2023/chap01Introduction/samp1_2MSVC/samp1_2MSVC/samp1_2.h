#pragma once

#include <QtWidgets/QWidget>
#include "ui_samp1_2.h"

class samp1_2 : public QWidget
{
    Q_OBJECT

public:
    samp1_2(QWidget *parent = nullptr);
    ~samp1_2();

private:
    Ui::samp1_2Class ui;
};
