#include "animation.h"
#include <QObject>
//TODO
MoveAnimation::MoveAnimation(QWidget *parent):QWidget(parent)//上0 下1 左2 右3
{
}
void MoveAnimation::moveOneCell(QWidget* object, int direction, int duration)
{

    int beginx = object->x();
    int beginy = object->y();
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
        QTimer::singleShot(i * (1.5 * duration), [=](){
            moveOneCell(object, path[i], duration);
        });
    }
    QTimer::singleShot((path.size())*1.5*duration,[=](){
        emit animationFinished();
    });
}
AttrackAnimation::AttrackAnimation(QWidget *parent):
    QWidget(parent)
{
}

void AttrackAnimation::startMove(QWidget *object, int begincellx, int begincelly, int endcellx, int endcelly, int duration)
{

    emit animationStarted();
    int beginx = (begincellx - 1)*CELL_SIZE;
    int beginy = (begincelly - 1)*CELL_SIZE;
    int endx = (endcellx - 1)*CELL_SIZE;
    int endy = (endcelly - 1)*CELL_SIZE;

    QSequentialAnimationGroup *group = new QSequentialAnimationGroup;
    QPropertyAnimation *animation1 = new QPropertyAnimation(object,"geometry");
    animation1->setStartValue(QRect(beginx, beginy, 64, 64));
    animation1->setEndValue(QRect(endx, endy, 64, 64));
    animation1->setDuration(duration);
    group->addAnimation(animation1);

    QPropertyAnimation *animation2 = new QPropertyAnimation(object,"geometry");
    animation2->setEndValue(QRect(beginx, beginy, 64, 64));
    animation2->setStartValue(QRect(endx, endy, 64, 64));
    animation2->setDuration(duration);
    group->addAnimation(animation2);

    group->start();

    QTimer::singleShot(duration * 2, [=](){
        emit animationFinished();
    });
}


void zoom(QWidget* widget)
{
    int x = widget->x();
    int y = widget->y();
    QSequentialAnimationGroup *group = new QSequentialAnimationGroup;
    QPropertyAnimation* animation = new QPropertyAnimation(widget,"geometry");
    animation->setDuration(ZOOM_DURATION);
    animation->setStartValue(QRect(x, y, widget->width(),widget->height()));
    animation->setEndValue(QRect(x, y + 10,widget->width(),widget->height()));
    animation->setEasingCurve(QEasingCurve::OutBounce);
    group->addAnimation(animation);
    animation->setEndValue(QRect(x, y, widget->width(),widget->height()));
    animation->setStartValue(QRect(x, y + 10, widget->width(),widget->height()));
    group->addAnimation(animation);
    group->start();
    QTimer::singleShot(ZOOM_DURATION*2, [](){Sleep(100);});
}
