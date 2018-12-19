#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtGui>
#include <cmath>
#include <QTime>
#define PI 3.14159265
class DPoint
{
public:
    double x;
    double y;
    DPoint(double x=0,double y=0);
    DPoint operator +(DPoint p2);
    DPoint operator -(DPoint p2);
    DPoint operator *(double i);
    DPoint operator /(double i);
    DPoint operator -();
    bool operator !=(DPoint p);
};
//智能体类
struct Agent
{
    DPoint pos;//位置
    DPoint velocity; //速度
    DPoint u; //控制输入
};

//画布类
class Paintarea:public QWidget
{
    Q_OBJECT
public:
    Paintarea(QWidget *parent=0);
    QPushButton *button;
    void paintEvent(QPaintEvent *event);
    bool virmode;
};
static DPoint posC_now;//虚拟中心位置
class Dock : public QDialog
{
    Q_OBJECT
public:
    Dock();
    QPushButton *startorstopPushButton;//开始停止按钮
    Paintarea *paintarea;//画布

    QLabel *systemLabel;//仿真系统选择设置
    QComboBox *systemComboBox;

    QLabel *nLabel;//智能体个数
    QLineEdit *nLineEdit;

    QLabel *stepLabel;//步长
    QLineEdit *stepLineEdit;

    QLabel *runtimeLabel;//系统运行时间显示
    QLineEdit *runtimeLineEdit;

    QLabel *VoLabel;//目标速度
    QLineEdit *VoLineEdit;

    QPushButton *posPushButton;
    QLabel *pLabel;
    QLabel *posLabel;//智能体初始位置
    QLineEdit *posLineEdit;

    QPushButton *velPushButton;
    QLabel *vLabel;
    QLabel *velLabel;//智能体初始速度
    QLineEdit *velLineEdit;

    QLabel *edijLabel;
    QLabel **dijLabel;//邻接矩阵
    QLineEdit **dijLineEdit;

    QLabel *K1Label;//常数k1
    QLineEdit *K1LineEdit;

    QLabel *K2Label;//常数k2
    QLineEdit *K2LineEdit;

    QPushButton *recoverPushButton;
    QVector<Agent> backup_agents;
    QHBoxLayout *upperlayout;
    QHBoxLayout *poslayout;
    QHBoxLayout *vellayout;
    QHBoxLayout *lowlayout;
    QGridLayout *downlayout;
    QVBoxLayout *mainlayout;

    //基于人工势场法的相关参数及处理函数
    QVector<QVector<int> > dij1;//人工势场法的邻接矩阵
    double kv,kp;//常系数
    DPoint Vo;//目标速度
    void system0_timeout();//基于人工势场法系统的 超时处理

    //基于多虚拟领导者的相关参数及处理函数
    QVector<DPoint> vir2c;//虚拟领导者相对于虚拟中心的偏移
    QVector<QString> Vmotionx;//保存目标运动方程在x轴的后缀表达式
    QVector<QString> Vmotiony;//保存目标运动方程在y轴的后缀表达式
    int R;//智能体感应半径
    double k1,k2;//常系数
    QLabel *RLabel;
    QLineEdit *RLineEdit;
    void system1_timeout();//基于多虚拟领导者系统的 超时处理
    void system3_timeout();//基于多虚拟领导者系统的有限时间 超时处理

    //基于盲区法的相关参数及处理函数
    QVector<QVector<DPoint> > dij2;//盲区的邻接矩阵
    double a1;//常系数
    long long communication_count;//通信次数
    QVector<QString> blind_area;//盲区半径表达式
    QLabel *blind_area_Label;//盲区半径
    QLineEdit *blind_area_LineEdit;
    QLabel *communication_per_Label;//单位时间通信量
    QLineEdit *communication_per_LineEdit;
    void system2_timeout();//基于盲区系统的 超时处理

    //其他公共参数
    long long int count;//步数统计
    double runtime;//系统运行时间;
    int step;//步长
    QTimer *timer;//定时器
    int select;//记录上次选择的仿真系统编号
    int lastn;//记录上次智能体个数
    void stop();//停止仿真
    void stoprun();//停止仿真
    int priority(const QChar c);//返回运算符的优先级
    double value_motion(int count,QVector<QString> &Vmotion);//返回表达式Vmotion的值
    void analysis_motion(const QString &Smotion,QVector<QString> &Vmotion);//解析关于时间t的表达式，中缀转后缀
    DPoint potential_power(int i,int j);//智能体j相对于智能体i的虚拟力
    int sign(double x);//符号函数
    DPoint sig(DPoint p,double a);//sig函数
public slots:
    void slotposLineEdit();//初始位置设置框内容改变
    void slotdij(int n);//理想距离设置
    void slotstart();//开始仿真前的预处理
    void slotpos();//随机生成智能体初始位置
    void slotvel();//随机生成智能体初始速度
    void slotn();//智能体个数变化
    void slotttimeout();//超时处理函数
    void slotVmotion();//虚拟中心轨迹变化
    void slotvir2c();//修改了虚拟领导者相对于虚拟中心的偏移对应的处理函数
    void slotrecover();//恢复上一次设置的初始状态
    void slota1();//a1修改后，对应的参数a2也要修改
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // WIDGET_H
