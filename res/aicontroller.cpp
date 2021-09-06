#include "aicontroller.h"

AIController::AIController(int t_maxCount, QWidget* parent):
    QTimer(parent), count(0), maxCount(t_maxCount)
{
}
void AIController::reset()
{
    count = 0;
}
void AIController::start()
{

}
