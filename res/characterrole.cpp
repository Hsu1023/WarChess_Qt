#include "characterrole.h"

// 勇士，血量丰厚
Warrior::Warrior( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, bool belong, QWidget* parent):
    Character( t_cellx, t_celly, LocalScreenx, LocalScreeny,belong, parent)
{
    m_hp = 120; m_fullhp = 120;
    m_move = m_fullmove = 5;
    m_attrack = 45;
    m_attrackable=1;
    name = "勇士";
    icon.load(WARRIOR_PATH);
    setPixmap(icon);
    hpLabel = new HPLabel(m_hp, m_fullhp, m_belong, this);
    propertyDlg = new CharacterProperty(name, m_fullhp, m_fullmove, m_attrack,m_attrackable, parent);
    propertyDlg->hide();
}
// 忍者，行动力强
Ninja::Ninja( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, bool belong, QWidget* parent):
    Character( t_cellx, t_celly, LocalScreenx, LocalScreeny,belong, parent)
{
    m_hp = 80; m_fullhp = 80;
    m_move = m_fullmove = 8;
    m_attrack = 30;
    m_attrackable = 1;
    name = "忍者";
    icon.load(NINJA_PATH);
    setPixmap(icon);
    hpLabel = new HPLabel(m_hp, m_fullhp, m_belong, this);
    propertyDlg = new CharacterProperty(name, m_fullhp, m_fullmove, m_attrack,m_attrackable, parent);
    propertyDlg->hide();
}
// 剑客，攻击范围广
Swordsman::Swordsman( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, bool belong, QWidget* parent):
    Character( t_cellx, t_celly, LocalScreenx, LocalScreeny,belong, parent)
{
    m_hp = 100; m_fullhp = 100;
    m_move = m_fullmove = 4;
    m_attrack = 50;
    m_attrackable = 2;
    name = "剑客";
    icon.load(SWORDSMAN_PATH);
    setPixmap(icon);
    hpLabel = new HPLabel(m_hp, m_fullhp, m_belong, this);
    propertyDlg = new CharacterProperty(name, m_fullhp, m_fullmove, m_attrack,m_attrackable, parent);
    propertyDlg->hide();
}
