#ifndef CHARACTERLABEL_H
#define CHARACTERLABEL_H

#include <QLabel>
#include "config.h"

class CharacterLabel: public QLabel
{
    Q_OBJECT
public:
    CharacterLabel(QString, int, int, int, QWidget* parent = nullptr);
public:
    QGridLayout layout;
    QLabel nameLabel, hpLabel1, hpLabel2;
    int x, y;
    void updateData(int hp, int fullhp, int move, int fullmove, int localx, int localy);
};

#endif // CHARACTERLABEL_H
