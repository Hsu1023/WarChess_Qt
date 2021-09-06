#ifndef AICONTROLLER_H
#define AICONTROLLER_H

#include <QObject>
#include "config.h"

class AIController : public QTimer
{
    Q_OBJECT
public:
    int count;
    int maxCount;
public:
    AIController(int t_maxCountQWidget, QWidget* parent = nullptr );
    void reset();
    void start();
signals:
    void AIFinished();
};

#endif // AICONTROLLER_H
