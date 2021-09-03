#include "character.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny):
    m_cellx(t_cellx), m_celly(t_celly),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE)
{
}
Warrior::Warrior( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny):
    Character( t_cellx, t_celly, LocalScreenx, LocalScreeny)
{
    m_blood =50; m_fullblood = 100;
    m_move = 4;
    m_attrack = 40;
    icon.load(WARRIOR_PATH);
    dlg = new CharacterDlg;
}

