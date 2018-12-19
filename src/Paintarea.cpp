#include "Paintarea.h"
int n;//智能体的个数
QVector<Agent> agents;//智能体向量

QVector<QColor> colors;//智能体的颜色
//二维数据类型构造函数
DPoint::DPoint(double xx, double yy)
{
    x=xx;
    y=yy;
}
//重载运算符+
DPoint DPoint::operator +(DPoint p2)
{
    return  DPoint(x+p2.x,y+p2.y);
}
//重载运算符-
DPoint DPoint::operator -(DPoint p2)
{
    return  DPoint(x-p2.x,y-p2.y);
}
//重载运算符*
DPoint DPoint::operator *(double i)
{
    return  DPoint(x*i,y*i);
}
//重载运算符/
DPoint DPoint::operator /(double i)
{
    return  DPoint(x/i,y/i);
}
//重载运算符-
DPoint DPoint::operator -()
{
    return  DPoint(-x,-y);
}
//重载运算符!=
bool DPoint::operator !=(DPoint p)
{
    return  x!=p.x||y!=p.y;
}
//画布类构造函数
Paintarea::Paintarea(QWidget *parent):QWidget(parent)
{
    virmode=false;
    button=new QPushButton("开始",this);
    setPalette(QPalette(Qt::white));
    setAutoFillBackground(true);
    show();
}

//重新实现 重画事件函数
void Paintarea::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(0, 127, 127, 191));
    painter.setPen(pen);
    painter.drawLine(0,height()/2,0,-height()/2);
    painter.drawLine(-width()/2,0,width()/2,0);
    pen.setWidth(4);
    QRect rect;
    int n=agents.size();
    for(int i=0;i<n;i++)
    {
        pen.setColor(colors[i]);
        painter.setPen(pen);
        rect.setRect(agents[i].pos.x-3,-(agents[i].pos.y+3),7,7);
        painter.drawEllipse(rect);
    }
    //如果是多虚拟领导者模式，则显示虚拟中心
    if(virmode)
    {
        pen.setColor(QColor(0,0,0));
        painter.setPen(pen);
        rect.setRect(posC_now.x-3,-(posC_now.y+3),7,7);
        painter.drawEllipse(rect);
    }
}

Dock::Dock()
{
    upperlayout=new QHBoxLayout;
    poslayout=new QHBoxLayout;
    vellayout=new QHBoxLayout;
    lowlayout=new QHBoxLayout;
    downlayout=new QGridLayout;
    mainlayout=new QVBoxLayout(this);
    mainlayout->addLayout(upperlayout);
    mainlayout->addLayout(lowlayout);
    mainlayout->addLayout(downlayout);
    mainlayout->addLayout(poslayout);
    mainlayout->addLayout(vellayout);
    QTime t=QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);
    select=0;

    /****   顶层布局   ****/
    systemLabel=new QLabel("仿真系统选择");
    systemComboBox=new QComboBox;
    systemComboBox->addItem("基于人工势场的多智能体编队");
    systemComboBox->addItem("基于多虚拟领导者的多智能体编队");
    systemComboBox->addItem("基于盲区的通信量减少的多智能体编队");
    systemComboBox->addItem("基于多虚拟领导者的多智能体有限时间编队");
    connect(systemComboBox,SIGNAL(activated(int)),this,SLOT(slotdij(int)));
    upperlayout->addWidget(systemLabel);
    upperlayout->addWidget(systemComboBox);

    lastn=n=6;
    nLabel=new QLabel("智能体个数");
    nLineEdit=new QLineEdit(QString::number(n));
    connect(nLineEdit,SIGNAL(returnPressed()),this,SLOT(slotn()));
    upperlayout->addWidget(nLabel);
    upperlayout->addWidget(nLineEdit);

    step=10;
    stepLabel=new QLabel("步长(ms)");
    stepLineEdit=new QLineEdit(QString::number(step));
    upperlayout->addWidget(stepLabel);
    upperlayout->addWidget(stepLineEdit);

    runtimeLabel=new QLabel("仿真时间(s)");
    runtimeLineEdit=new QLineEdit("0");
    runtimeLineEdit->setEnabled(false);
    upperlayout->addWidget(runtimeLabel);
    upperlayout->addWidget(runtimeLineEdit);

    VoLabel=new QLabel("目标速度");
    VoLineEdit=new QLineEdit("15,15");
    connect(VoLineEdit,SIGNAL(editingFinished()),this,SLOT(slotVmotion()));
    upperlayout->addWidget(VoLabel);
    upperlayout->addWidget(VoLineEdit);

    startorstopPushButton=new QPushButton("开始仿真");
    connect(startorstopPushButton,SIGNAL(clicked()),this,SLOT(slotstart()));
    upperlayout->addWidget(startorstopPushButton);

    /*****   下层布局   *****/
    edijLabel=new QLabel;
    recoverPushButton=new QPushButton("上一次的初始状态");
    connect(recoverPushButton,SIGNAL(clicked()),this,SLOT(slotrecover()));
    K1Label=new QLabel("Kv");
    K1LineEdit=new QLineEdit("1");
    K2Label=new QLabel("Kp");
    K2LineEdit=new QLineEdit("1");
    lowlayout->addWidget(edijLabel);
    lowlayout->addSpacing(6);
    lowlayout->addWidget(recoverPushButton);
    lowlayout->addWidget(K1Label);
    lowlayout->addWidget(K1LineEdit);
    lowlayout->addWidget(K2Label);
    lowlayout->addWidget(K2LineEdit);

    /*****   低层布局   *****/
    paintarea=new Paintarea;
    posLabel=NULL;
    dijLabel=NULL;
    slotn();
    //关联定时器超时函数
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(slotttimeout()));
}

