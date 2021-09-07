#include "character.h"
#include "algorithm.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny,bool belong, QWidget* parent):
    QLabel(parent),
    m_cellx(t_cellx), m_celly(t_celly),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE),
    m_belong(belong),characterState(BEGIN), attrackedOrNot(false)
{
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
    });

    connect(selectionDlg->moveButton, &QPushButton::clicked,
            this, &Character::moveAction);
    connect(selectionDlg->attrackButton, &QPushButton::clicked,
            this, &Character::attrackAction);
    connect(selectionDlg->skipButton, &QPushButton::clicked,
            this, &Character::skipAction);
    connect(this, &Character::beAttracked, this, &Character::attrackedEvent);

    setFixedSize(64,64);

    //setMouseTracking(true);

    //repaint();

}
void Character::setLabel()
{
}
Warrior::Warrior( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, bool belong, QWidget* parent):
    Character( t_cellx, t_celly, LocalScreenx, LocalScreeny,belong, parent)
{
    m_hp =50; m_fullhp = 100;
    m_move = m_fullmove = 4;
    m_attrack = 40;
    m_attrackable=1;
    name = "勇士";
    icon.load(WARRIOR_PATH);
    setPixmap(icon);
//begintag;
    hpLabel = new HPLabel(m_hp, m_fullhp, m_belong, this);
//endtag;
    propertyDlg = new CharacterProperty(name, m_fullhp, m_fullmove, m_attrack,m_attrackable, parent);
    propertyDlg->hide();
}

void Character::enterEvent(QEvent *)
{
    //emit repaintScreen();
    if(propertyDlg->isHidden()==true)
    {
        propertyDlg->show();
        propertyDlg->updateData(m_hp, m_fullhp, m_move, m_fullmove, m_localCellx, m_localCelly);
        propertyDlg->raise();
        QTimer::singleShot(1000,[=](){
              propertyDlg->hide();
        });
    }
}
void Character::leaveEvent(QEvent *)
{
    //emit repaintScreen();
    if(propertyDlg->isHidden()==false)
        propertyDlg->hide();
}

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
    double d = (rand()%20-40)/100 + 1;
    attrack = int(1.0 * d *attrack);

    m_hp -= attrack;
    //扣血
    QLabel *tempLabel= new QLabel(parentWidget());
    tempLabel->setAttribute(Qt::WA_DeleteOnClose);
    tempLabel->setStyleSheet("color:rgb(255,40,0,0.8); font:bold; font-size:20px;");
    tempLabel->setText(QString("-%1").arg(attrack));
    tempLabel->show();
    tempLabel->raise();
    QPropertyAnimation* animation = new QPropertyAnimation(tempLabel,"geometry");
    animation->setDuration(500);
    animation->setStartValue(QRect((m_localCellx-1)*CELL_SIZE+CELL_SIZE/4, (m_localCelly-1)*CELL_SIZE-30, tempLabel->width(),tempLabel->height()));
    animation->setEndValue(QRect((m_localCellx-1)*CELL_SIZE+CELL_SIZE/4, (m_localCelly-1)*CELL_SIZE-30 -30,tempLabel->width(),tempLabel->height()));
    animation->start();
    if(m_hp<=0)
    {
       characterState=DEAD;
       hide();
       emit dieOneCharacter(this);//TODO:slot
    }
    QTimer::singleShot(500,[=](){tempLabel->close();});

    emit infoChanged();
}
void Character::skipAction()
{
    characterState = END;
    selectionDlg->hide();
    emit endOneCharacter(this);//TODO:slot
}
void Character::updateInfo()
{
    emit infoChanged();
}
void Character::movePos(int steps, std::vector<int>path)
{
    selectionDlg->hide();
    m_move -= steps;
    //m_cellx = mouseCellx;
    //m_celly = mouseCelly;
    //m_localCellx = mouseLocalCellx;
    //m_localCelly = mouseLocalCelly;
    //move((mouseLocalCellx-1)*CELL_SIZE, (mouseLocalCelly-1)*CELL_SIZE);
    mover->moveAlongPath(this, path);
    updateInfo();
}
