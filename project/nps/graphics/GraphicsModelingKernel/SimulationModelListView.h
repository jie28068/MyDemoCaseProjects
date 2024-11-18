#ifndef SIMULATIONMODELLISTVIEW_H
#define SIMULATIONMODELLISTVIEW_H

#include <QListWidget>

class SimulationModelListView : public QListWidget
{
    Q_OBJECT

public:
    SimulationModelListView(QWidget *parent);
    ~SimulationModelListView();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
};

#endif // SIMULATIONMODELLISTVIEW_H
