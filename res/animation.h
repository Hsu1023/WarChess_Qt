#ifndef ANIMATION_H
#define ANIMATION_H

#include "config.h"
#include <QObject>
#include <QWidget>

class MoveAnimation :public QWidget
{
    Q_OBJECT
public:
    MoveAnimation(QLabel *character, QWidget *parent=nullptr);
};

#endif // ANIMATION_H
