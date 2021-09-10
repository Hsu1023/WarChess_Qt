#include "characterrole.h"

/* 此文件包含了Character所有的派生类，囊括设计的几种人物
 * 共有勇士、忍者、剑客三种
 * 以下是他们属性值的初始化
*/


// 勇士，血量丰厚
Warrior::Warrior(const int t_cellx, const int t_celly, const int LocalScreenx, const int LocalScreeny, const bool belong, QWidget* parent) :
    Character(t_cellx, t_celly, LocalScreenx, LocalScreeny, belong, parent)
{
    m_hp = 120; m_fullhp = 120;
    m_move = m_fullmove = 5;
    m_attrack = 45;
    m_attrackable = 1;
    m_name = "勇士";
    m_icon.load(WARRIOR_PATH);
    setPixmap(m_icon);
    m_hpLabel = new HPLabel(m_hp, m_fullhp, m_belong, this);
    m_propertyDlg = new CharacterProperty(m_name, m_fullhp, m_fullmove, m_attrack, m_attrackable, parent);
    m_propertyDlg->hide();
}
// 忍者，行动力强
Ninja::Ninja(const int t_cellx, const int t_celly, const int LocalScreenx, const int LocalScreeny, const bool belong, QWidget* parent) :
    Character(t_cellx, t_celly, LocalScreenx, LocalScreeny, belong, parent)
{
    m_hp = 80; m_fullhp = 80;
    m_move = m_fullmove = 8;
    m_attrack = 30;
    m_attrackable = 1;
    m_name = "忍者";
    m_icon.load(NINJA_PATH);
    setPixmap(m_icon);
    m_hpLabel = new HPLabel(m_hp, m_fullhp, m_belong, this);
    m_propertyDlg = new CharacterProperty(m_name, m_fullhp, m_fullmove, m_attrack, m_attrackable, parent);
    m_propertyDlg->hide();
}
// 剑客，攻击范围广
Swordsman::Swordsman(const int t_cellx, const int t_celly, const int LocalScreenx, const int LocalScreeny, const bool belong, QWidget* parent) :
    Character(t_cellx, t_celly, LocalScreenx, LocalScreeny, belong, parent)
{
    m_hp = 100; m_fullhp = 100;
    m_move = m_fullmove = 4;
    m_attrack = 50;
    m_attrackable = 2;
    m_name = "剑客";
    m_icon.load(SWORDSMAN_PATH);
    setPixmap(m_icon);
    m_hpLabel = new HPLabel(m_hp, m_fullhp, m_belong, this);
    m_propertyDlg = new CharacterProperty(m_name, m_fullhp, m_fullmove, m_attrack, m_attrackable, parent);
    m_propertyDlg->hide();
}
