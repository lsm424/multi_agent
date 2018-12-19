#include"mainwidget.h"
MainWidget::MainWidget()
{
    dock=new Dock;

    dockwidget=new QDockWidget(this);
    dockwidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
    dockwidget->setWidget(dock);
    addDockWidget(Qt::BottomDockWidgetArea,dockwidget);
    setCentralWidget(dock->paintarea);
    dock->paintarea->setParent(this);
    connect(dock->paintarea->button,SIGNAL(clicked()),this,SLOT(slotdock()));
    setWindowTitle("基于盲区的通信量减少的多智能体有限时间编队");
}
void MainWidget::slotdock()
{
    if(dock->paintarea->button->text()==QString("开始"))
    {
        dock->slotstart();
        dock->paintarea->button->setText("停止");
        dock->close();
    }
    else
    {
        dock->show();
        dock->stoprun();
        dock->paintarea->button->setText("开始");
    }
}