//为智能体随机生成位置
void Dock::slotpos()
{
    int x,y;
    n=nLineEdit->text().toInt();

    for(int i=0;i<n;i++)
    {
        x=qrand()%paintarea->width()-paintarea->width()/2;
        y=qrand()%paintarea->height()-paintarea->height()/2;
        posLineEdit[i].setText(QString::number(x)+','+QString::number(y));
        agents[i].pos.x=x;
        agents[i].pos.y=y;
        backup_agents[i].pos=agents[i].pos;
    }
    recoverPushButton->setEnabled(true);
    paintarea->update();
}
//为智能体随机生成速度
void Dock::slotvel()
{
    int x,y;
    n=nLineEdit->text().toInt();
    for(int i=0;i<n;i++)
    {
        if(systemComboBox->currentIndex()!=2)
        {
            x=qrand()%20;
            y=qrand()%20;
        }
        else
            x=y=0;
        velLineEdit[i].setText(QString::number(x)+","+QString::number(y));
        agents[i].velocity.x=x;
        agents[i].velocity.y=y;
        backup_agents[i].velocity=agents[i].velocity;
    }
    recoverPushButton->setEnabled(true);
}
//智能体个数变化时对应的框架变化
void Dock::slotn()
{
    int a,b,c;
    //清理所有widget框架
    if(posLabel)
    {
        delete posPushButton;
        delete velPushButton;
        delete pLabel;
        delete vLabel;
        for(int i=0;i<n;i++)
            posLabel->removeEventFilter(this);
        delete[] posLabel;
        delete[] posLineEdit;
        delete[] velLabel;
        delete[] velLineEdit;
    }
    //重新生成框架
    n=nLineEdit->text().toInt();
    agents.resize(n);
    backup_agents.resize(n);
    colors.resize(n);
    posLabel=new QLabel[n];
    posLineEdit=new QLineEdit[n];
    velLabel=new QLabel[n];
    velLineEdit=new QLineEdit[n];
    pLabel=new QLabel("各智能体位置");
    poslayout->addWidget(pLabel);
    vLabel=new QLabel("各智能体速度");
    vellayout->addWidget(vLabel);
    for(int i=0;i<n;i++)
    {
        //随机生成颜色
        a=qrand()%256;
        b=qrand()%256;
        c=qrand()%256;
        colors[i].setRgb(a,b,c);
        //添加初始位置布局框
        posLabel[i].setFrameStyle(QFrame::Panel|QFrame::Sunken);
        posLabel[i].setAutoFillBackground(true);
        posLabel[i].setPalette(QPalette(colors[i]));
        //posLabel[i].installEventFilter(this);
        poslayout->addWidget(posLabel+i);
        connect(posLineEdit+i,SIGNAL(editingFinished()),this,SLOT(slotposLineEdit()));
        poslayout->addWidget(posLineEdit+i);
        //添加初始速度布局框
        velLabel[i].setFrameStyle(QFrame::Panel|QFrame::Sunken);
        velLabel[i].setAutoFillBackground(true);
        velLabel[i].setPalette(QPalette(colors[i]));
        vellayout->addWidget(velLabel+i);
        vellayout->addWidget(velLineEdit+i);
    }
    posPushButton=new QPushButton("随机生成位置");
    connect(posPushButton,SIGNAL(clicked()),this,SLOT(slotpos()));
    poslayout->addWidget(posPushButton);
    velPushButton=new QPushButton("随机生成速度");
    connect(velPushButton,SIGNAL(clicked()),this,SLOT(slotvel()));
    vellayout->addWidget(velPushButton);
    slotpos();
    slotvel();
    slotdij(systemComboBox->currentIndex());
    lastn=n;
}
//初始位置设置框内容改变
void Dock::slotposLineEdit()
{
    QStringList sl;
    for(int i=0;i<n;i++)
    {
        sl=posLineEdit[i].text().split(',');
        agents[i].pos.x=sl.at(0).toInt();
        agents[i].pos.y=sl.at(1).toInt();
    }
    paintarea->update();
}
//理想距离设置布局框搭建
void Dock::slotdij(int choice)
{
    if(choice==select&&n==lastn&&dijLabel)
        return;
    //清理上一次遗留的布局框
    if(dijLabel)
    {
        int len;
        if(select==1||select==3)//如果上一次选择的系统是多虚拟领导者模型
        {
            len=1;
            delete RLabel;
            delete RLineEdit;
        }
        else if(select==0||select==2)
        {
            len=lastn;
            if(select==2)
            {
                delete blind_area_Label;
                delete blind_area_LineEdit;
                delete communication_per_Label;
                delete communication_per_LineEdit;
            }
        }
        for(int i=0;i<len;i++)
        {
            delete []dijLabel[i];
            delete []dijLineEdit[i];
        }
        delete []dijLabel;
        delete []dijLineEdit;
    }
    velPushButton->setEnabled(true);
    //重新分配布局
    if(choice==0||choice==2)
    {
        paintarea->virmode=false;
        VoLabel->setText("目标速度");
        //基于人工势场的多智能体编队
        if(choice==0)
        {
            edijLabel->setText("下面请输入各智能体间的理想距离");
            K1Label->setText("Kv");
            K1LineEdit->setText("1");
            K2Label->setText("Kp");
            K2LineEdit->setText("1");
            K2LineEdit->setEnabled(true);
            VoLineEdit->setText("15,15");
            VoLineEdit->setEnabled(true);
        }
        //基于盲区的通信量减少的多智能体编队
        else if(choice==2)
        {
            edijLabel->setText("下面请输入各智能体间在x轴和y轴方向上的理想位移");
            K1Label->setText("a");
            K1LineEdit->setText("0.8");
            communication_per_Label=new QLabel("单位时间通信次数:");
            communication_per_LineEdit=new QLineEdit("0");
            communication_per_LineEdit->setEnabled(false);
            lowlayout->addWidget(communication_per_Label);
            lowlayout->addWidget(communication_per_LineEdit);
            blind_area_Label=new QLabel("盲区半径R=");
            blind_area_LineEdit=new QLineEdit("300-60*t");
            lowlayout->addWidget(blind_area_Label);
            lowlayout->addWidget(blind_area_LineEdit);
            K2Label->setText("通信次数");
            K2LineEdit->setText("0");
            K2LineEdit->setEnabled(false);
            VoLineEdit->setText("");
            VoLineEdit->setEnabled(false);
            for(int i=0;i<n;i++)
                velLineEdit[i].setText("0,0");
            velPushButton->setEnabled(false);
        }
        dijLabel=new QLabel*[n];
        dijLineEdit=new QLineEdit*[n];
        for(int i=0;i<n;i++)
        {
            dijLabel[i]=new QLabel[n];
            dijLineEdit[i]=new QLineEdit[n];
            for(int j=i+1;j<n;j++)
            {
                dijLabel[i][j].setText("d"+QString::number(i+1)+QString::number(j+1));
                downlayout->addWidget(dijLabel[i]+j,i,2*j);
                if(choice==0)
                    dijLineEdit[i][j].setText("100");
                else if(choice==2)
                {
                    DPoint p;
                    p.x=80*(cos(j*2*PI/n)-cos(i*2*PI/n));
                    p.y=80*(sin(j*2*PI/n)-sin(i*2*PI/n));
                    dijLineEdit[i][j].setText(QString::number(p.x)+","+QString::number(p.y));
                }
                downlayout->addWidget(dijLineEdit[i]+j,i,2*j+1);
            }
        }
    }

    //基于多虚拟领导者的多智能体编队
    else if(choice==1||choice==3)
    {
        paintarea->virmode=true;
        edijLabel->setText("下面请输入各虚拟领导者相对于虚拟中心的偏移，格式为(距离，偏移角)");
        if(choice==1)
        {
            K1Label->setText("K1");
            K1LineEdit->setText("1");
            K2Label->setText("K2");
            K2LineEdit->setText("1");
            K2LineEdit->setEnabled(true);
        }
        else
        {
            K1Label->setText("a1");
            k1=0.8;
            K1LineEdit->setText(QString::number(k1));
            connect(K1LineEdit,SIGNAL(editingFinished()),this,SLOT(slota1()));
            K2Label->setText("a2");
            k2=2*k1/(k1+1);
            K2LineEdit->setText(QString::number(k2));
            K2LineEdit->setEnabled(false);
        }
        VoLabel->setText("虚拟中心轨迹");
        VoLineEdit->setText("t^2,20*sin(t^2)");
        VoLineEdit->setEnabled(true);
        R=20;
        RLabel=new QLabel("通信半径");
        RLineEdit=new QLineEdit(QString::number(R));
        lowlayout->addWidget(RLabel);
        lowlayout->addWidget(RLineEdit);
        dijLabel=new QLabel*;
        dijLabel[0]=new QLabel[n];
        dijLineEdit=new QLineEdit*;
        dijLineEdit[0]=new QLineEdit[n];
        for(int j=0;j<n;j++)
        {
            dijLabel[0][j].setText("Xo"+QString::number(j));
            downlayout->addWidget(dijLabel[0]+j,0,2*j);
            dijLineEdit[0][j].setText("80,"+QString::number(360*j/n));
            connect(dijLineEdit[0]+j,SIGNAL(editingFinished()),this,SLOT(slotvir2c()));
            downlayout->addWidget(dijLineEdit[0]+j,0,2*j+1);
        }
    }
    select=choice;
}

