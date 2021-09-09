#include "characterlabel.h"

// 人物属性栏：依附于主窗体
CharacterProperty::CharacterProperty(QString name, int fullhp, int fullmove, int attrack, int attrackable, QWidget *parent):
    QLabel(parent)
{
    // 初始设定
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setMouseTracking(true);
    setFixedSize(165, 150);
    setPixmap(QPixmap(":/pic/dlg.png"));

    // 属性栏各种属性
    nameLabel.setText(name);
    hpLabel1.setText("血量");
    hpLabel2.setText(QString::number(fullhp)+"/"+QString::number(fullhp));
    moveLabel1.setText("行动力");
    moveLabel2.setText(QString::number(fullmove)+"/"+QString::number(fullmove));
    attrackLabel1.setText("攻击力");
    attrackLabel2.setText(QString::number(attrack));
    attrackableLabel1.setText("攻击范围");
    attrackableLabel2.setText(QString::number(attrackable));

    // 将属性加入layout
    layout.addWidget(&nameLabel,0,0);
    layout.addWidget(&hpLabel1,1,0);
    layout.addWidget(&hpLabel2,1,1);
    layout.addWidget(&moveLabel1,2,0);
    layout.addWidget(&moveLabel2,2,1);
    layout.addWidget(&attrackLabel1,3,0);
    layout.addWidget(&attrackLabel2,3,1);
    layout.addWidget(&attrackableLabel1,4,0);
    layout.addWidget(&attrackableLabel2,4,1);
    layout.setVerticalSpacing(0);
    setLayout(&layout);
}
// 更新属性栏数据
void CharacterProperty::updateData(int hp, int fullhp, int move1, int fullmove, int localCellx, int localCelly)
{
    bool ishidden = isHidden();
    // 寻找属性栏显示位置
    int x = 0;
    if((localCellx-1)*CELL_SIZE-165 >= 0)
        x=(localCellx-1)*CELL_SIZE-165;
    else
        x=(localCellx-1)*CELL_SIZE+32;
    move(x,(localCelly-1)*CELL_SIZE);
    // 更新数据
    hpLabel2.setText(QString::number(hp)+"/"+QString::number(fullhp));
    moveLabel2.setText(QString::number(move1)+"/"+QString::number(fullmove));
    if(ishidden)hide();
    else show();
}


// 人物操作选择栏：依附于主窗体
CharacterSelection::CharacterSelection(QWidget* parent):
    QLabel(parent)
{
    // 为三个操作按键设置layout
    layout = new QVBoxLayout;
    moveButton = new QPushButton;
    attrackButton = new QPushButton;
    skipButton = new QPushButton;
    setFixedSize(90,90);
    // 设置文字
    moveButton->setText("移动");
    attrackButton->setText("攻击");
    skipButton->setText("跳过");
    layout->addWidget(moveButton);
    layout->addWidget(attrackButton);
    layout->addWidget(skipButton);
    layout->setSpacing(0);
    setLayout(layout);
}
// 更新人物操作栏信息，也即更新显示位置
void CharacterSelection::updateData(int localCellx, int localCelly)
{
    bool ishidden = isHidden();
    move((localCellx-1)*CELL_SIZE + 64,(localCelly-1)*CELL_SIZE-10);
    if(ishidden)hide();
    else
    {
        show();
        raise();
    }
}

// 人物血条：依附于人物身上
HPLabel::HPLabel(int hp, int totalhp, bool belong,QWidget *parent):
    QLabel(parent), m_hp(hp), m_totalhp(totalhp), m_belong(belong)
{

}
// 更新血量
void HPLabel::reset(int t_hp, int t_totalhp, bool t_belong)
{
    m_hp = t_hp;
    if(m_hp < 0) m_hp = 0;
    m_totalhp = t_totalhp;
    m_belong = t_belong;
}
// 绘制血量
void HPLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(m_belong==YOURS)// 蓝方蓝血
        painter.setBrush(Qt::red);
    else // 红方红血
        painter.setBrush(Qt::blue);
    painter.drawRect(2,0, (1.0 * m_hp / m_totalhp)*CELL_SIZE-4, HP_HEIGHT); // 血槽内容
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    painter.drawRect(2,0, CELL_SIZE-4, HP_HEIGHT); // 血槽边框
    painter.setPen(Qt::NoPen);
}
