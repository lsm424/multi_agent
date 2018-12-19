#ifndef MAINWIDGET
#define MAINWIDGET
#include "Paintarea.h"
#include <QMainWindow>
class MainWidget:public QMainWindow
{
    Q_OBJECT
public:
    MainWidget();
    Dock *dock;
    QDockWidget *dockwidget;
protected slots:
    void slotdock();
};
#endif // MAINWIDGET