//停止仿真
void Dock::stoprun()
{
    startorstopPushButton->setText("开始仿真");
    timer->stop();
    systemComboBox->setEnabled(true);
    recoverPushButton->setEnabled(true);
}

//按下了开始仿真按钮
void Dock::slotstart()
{
    if(startorstopPushButton->text()==QString("开始仿真"))
    {
        agents.resize(n);
        //获取步长
        step=stepLineEdit->text().toInt();
        QStringList sl;
        //获取初始位置及初始速度
        for(int i=0;i<n;i++)
        {
            sl=posLineEdit[i].text().split(',');
            agents[i].pos.x=sl.at(0).toDouble();
            agents[i].pos.y=sl.at(1).toDouble();
            backup_agents[i].pos=agents[i].pos;
            recoverPushButton->setEnabled(true);
            sl=velLineEdit[i].text().split(',');
            agents[i].velocity.x=sl.at(0).toDouble();
            agents[i].velocity.y=sl.at(1).toDouble();
        }

        //根据不同的系统获取不同的参数
        switch(systemComboBox->currentIndex())
        {
        //基于人工势场法
        case 0:
            //获取邻接矩阵
            dij1.resize(n);
            for(int i=0;i<n;i++)
            {
                dij1[i].resize(n);
                for(int j=0;j<n;j++)
                {
                    if(i<j)
                        dij1[i][j]=dijLineEdit[i][j].text().toInt();
                    else if(i==j)
                        dij1[i][j]=0;
                    else
                        dij1[i][j]=dij1[j][i];
                }
            }
            //获取常系数参数kv,kp
            kv=K1LineEdit->text().toDouble();
            kp=K2LineEdit->text().toDouble();
            //获取目标速度
            sl=VoLineEdit->text().split(',');
            Vo.x=sl.at(0).toDouble();
            Vo.y=sl.at(1).toDouble();
            break;

        //多虚拟领导者法
        case 1:
        case 3:
            //获取虚拟领导者相对于虚拟中心的偏移
            vir2c.resize(n);
            for(int i=0;i<n;i++)
            {
                sl=dijLineEdit[0][i].text().split(',');
                vir2c[i].x=sl.at(0).toDouble()*cos(sl.at(1).toDouble()*PI/180.0);
                vir2c[i].y=sl.at(0).toDouble()*sin(sl.at(1).toDouble()*PI/180.0);

            }
            //李雅普诺夫法
            if(systemComboBox->currentIndex()==1)
            {    
                dij1.resize(n);
                for(int i=0;i<n;i++)
                {
                    dij1[i].resize(n);
                    for(int j=0;j<n;j++)     
                        dij1[i][j]=sqrt(pow(vir2c[i].x-vir2c[j].x,2)+pow(vir2c[i].y-vir2c[j].y,2));
                }
            }
            //有限时间法
            else
            {
                dij2.resize(n);
                //获取各智能体间的理想位移
                for(int i=0;i<n;i++)
                {
                    dij2[i].resize(n);
                    for(int j=0;j<n;j++)
                    {
                        if(i<j)
                        {
                            dij2[i][j]=DPoint(vir2c[j].x-vir2c[i].x,vir2c[j].y-vir2c[i].y);
                        }
                        else if(i==j)
                            dij2[i][j]=DPoint(0,0);
                        else
                            dij2[i][j]=-dij2[j][i];
                    }
                }
            }
            //获取常系数参数k1,k2
            k1=K1LineEdit->text().toDouble();
            k2=K2LineEdit->text().toDouble();
            R=RLineEdit->text().toInt();
            //获取并解析虚拟中心运动轨迹方程
            sl=VoLineEdit->text().split(',');
            analysis_motion(sl.at(0),Vmotionx);
            analysis_motion(sl.at(1),Vmotiony);
            break;

        //盲区法
        case 2:
            //获取各智能体间的理想位移
            dij2.resize(n);
            for(int i=0;i<n;i++)
            {
                dij2[i].resize(n);
                for(int j=0;j<n;j++)
                {
                    if(i<j)
                    {
                        sl=dijLineEdit[i][j].text().split(',');
                        dij2[i][j]=DPoint(sl.at(0).toDouble(),sl.at(1).toDouble());
                    }
                    else if(i==j)
                        dij2[i][j]=DPoint(0,0);
                    else
                        dij2[i][j]=-dij2[j][i];
                }
            }
            //获取常数a1
            a1=K1LineEdit->text().toDouble();
            //通信次数清零
            communication_count=0;
            //获取通信半径表达
            analysis_motion(blind_area_LineEdit->text(),blind_area);
            //计算稳定时智能体的位置
            DPoint avp,avpd;
            QVector<DPoint> test;
            for(int i=0;i<n;i++)
            {
                avp=avp+agents[i].pos;
                avpd=avpd+dij2[0][i];
            }
            avp=avp/n;
            avpd=avpd/n;
            for(int i=0;i<n;i++)
            {
                DPoint p=avp+dij2[0][i]-avpd;
                test.push_back(p);
            }
            break;
        }
        count=0;
        timer->start(step);
        startorstopPushButton->setText("停止仿真");
        systemComboBox->setEnabled(false);
    }
    else
        stoprun();//停止仿真，善后处理
}

