#ifndef CHARACTERROLE_H
#define CHARACTERROLE_H

#include "character.h"


class Warrior: public Character
{
public:
    Warrior(int t_cell_x, int t_cell_y, int LocalScreenx, int LocalScreeny,bool belong, QWidget* parent = nullptr);
};
class Ninja: public Character
{
public:
    Ninja(int t_cell_x, int t_cell_y, int LocalScreenx, int LocalScreeny,bool belong, QWidget* parent = nullptr);
};
class Swordsman: public Character
{
public:
    Swordsman(int t_cell_x, int t_cell_y, int LocalScreenx, int LocalScreeny,bool belong, QWidget* parent = nullptr);
};
#endif // CHARACTERROLE_H
