#ifndef ANIMATION_H
#define ANIMATION_H

#include "config.h"
#include <QObject>
#include <QWidget>

class MoveAnimation :public QWidget
{
    Q_OBJECT

public:
    MoveAnimation(QWidget *parent=nullptr);
public:
    void moveOneCell(QWidget* object, int duration, int direction);
    void moveAlongPath(QWidget* object, std::vector<int>&);
signals:
    void widgetDown();
    void widgetUp();
    void widgetRight();
    void widgetLeft();
    void animationFinished();
    void animationStarted();
};

#endif // ANIMATION_H