//超时处理函数
void Dock::slotttimeout()
{
    //显示运行时间
    runtimeLineEdit->setText(QString::number(step*(++count)/1000.0));
    //根据仿真系统不同，选择相应的控制算法
    switch(systemComboBox->currentIndex())
    {
    //基于人工势场法
    case 0:
        system0_timeout();
        break;
    //基于多虚拟领导者法
    case 1:
        system1_timeout();
        break;
    //基于盲区法
    case 2:
        system2_timeout();
        break;
    //有限时间编队
    case 3:
        system3_timeout();
        break;
    }
    paintarea->update();
}

//智能体j给智能体i产生的势场力
DPoint Dock::potential_power(int i,int j)
{
    double xij2=pow(agents[j].pos.x-agents[i].pos.x,2)+pow(agents[j].pos.y-agents[i].pos.y,2);
    double xij=sqrt(xij2);
    double power=(xij2-pow(dij1[i][j],2))/xij;
    double power_x=power*(agents[j].pos.x-agents[i].pos.x)/xij;
    double power_y=power*(agents[j].pos.y-agents[i].pos.y)/xij;
    return DPoint(power_x,power_y);
}
//基于人工势场法系统的超时处理
void Dock::system0_timeout()
{
    timer->stop();
    //计算控制输入
    for(int i=0;i<n;i++)
    {
        agents[i].u=Vo-agents[i].velocity;
        for(int j=0;j<n;j++)
            if(i!=j&&dij1[i][j])
                agents[i].u=agents[i].u+(agents[j].velocity-agents[i].velocity)*kv+potential_power(i,j)*kp;
    }
    //计算当前位置和速度
    for(int i=0;i<n;i++)
    {
        agents[i].pos=agents[i].pos+agents[i].velocity*step/1000.0;
        agents[i].velocity=agents[i].velocity+agents[i].u*step/1000.0;
        posLineEdit[i].setText(QString::number(agents[i].pos.x)+","+QString::number(agents[i].pos.y));
        velLineEdit[i].setText(QString::number(agents[i].velocity.x)+","+QString::number(agents[i].velocity.y));
    }
    timer->start(step);
}

