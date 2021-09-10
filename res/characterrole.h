#ifndef CHARACTERROLE_H
#define CHARACTERROLE_H

#include "character.h"

// 勇士
class Warrior: public Character
{
public:
    Warrior(const int t_cell_x, const int t_cell_y, const int LocalScreenx, const int LocalScreeny, const bool belong, QWidget* parent = nullptr);
};
// 忍者
class Ninja: public Character
{
public:
    Ninja(const int t_cell_x, const int t_cell_y, const int LocalScreenx, const int LocalScreeny, const bool belong, QWidget* parent = nullptr);
};
// 剑客
class Swordsman: public Character
{
public:
    Swordsman(const int t_cell_x, const int t_cell_y, const int LocalScreenx, const int LocalScreeny, const bool belong, QWidget* parent = nullptr);
};
#endif // CHARACTERROLE_H
