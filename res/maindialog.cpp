#include "maindialog.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    m_x = 0; m_y = 0;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    setMouseMoveTimer();
}

MainDialog::~MainDialog()
{
}
void MainDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(m_x, 0, m_map.m_map1);

    qDebug()<<m_x;
}
void MainDialog::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = event->localPos();
    int x = point.x();

    if(x >= WINDOW_WIDTH - WINDOW_BOUNDARY && -m_x <= MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE)
    {
        emit moveRight();
    }
    if(-m_x > MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE || x < WINDOW_WIDTH - WINDOW_BOUNDARY)
    {
        emit notMoveRight();
    }

    if(x <= WINDOW_BOUNDARY && m_x <= 0)
    {
        emit moveLeft();
    }
    if(m_x > 0 || x > WINDOW_BOUNDARY)
    {
        emit notMoveLeft();
    }

}
void MainDialog::setMouseMoveTimer()
{
    QTimer *rtimer = new QTimer(this);
    rtimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveRight()), rtimer, SLOT(start()));
    connect(this, SIGNAL(notMoveRight()),rtimer, SLOT(stop()));
    connect(rtimer, &QTimer::timeout, [=]()
    {
        if(-m_x >= MAP_WIDTH - WINDOW_WIDTH)
        {
            emit notMoveRight();
            return ;
        }
        m_x -= CELL_SIZE;
        repaint();
    });

    QTimer *ltimer = new QTimer(this);
    ltimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveLeft()), ltimer, SLOT(start()));
    connect(this, SIGNAL(notMoveLeft()),ltimer, SLOT(stop()));
    connect(ltimer, &QTimer::timeout, [=]()
    {
        if(m_x >= 0)
        {
            emit notMoveLeft();
            return ;
        }
        m_x += CELL_SIZE;
        repaint();
    });
}