//基于多虚拟领导者系统的超时处理
void Dock::system1_timeout()
{
    timer->stop();
    //计算虚拟中心的位置和速度信息
    posC_now=DPoint(value_motion(count,Vmotionx),value_motion(count,Vmotiony));
    DPoint posC_next(value_motion(count+1,Vmotionx),value_motion(count+1,Vmotiony));
    DPoint posC_next_next(value_motion(count+2,Vmotionx),value_motion(count+2,Vmotiony));
    DPoint velC_now=(posC_next-posC_now)*1000.0/(double)step;
    DPoint velC_next=(posC_next_next-posC_next)*1000.0/(double)step;
    //计算控制输入
    for(int i=0;i<n;i++)
    {
        agents[i].u=(posC_now+vir2c[i]-agents[i].pos)*k1+(velC_now-agents[i].velocity)*k2+(velC_next-velC_now)*1000.0/(double)step;
        for(int j=0;j<n;j++)
            if(i!=j&&dij1[i][j])
                agents[i].u=agents[i].u+potential_power(i,j);
    }
    //计算位置和速度
    for(int i=0;i<n;i++)
    {
        agents[i].pos=agents[i].pos+agents[i].velocity*step/1000.0;
        agents[i].velocity=agents[i].velocity+agents[i].u*step/1000.0;
        posLineEdit[i].setText(QString::number(agents[i].pos.x)+","+QString::number(agents[i].pos.y));
        velLineEdit[i].setText(QString::number(agents[i].velocity.x)+","+QString::number(agents[i].velocity.y));
    }
    timer->start(step);
}

