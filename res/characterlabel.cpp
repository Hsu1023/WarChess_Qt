#include "characterlabel.h"

/* 此文件包含与人物相关的几种widgets
 * 分别包含人物属性栏CharacterProperty、人物操作选择栏CharacterSelection和血量条HPLabel
 * 以下是他们的初始化和相关方法的内容
 */

// 人物属性栏：依附于主窗体
CharacterProperty::CharacterProperty(QString name, int fullhp, int fullmove, int attrack, int attrackable, QWidget* parent) :
    QLabel(parent)
{
    // 初始设定
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setMouseTracking(true);
    setFixedSize(165, 150);
    setPixmap(QPixmap(":/pic/dlg.png"));

    // 属性栏各种属性
    m_nameLabel.setText(name);
    m_hpLabel1.setText("血量");
    m_hpLabel2.setText(QString::number(fullhp) + "/" + QString::number(fullhp));
    m_moveLabel1.setText("行动力");
    m_moveLabel2.setText(QString::number(fullmove) + "/" + QString::number(fullmove));
    m_attrackLabel1.setText("攻击力");
    m_attrackLabel2.setText(QString::number(attrack));
    m_attrackableLabel1.setText("攻击范围");
    m_attrackableLabel2.setText(QString::number(attrackable));

    // 将属性加入layout
    m_layout.addWidget(&m_nameLabel, 0, 0);
    m_layout.addWidget(&m_hpLabel1, 1, 0);
    m_layout.addWidget(&m_hpLabel2, 1, 1);
    m_layout.addWidget(&m_moveLabel1, 2, 0);
    m_layout.addWidget(&m_moveLabel2, 2, 1);
    m_layout.addWidget(&m_attrackLabel1, 3, 0);
    m_layout.addWidget(&m_attrackLabel2, 3, 1);
    m_layout.addWidget(&m_attrackableLabel1, 4, 0);
    m_layout.addWidget(&m_attrackableLabel2, 4, 1);
    m_layout.setVerticalSpacing(0);
    setLayout(&m_layout);
}
// 更新属性栏数据
void CharacterProperty::updateData(const int hp, const int fullhp, const int move1, const int fullmove, const int localCellx, const int localCelly)
{
    bool ishidden = isHidden();
    // 寻找属性栏显示位置
    int x = 0;
    if ((localCellx - 1) * CELL_SIZE - 165 >= 0)
        x = (localCellx - 1) * CELL_SIZE - 165;
    else
        x = (localCellx - 1) * CELL_SIZE + 32;
    move(x, (localCelly - 1) * CELL_SIZE);
    // 更新数据
    m_hpLabel2.setText(QString::number(hp) + "/" + QString::number(fullhp));
    m_moveLabel2.setText(QString::number(move1) + "/" + QString::number(fullmove));
    if (ishidden)hide();
    else show();
}


// 人物操作选择栏：依附于主窗体
CharacterSelection::CharacterSelection(QWidget* parent) :
    QLabel(parent)
{
    // 为三个操作按键设置layout
    m_layout = new QVBoxLayout;
    m_moveButton = new QPushButton;
    m_attrackButton = new QPushButton;
    m_skipButton = new QPushButton;
    setFixedSize(90, 90);
    // 设置文字
    m_moveButton->setText("移动");
    m_attrackButton->setText("攻击");
    m_skipButton->setText("跳过");
    m_layout->addWidget(m_moveButton);
    m_layout->addWidget(m_attrackButton);
    m_layout->addWidget(m_skipButton);
    m_layout->setSpacing(0);
    setLayout(m_layout);
}
// 更新人物操作栏信息，也即更新显示位置
void CharacterSelection::updateData(const int localCellx, const int localCelly)
{
    bool ishidden = isHidden();
    move((localCellx - 1) * CELL_SIZE + 64, (localCelly - 1) * CELL_SIZE - 10);
    if (ishidden)hide();
    else
    {
        show();
        raise();
    }
}

// 人物血条：依附于人物身上
HPLabel::HPLabel(const int hp, const int totalhp, const bool belong, QWidget* parent) :
    QLabel(parent), m_hp(hp), m_totalhp(totalhp), m_belong(belong)
{

}
// 更新血量
void HPLabel::reset(const int t_hp, const int t_totalhp, const bool t_belong)
{
    m_hp = t_hp;
    if (m_hp < 0) m_hp = 0;
    m_totalhp = t_totalhp;
    m_belong = t_belong;
}
// 绘制血量
void HPLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    if (m_belong == YOURS)// 蓝方蓝血
        painter.setBrush(Qt::red);
    else // 红方红血
        painter.setBrush(Qt::blue);
    painter.drawRect(2, 0, (1.0 * m_hp / m_totalhp) * CELL_SIZE - 4, HP_HEIGHT); // 血槽内容
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    painter.drawRect(2, 0, CELL_SIZE - 4, HP_HEIGHT); // 血槽边框
    painter.setPen(Qt::NoPen);
}
