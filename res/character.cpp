#include "character.h"
#include "algorithm.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, QWidget* parent):
    QLabel(parent),
    m_cellx(t_cellx), m_celly(t_celly), characterState(BEGIN),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE)
{
    selectionDlg = new CharacterSelection(parent);
    selectionDlg->hide();
    connect(selectionDlg->moveButton, &QPushButton::clicked,
            this, &Character::moveAction);
    connect(selectionDlg->attrackButton, &QPushButton::clicked,
            this, &Character::attrackAction);
    connect(selectionDlg->skipButton, &QPushButton::clicked,
            this, &Character::skipAction);
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
    name = "勇士";
    icon.load(WARRIOR_PATH);
    setFixedSize(64,64);
    setPixmap(icon);

    setMouseTracking(true);
    propertyDlg = new CharacterProperty(name, m_fullhp, m_fullmove, m_attrack, parent);
    propertyDlg->hide();

}

void Character::enterEvent(QEvent *)
{
    if(propertyDlg->isHidden()==true)
    {
        int x = 0;
        if((m_localCellx-1)*CELL_SIZE-165 >= 0)
            x=(m_localCellx-1)*CELL_SIZE-165;
        else
            x=(m_localCellx-1)*CELL_SIZE+32;
        //dlg->show();

        propertyDlg->show();
        propertyDlg->updateData(m_hp, m_fullhp, m_move, m_fullmove, x, (m_localCelly-1)*CELL_SIZE);
        propertyDlg->raise();

    }
}
void Character::leaveEvent(QEvent *)
{
    if(propertyDlg->isHidden()==false)
        propertyDlg->hide();
}

void Character::moveAction()
{
    //TODO：selectionDlg->hide();
    characterState = FINDPATH;
    emit characterMoveAction(this);
}
void Character::attrackAction()
{

    characterState = FINDATTRACK;
    emit characterAttrackAction(this);
}
void Character::skipAction()
{
    characterState = END;
}