//基于盲区系统的 超时处理
void Dock::system2_timeout()
{
    DPoint u;
    timer->stop();
    //盲区半径
    double r=value_motion(count,blind_area);
    r=r>0?r:0;
    //计算控制输入
    for(int i=0;i<n;i++)
    {
        agents[i].u=DPoint(0,0);
        for(int j=0;j<n;j++)
        {
            //智能体j和盲区中心的偏移
            DPoint p=agents[j].pos-agents[i].pos-dij2[i][j];
            //如果两智能体的距离大于盲区，则进行通信
            if(dij2[i][j]!=DPoint(0,0)&&sqrt(pow(p.x,2)+pow(p.y,2))>r)
            {      
                agents[i].u=agents[i].u+sig(p,a1);
                communication_count++;
            }
        }
        //统计系统总的控制输入
        u.x+=abs(agents[i].u.x);
        u.y+=abs(agents[i].u.y);
    }
    //计算智能体位置与速度
    for(int i=0;i<n;i++)
    {
        agents[i].pos=agents[i].pos+agents[i].u*step/1000.0;
        agents[i].velocity=agents[i].u;
        posLineEdit[i].setText(QString::number(agents[i].pos.x)+","+QString::number(agents[i].pos.y));
        velLineEdit[i].setText(QString::number(agents[i].velocity.x)+","+QString::number(agents[i].velocity.y));
    }
    //计算通信次数
    K2LineEdit->setText(QString::number(communication_count));
    //计算单位时间通信量
    communication_per_LineEdit->setText(QString::number(communication_count*1000.0/(double)(count*step)));
    //如果系统控制输入为0则停止仿真
    if(abs(u.x)==0&&abs(u.y)==0&&r==0)
    {
        double a=(a1+1)/2.0;
        DPoint time_res;
        double k=pow(n,a);
        for(int i=0;i<n;i++)
        {
            time_res=time_res+DPoint(pow(backup_agents[i].pos.x-agents[i].pos.x,2),pow(backup_agents[i].pos.y-agents[i].pos.y,2));
        }
        time_res.x=pow(time_res.x,1-a)/(k*(1-a));
        time_res.y=pow(time_res.y,1-a)/(k*(1-a));
        stoprun();
    }
    else
        timer->start(step);
}

