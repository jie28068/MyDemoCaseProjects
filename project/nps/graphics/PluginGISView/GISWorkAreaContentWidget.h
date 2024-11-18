#pragma once

#include "CoreLib/ServerBase.h"
#include "CoreUi/Common/BasicWorkareaContentWidget.h"

class GISWorkAreaContentWidget : public BasicWorkareaContentWidget
{
    Q_OBJECT
    DEFINE_WORKAREA

public:
    GISWorkAreaContentWidget(QWidget *parent = nullptr, QString title = "");

    ~GISWorkAreaContentWidget();

private:
    void initUI();
};
