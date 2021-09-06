#include "animation.h"
#include <QObject>
//TODO
MoveAnimation::MoveAnimation(QWidget *parent):QWidget(parent)//上0 下1 左2 右3
{
    //group = new QSequentialAnimationGroup();
}


void MoveAnimation::moveOneCell(QWidget* object, int duration, int direction)
{
    /*
    for(int i=1;i<=64;i++)
        QTimer::singleShot(5 * i,[=]()
        {
            object->move((object->m_localCellx-1)*CELL_SIZE, (object->m_localCelly-1)*CELL_SIZE+i*1);
            object->repaint();
        });
    */
    int beginx = object->x();
    int beginy = object->y();
    //qDebug()<<object->x();
    int endx = beginx;
    int endy = beginy;
    switch(direction)
    {
    case LEFT:  endx-=CELL_SIZE; emit widgetLeft(); break;
    case RIGHT: endx+=CELL_SIZE; emit widgetRight(); break;
    case UP: endy-=CELL_SIZE; emit widgetUp(); break;
    case DOWN: endy+=CELL_SIZE; emit widgetDown(); break;
    }

    QPropertyAnimation *animation = new QPropertyAnimation(object,"geometry");
    animation->setStartValue(QRect(beginx, beginy, 64, 64));
    animation->setEndValue(QRect(endx, endy, 64, 64));
    animation->setDuration(duration);
    animation->start();
}
void MoveAnimation::moveAlongPath(QWidget* object, std::vector<int>& path)
{
    int duration = ANIMATION_DURATION;
    emit animationStarted();
    for(ull i = 0; i < path.size(); i++)
    {
        /*
        int direction = -1;
        if(path[i+1].first - path[i].first == 1)direction = RIGHT;
        else if(path[i+1].first - path[i].first == -1)direction = LEFT;
        else if(path[i+1].second - path[i].second == 1)direction = DOWN;
        else direction = UP;
        */
        QTimer::singleShot(i * (1.5 * duration), [=](){
            moveOneCell(object, duration, path[i]);
        });
    }
    QTimer::singleShot((path.size())*1.5*duration,[=](){
        emit animationFinished();
    });
}

