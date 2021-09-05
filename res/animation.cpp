#include "animation.h"
#include <QObject>
//TODO
MoveAnimation::MoveAnimation(QLabel *character, QWidget *parent):QWidget(parent)//上0 下1 左2 右3
{
    QTimer *timer= new QTimer;
    timer->setInterval(500);

    connect(timer, &QTimer::timeout, [=](){
        static int cnt = 4;
        cnt++;
        if(cnt>4)cnt=1;
        QString fileDir = QString(":/pic/warrior_down_%1.png").arg(cnt);
        //move()
        character->setPixmap(fileDir);

    });

}
