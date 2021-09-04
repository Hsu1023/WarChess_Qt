#include "character.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, QWidget* parent):
    QLabel(parent),
    m_cellx(t_cellx), m_celly(t_celly),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE)
{
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
    dlg = new CharacterLabel(name, m_fullhp, m_fullmove, m_attrack, parent);
    dlg->hide();
    //setLabel();
}

void Character::enterEvent(QEvent *)
{
    if(dlg->isHidden()==true)
    {
        int x = 0;
        if((m_localCellx-1)*CELL_SIZE-165 >= 0)
            x=(m_localCellx-1)*CELL_SIZE-165;
        else
            x=(m_localCellx-1)*CELL_SIZE+32;
        //dlg->show();

        dlg->show();
        dlg->updateData(m_hp, m_fullhp, m_move, m_fullmove, x, (m_localCelly-1)*CELL_SIZE);
        dlg->raise();
        qDebug()<<(m_localCelly-1)*CELL_SIZE;
    }
}
void Character::leaveEvent(QEvent *)
{
    if(dlg->isHidden()==false)
        dlg->hide();
}