//基于多虚拟领导者的多智能体有限时间编队
void Dock::system3_timeout()
{
    DPoint u;
    timer->stop();
    //计算虚拟中心的位置和速度信息
    posC_now=DPoint(value_motion(count,Vmotionx),value_motion(count,Vmotiony));
    DPoint posC_next(value_motion(count+1,Vmotionx),value_motion(count+1,Vmotiony));
    DPoint posC_next_next(value_motion(count+2,Vmotionx),value_motion(count+2,Vmotiony));
    DPoint velC_now=(posC_next-posC_now)*1000.0/(double)step;
    DPoint velC_next=(posC_next_next-posC_next)*1000.0/(double)step;
    DPoint aC_now=(velC_next-velC_now)*1000.0/(double)step;
    //计算控制输入
    for(int i=0;i<n;i++)
    {
        agents[i].u=sig(posC_now+vir2c[i]-agents[i].pos,k1)+sig(velC_now-agents[i].velocity,k2)+aC_now;
        for(int j=0;j<n;j++)
            if(i!=j&&dij2[i][j]!=DPoint(0,0))
                agents[i].u=agents[i].u+sig(agents[j].pos-agents[i].pos-dij2[i][j],k1)+sig(agents[j].velocity-agents[i].velocity,k2);
        u=u+agents[i].u;
    }
    //计算位置和速度
    for(int i=0;i<n;i++)
    {
        agents[i].pos=agents[i].pos+agents[i].velocity*step/1000.0;
        agents[i].velocity=agents[i].velocity+agents[i].u*step/1000.0;
        posLineEdit[i].setText(QString::number(agents[i].pos.x)+","+QString::number(agents[i].pos.y));
        velLineEdit[i].setText(QString::number(agents[i].velocity.x)+","+QString::number(agents[i].velocity.y));
    }
    aC_now=aC_now*n;
    if(u.x==aC_now.x&&u.y==aC_now.y)
        stoprun();
    else
        timer->start(step);
}

