#include "SimulationModelListView.h"
#include <QKeyEvent>

SimulationModelListView::SimulationModelListView(QWidget *parent) : QListWidget(parent) { }

SimulationModelListView::~SimulationModelListView() { }

void SimulationModelListView::keyPressEvent(QKeyEvent *event)
{
    QListView::keyPressEvent(event);
    event->accept();
}
