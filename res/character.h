#ifndef CHARACTER_H
#define CHARACTER_H

#include "config.h"
#include "characterdlg.h"

class Character
{
public:
    Character(int t_cell_x, int t_cell_y, int LocalScreenx, int LocalScreeny);
public:
    int m_attrack, m_move, m_fullblood, m_blood;
    int m_cellx, m_celly; //  全局位置
    int m_localCellx, m_localCelly; // 桌面位置
    QPixmap icon;
    CharacterDlg *dlg;
public:
    void drawCharacter(QPainter &);
};

class Warrior: public Character
{
public:
    Warrior(int t_cell_x, int t_cell_y, int LocalScreenx, int LocalScreeny);
};

#endif // CHARACTER_H
