#include "character.h"
#include "algorithm.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny,bool belong, QWidget* parent):
    QLabel(parent),
    m_cellx(t_cellx), m_celly(t_celly),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE),
    m_belong(belong),characterState(BEGIN), attrackedOrNot(false)
{
    // 初始设定
    setAttribute(Qt::WA_MouseNoMask, true);

    // 关于移动、攻击动画的信号和槽
    mover = new MoveAnimation;
    attracker = new AttrackAnimation;
    connect(mover, &MoveAnimation::widgetDown, [=](){m_celly++; m_localCelly++;});
    connect(mover, &MoveAnimation::widgetUp, [=](){m_celly--; m_localCelly--;});
    connect(mover, &MoveAnimation::widgetRight, [=](){m_cellx++; m_localCellx++;});
    connect(mover, &MoveAnimation::widgetLeft, [=](){m_cellx--; m_localCellx--;});
    connect(mover, &MoveAnimation::animationFinished, [=](){emit infoChanged();});
    layout = new QVBoxLayout(this);
    // 操作选择框
    selectionDlg = new CharacterSelection(parent);
    selectionDlg->hide();

    // 当人物信息改变时，血条、人物属性和操作选择框的所有信息都要改变
    connect(this, &Character::infoChanged,[=](){
        // 按顺序，raise有次序
        if(characterState==DEAD)
        {
            propertyDlg->close();
            selectionDlg->close();
            hide();
        }
        propertyDlg->updateData(m_hp,m_fullhp,m_move,m_fullmove,m_localCellx,m_localCelly);
        selectionDlg->updateData(m_localCellx,m_localCelly);
        hpLabel->reset(m_hp, m_fullhp, belong);
        hpLabel->repaint();
    });

    // 操作选择框的各个操作按钮
    connect(selectionDlg->moveButton, &QPushButton::clicked,
            this, &Character::moveAction);
    connect(selectionDlg->attrackButton, &QPushButton::clicked,
            this, &Character::attrackAction);
    connect(selectionDlg->skipButton, &QPushButton::clicked,
            this, &Character::skipAction);
    connect(this, &Character::beAttracked, this, &Character::attrackedEvent);


}
// 鼠标进入，弹出人物属性栏
void Character::enterEvent(QEvent *)
{
    if(propertyDlg->isHidden()==true)
    {
        propertyDlg->show();
        propertyDlg->raise();
        // 延时关闭
        QTimer::singleShot(1500,this,[=](){
              propertyDlg->hide();
        });
    }
}
// 鼠标移出，关闭人物属性栏
void Character::leaveEvent(QEvent *)
{
    if(propertyDlg->isHidden()==false)
        propertyDlg->hide();
}
// 人物需要移动，连接到GameScene的寻路显示上
void Character::moveAction()
{
    if(selectionDlg->isHidden()==false)
        selectionDlg->hide();
    characterState = FINDPATH;
    emit characterMoveAction(this);
}
// 人物需要攻击，连接到GameScene的攻击显示上
void Character::attrackAction()
{
    if(attrackedOrNot)
    {
        emit submitHint("本回合已攻击过");
        emit hideCancelButton();
        return;
    }
    if(selectionDlg->isHidden()==false)
        selectionDlg->hide();
    characterState = FINDATTRACK;
    emit characterAttrackAction(this);
}
// 被攻击时，attrack是被攻击的攻击点数
void Character::attrackedEvent(int attrack)
{
    // 扣血
    double d = 1.0*(rand()%40-20)/100 + 1;
    attrack = int(1.0 * d *attrack);
    m_hp -= attrack;

    // 判断是否死亡
    if(m_hp<=0)
    {
       characterState=DEAD;
       hide();
       emit dieOneCharacter(this);
    }

    // 扣血条
    QLabel *tempLabel= new QLabel(parentWidget());
    tempLabel->setAttribute(Qt::WA_DeleteOnClose);
    tempLabel->setStyleSheet("color:rgb(255,40,0); font:bold; font-size:20px;");
    tempLabel->setText(QString("-%1").arg(attrack));
    tempLabel->show();
    tempLabel->raise();
    // 扣血条上浮并停留的动画
    QPropertyAnimation* animation = new QPropertyAnimation(tempLabel,"geometry");
    animation->setDuration(1000);
    animation->setStartValue(QRect((m_localCellx-1)*CELL_SIZE+CELL_SIZE/4, (m_localCelly-1)*CELL_SIZE-30, tempLabel->width(),tempLabel->height()));
    animation->setEndValue(QRect((m_localCellx-1)*CELL_SIZE+CELL_SIZE/4, (m_localCelly-1)*CELL_SIZE-30 -30,tempLabel->width(),tempLabel->height()));
    animation->start();

    emit infoChanged();
    QTimer::singleShot(1500,this,[=](){tempLabel->close();});
}
// 跳过选择，向GameScene说明有人结束
void Character::skipAction()
{
    characterState = END;
    selectionDlg->hide();
    emit endOneCharacter(this);
}
// 需要更新数据时，发射数据更新信号
void Character::updateInfo()
{
    emit infoChanged();
}
// 给予路径，调动MoveAnimation播放移动的动画
void Character::movePos(int steps, std::vector<int>path)
{
    selectionDlg->hide();
    m_move -= steps;
    mover->moveAlongPath(this, path);
    updateInfo();
}
