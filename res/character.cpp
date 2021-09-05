#include "character.h"
#include "algorithm.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, QWidget* parent):
    QLabel(parent),
    m_cellx(t_cellx), m_celly(t_celly), characterState(BEGIN), attrackedOrNot(false),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE)
{
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
    });

    connect(selectionDlg->moveButton, &QPushButton::clicked,
            this, &Character::moveAction);
    connect(selectionDlg->attrackButton, &QPushButton::clicked,
            this, &Character::attrackAction);
    connect(selectionDlg->skipButton, &QPushButton::clicked,
            this, &Character::skipAction);
    connect(this, &Character::beAttracked, this, &Character::attrackedEvent);

}
void Character::setLabel()
{

}
Warrior::Warrior( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, QWidget* parent):
    Character( t_cellx, t_celly, LocalScreenx, LocalScreeny, parent)
{
    m_hp =50; m_fullhp = 100;
    m_move = m_fullmove = 4;
    m_attrack = 40;
    m_attrackable=1;
    name = "勇士";
    icon.load(WARRIOR_PATH);
    setFixedSize(64,64);
    setPixmap(icon);

    setMouseTracking(true);
    propertyDlg = new CharacterProperty(name, m_fullhp, m_fullmove, m_attrack,m_attrackable, parent);
    propertyDlg->hide();

    emit infoChanged();
}

void Character::enterEvent(QEvent *)
{
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
    m_hp -= attrack;

    //扣血
    QLabel *tempLabel= new QLabel(parentWidget());
    tempLabel->setAttribute(Qt::WA_DeleteOnClose);
    tempLabel->setStyleSheet("color:red; font:bold; font-size:20px;");
    tempLabel->setText(QString("-%1").arg(attrack));
    tempLabel->show();
    tempLabel->raise();
    tempLabel->setGeometry((m_localCellx-1)*CELL_SIZE+CELL_SIZE/4, (m_localCelly-1)*CELL_SIZE-40,CELL_SIZE/2,40);
    QTimer::singleShot(1000,[=](){delete tempLabel;});

    if(m_hp<=0)
    {
        characterState=DEAD;
        emit dieOneCharacter(this);//TODO:slot
    }
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
