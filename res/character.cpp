#include "character.h"
#include "algorithm.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny,bool belong, QWidget* parent):
    QLabel(parent),
    m_cellx(t_cellx), m_celly(t_celly),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE),
    m_belong(belong),characterState(BEGIN), attrackedOrNot(false)
{
    setAttribute(Qt::WA_MouseNoMask, true);
    mover = new MoveAnimation;
    attracker = new AttrackAnimation;
    connect(mover, &MoveAnimation::widgetDown, [=](){m_celly++; m_localCelly++;});
    connect(mover, &MoveAnimation::widgetUp, [=](){m_celly--; m_localCelly--;});
    connect(mover, &MoveAnimation::widgetRight, [=](){m_cellx++; m_localCellx++;});
    connect(mover, &MoveAnimation::widgetLeft, [=](){m_cellx--; m_localCellx--;});
    connect(mover, &MoveAnimation::animationFinished, [=](){emit infoChanged();});

    layout = new QVBoxLayout(this);

    selectionDlg = new CharacterSelection(parent);
    selectionDlg->hide();

    connect(this, &Character::infoChanged,[=](){
        //按顺序，raise有次序
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

    connect(selectionDlg->moveButton, &QPushButton::clicked,
            this, &Character::moveAction);
    connect(selectionDlg->attrackButton, &QPushButton::clicked,
            this, &Character::attrackAction);
    connect(selectionDlg->skipButton, &QPushButton::clicked,
            this, &Character::skipAction);
    connect(this, &Character::beAttracked, this, &Character::attrackedEvent);


    setFixedSize(64,64);

    setMouseTracking(true);

    //repaint();

}
void Character::setLabel()
{
}


void Character::enterEvent(QEvent *)
{
    if(propertyDlg->isHidden()==true)
    {
        propertyDlg->show();
        propertyDlg->raise();
        QTimer::singleShot(1000,this,[=](){
              propertyDlg->hide();
        });
    }
}
void Character::leaveEvent(QEvent *)
{
    if(propertyDlg->isHidden()==false)
        propertyDlg->hide();
}
/*
void Character::mouseMoveEvent(QMouseEvent *ev)
{
    ev->ignore();
}
*/
void Character::moveAction()
{
    if(selectionDlg->isHidden()==false)
        selectionDlg->hide();
    characterState = FINDPATH;
    emit characterMoveAction(this);
}
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
void Character::attrackedEvent(int attrack)
{
    double d = 1.0*(rand()%40-20)/100 + 1;
    attrack = int(1.0 * d *attrack);
    m_hp -= attrack;
    if(m_hp<=0)
    {
       characterState=DEAD;
       hide();
       emit dieOneCharacter(this);
    }
    //扣血
    QLabel *tempLabel= new QLabel(parentWidget());
    tempLabel->setAttribute(Qt::WA_DeleteOnClose);
    tempLabel->setStyleSheet("color:rgb(255,40,0); font:bold; font-size:20px;");
    tempLabel->setText(QString("-%1").arg(attrack));
    tempLabel->show();
    tempLabel->raise();
    QPropertyAnimation* animation = new QPropertyAnimation(tempLabel,"geometry");
    animation->setDuration(1000);
    animation->setStartValue(QRect((m_localCellx-1)*CELL_SIZE+CELL_SIZE/4, (m_localCelly-1)*CELL_SIZE-30, tempLabel->width(),tempLabel->height()));
    animation->setEndValue(QRect((m_localCellx-1)*CELL_SIZE+CELL_SIZE/4, (m_localCelly-1)*CELL_SIZE-30 -30,tempLabel->width(),tempLabel->height()));
    animation->start();

    emit infoChanged();
    QTimer::singleShot(1500,this,[=](){tempLabel->close();});
}
void Character::skipAction()
{
    characterState = END;
    selectionDlg->hide();
    emit endOneCharacter(this);
}
void Character::updateInfo()
{
    emit infoChanged();
}
void Character::movePos(int steps, std::vector<int>path)
{
    selectionDlg->hide();
    m_move -= steps;
    mover->moveAlongPath(this, path);
    updateInfo();
}