//sig函数
DPoint Dock::sig(DPoint p,double a)
{
    DPoint res;
    res.x=pow(abs(p.x),a)*sign(p.x);
    res.y=pow(abs(p.y),a)*sign(p.y);
    return res;
}
//对虚拟中心的运动轨迹方程进行解析，中缀表达式转后缀表达式
void Dock::analysis_motion(const QString &Smotion,QVector<QString> &Vmotion)
{
    QVector<QChar> s;
    Vmotion.clear();
    for(int i=0;i<Smotion.size();i++)
    {
        if(Smotion[i]=='+'||Smotion[i]=='-'||Smotion[i]=='*'||Smotion[i]=='/'||Smotion[i]=='s'||Smotion[i]=='c'||Smotion[i]=='^')
        {
            while(!s.empty()&&priority(Smotion[i])<=priority(s.front()))
            {
                Vmotion.push_back(QString(s.front()));
                s.pop_front();
            }
            s.push_front(Smotion[i]);
        }
        else if(Smotion[i]=='(')
            s.push_front('(');
        else if(Smotion[i]==')')
        {
            QChar c;
            while((c=s.front())!='(')
            {
                Vmotion.push_back(QString(c));
                s.pop_front();
            }
            s.pop_front();
        }
        else if(Smotion[i]>='0'&&Smotion[i]<='9'||Smotion[i]=='-')
        {
            int j=i;
            while(++i<Smotion.size()&&Smotion[i]>='0'&&Smotion[i]<='9');
            Vmotion.push_back(Smotion.mid(j,(i--)-j));
        }
        else if(Smotion[i]=='t')
            Vmotion.push_back(QString("t"));
    }
    while(!s.empty())
    {
        Vmotion.push_back(QString(s.front()));
        s.pop_front();
    }
}
//返回运算符的优先级
int Dock::priority(const QChar c)
{
    if(c=='(')
        return 0;
    else if(c=='+'||c=='-')
        return 1;
    else if(c=='*'||c=='/')
        return 2;
    else if(c=='s'||c=='c')
        return 3;
    else if(c=='^')
        return 4;
}
//返回虚拟中心的位置
double Dock::value_motion(int count,QVector<QString> &Vmotion)
{
    double first,second;
    QVector<double> s;
    for(int i=0;i<Vmotion.size();i++)
    {
        if(Vmotion[i]==QString("+"))
        {
            second=s.front();
            s.pop_front();
            first=s.front();
            s.pop_front();
            s.push_front(first+second);
        }
        else if(Vmotion[i]==QString("-"))
        {
            second=s.front();
            s.pop_front();
            first=s.front();
            s.pop_front();
            s.push_front(first-second);
        }
        else if(Vmotion[i]==QString("*"))
        {
            second=s.front();
            s.pop_front();
            first=s.front();
            s.pop_front();
            s.push_front(first*second);
        }
        else if(Vmotion[i]==QString("/"))
        {
            second=s.front();
            s.pop_front();
            first=s.front();
            s.pop_front();
            s.push_front(first/second);
        }
        //sfirstn运算
        else if(Vmotion[i]==QString("s"))
        {
            first=s.front();
            s.pop_front();
            s.push_front(sin(first));
        }
        //cos运算
        else if(Vmotion[i]==QString("c"))
        {
            first=s.front();
            s.pop_front();
            s.push_front(cos(first));
        }
        //幂运算
        else if(Vmotion[i]==QString("^"))
        {
            second=s.front();
            s.pop_front();
            first=s.front();
            s.pop_front();
            s.push_front(pow(first,second));
        }
        //当前时间入栈
        else if(Vmotion[i]==QString("t"))
            s.push_front(double(count*step)/1000.0);
        else
            s.push_front(Vmotion[i].toDouble());
    }
    return s.front();
}
void Dock::slotVmotion()
{
    if(VoLabel->text()==QString("虚拟中心轨迹"))
    {
        QStringList sl=VoLineEdit->text().split(',');
        analysis_motion(sl.at(0),Vmotionx);
        analysis_motion(sl.at(1),Vmotiony);

        /*
        bool started=timer->isActive();
        DPoint posv;
        if(started)
        {
            posv.x=value_motion(count,Vmotionx);
            posv.y=value_motion(count,Vmotiony);
            timer->stop();
        }
        QStringList sl=VoLineEdit->text().split(',');
        analysis_motion(sl.at(0),Vmotionx);
        analysis_motion(sl.at(1),Vmotiony);
        if(started)
        {
            for(int i=0;i<n;i++)
            {
                vir2c[i].x+=posv.x;
                vir2c[i].y+=posv.y;
            }
            timer->start(step);
        }
        */

    }
}
//修改了智能体相对于虚拟中心位置的处理函数
void Dock::slotvir2c()
{
    for(int i=0;i<n;i++)
    {
        if(!dijLineEdit[0][i].text().isEmpty())
        {
            QStringList sl=dijLineEdit[0][i].text().split(',');
            vir2c[i].x=sl.at(0).toInt()*cos(sl.at(1).toDouble()*PI/180.0);
            vir2c[i].y=sl.at(0).toInt()*sin(sl.at(1).toDouble()*PI/180.0);
        }
    }
}
//恢复上一次设置的初始状态
void Dock::slotrecover()
{
    for(int i=0;i<n;i++)
    {
        posLineEdit[i].setText(QString::number(backup_agents[i].pos.x)+","+QString::number(backup_agents[i].pos.y));
        velLineEdit[i].setText(QString::number(backup_agents[i].velocity.x)+","+QString::number(backup_agents[i].velocity.y));
        agents[i].pos=backup_agents[i].pos;
    }
    recoverPushButton->setEnabled(false);
    paintarea->update();
}
//符号函数
int Dock::sign(double x)
{
    if(x>0)
        return 1;
    else if(x==0)
        return 0;
    else
        return -1;
}
bool Dock::eventFilter(QObject *object, QEvent *event)
{
    for(int i=0;i<n;i++)
    {
        if(posLabel+i==object&&event->type()==QEvent::MouseButtonDblClick)
        {
            QColor color=QColorDialog::getColor();
            colors[i]=color;
            posLabel[i].setPalette(QPalette(color));
            velLabel[i].setPalette(QPalette(color));
            paintarea->update();
            return true;
        }
    }
    return true;
}
void Dock::slota1()
{
    if(systemComboBox->currentIndex()==3)
    {
        double a1=K1LineEdit->text().toDouble();
        double a2=2*a1/(a1+1);
        K2LineEdit->setText(QString::number(a2));
    }
}
