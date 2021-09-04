#ifndef CHARACTERLABEL_H
#define CHARACTERLABEL_H

#include <QLabel>
#include "config.h"

class CharacterProperty: public QLabel
{
    Q_OBJECT
public:
    CharacterProperty(QString, int, int, int, QWidget* parent = nullptr);
public:
    QGridLayout layout;
    QLabel nameLabel, hpLabel1, hpLabel2;
    void updateData(int hp, int fullhp, int move, int fullmove, int localx, int localy);
};

class CharacterSelection: public QLabel
{
    Q_OBJECT
public:
    CharacterSelection(QWidget* parent = nullptr);
public:
    QVBoxLayout *layout;
    QPushButton *moveButton, *attrackButton, *skipButton;
};

#endif // CHARACTERLABEL_H
