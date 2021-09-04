#include "maindialog.h"
#include "character.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{

    m_x = 0; m_y = 0;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    setMouseTracking(true);
    setScreenMoveTimer();

    characterNum = 2;
    character[0] = new Warrior(2, 2, m_x, m_y, this);
    character[1] = new Warrior(5, 5, m_x, m_y, this);

    connect(this, &MainDialog::moveScreen, this, &MainDialog::redrawCharacter);
    connect(this, &MainDialog::moveScreen, [=](){repaint();});


    emit moveScreen();
}
MainDialog::~MainDialog()
{
}
void MainDialog::redrawCharacter()
{
    for(int i=0;i<characterNum;i++)
    {
        character[i]->show();
        character[i]->setGeometry((character[i]->m_localCellx - 1) * CELL_SIZE,
                          (character[i]->m_localCelly - 1) * CELL_SIZE, 64, 64);
        qDebug()<<(character[i]->m_localCelly - 1) * CELL_SIZE;
    }
}
void MainDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // 画背景图
    painter.drawPixmap(m_x, m_y, m_map.m_map1);

    // 方框标白
    painter.setPen(QPen(Qt::white,3));
    painter.drawRect((mouseCellx + m_x/CELL_SIZE - 1)*CELL_SIZE,
                     (mouseCelly + m_y/CELL_SIZE - 1)*CELL_SIZE,
                     CELL_SIZE, CELL_SIZE);
    painter.setPen(Qt::NoPen);
    // 加入血条
    for(int i=0;i<characterNum;i++)
    {
        painter.setBrush(Qt::red);
        painter.drawRect((character[i]->m_localCellx - 1) * CELL_SIZE,
                         (character[i]->m_localCelly - 1) * CELL_SIZE - HP_DISTANCE, (1.0 * character[i]->m_hp / character[i]->m_fullhp)*CELL_SIZE, HP_HEIGHT);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::black);
        painter.drawRect((character[i]->m_localCellx - 1) * CELL_SIZE,
                         (character[i]->m_localCelly - 1) * CELL_SIZE - HP_DISTANCE, CELL_SIZE, HP_HEIGHT);
        painter.setPen(Qt::NoPen);
    }

}
void MainDialog::mouseMoveEvent(QMouseEvent* event)
{
    updateMousePosition(event);
    checkScreenMove();
    update();
}
void MainDialog::updateMousePosition(QMouseEvent* event)
{

    QPointF point = event->localPos();
    mousex = point.x();
    mousey = point.y();

    mouseCellx = (-m_x + mousex) / CELL_SIZE + 1;
    mouseCelly = (-m_y + mousey) / CELL_SIZE + 1;

    mouseLocalCellx = (mousex) / CELL_SIZE + 1;
    mouseLocalCelly = (mousey) / CELL_SIZE + 1;
}
void MainDialog::checkScreenMove()
{
    if(mousex >= WINDOW_WIDTH - WINDOW_BOUNDARY
            && -m_x <= MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE)
        emit moveRight();
    if(-m_x > MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE
            || mousex < WINDOW_WIDTH - WINDOW_BOUNDARY)
        emit notMoveRight();

    if(mousex <= WINDOW_BOUNDARY && m_x <= 0)
        emit moveLeft();
    if(m_x > 0 || mousex > WINDOW_BOUNDARY)
        emit notMoveLeft();

    if(mousey >= WINDOW_HEIGHT - WINDOW_BOUNDARY
            && -m_y <= MAP_HEIGHT - WINDOW_HEIGHT - CELL_SIZE)
        emit moveDown();
    if(-m_y > MAP_HEIGHT - WINDOW_HEIGHT - CELL_SIZE
            || mousey < WINDOW_HEIGHT - WINDOW_BOUNDARY)
        emit notMoveDown();

    if(mousey <= WINDOW_BOUNDARY && m_y <= 0)
        emit moveUp();
    if(m_y > 0 || mousey > WINDOW_BOUNDARY)
        emit notMoveUp();


}
void MainDialog::setScreenMoveTimer()
{
    QTimer *rtimer = new QTimer(this);
    rtimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveRight()), rtimer, SLOT(start()));
    connect(this, SIGNAL(notMoveRight()),rtimer, SLOT(stop()));
    connect(rtimer, &QTimer::timeout, [this]()
    {
        if(-m_x >= MAP_WIDTH - WINDOW_WIDTH)
        {
            emit notMoveRight();
            return ;
        }
        for(int i = 0; i < characterNum; i++)
            character[i]->m_localCellx--;
        m_x -= CELL_SIZE;
        emit moveScreen();
    });

    QTimer *ltimer = new QTimer(this);
    ltimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveLeft()), ltimer, SLOT(start()));
    connect(this, SIGNAL(notMoveLeft()),ltimer, SLOT(stop()));
    connect(ltimer, &QTimer::timeout, [this]()
    {
        if(m_x >= 0)
        {
            emit notMoveLeft();
            return ;
        }

        for(int i = 0; i < characterNum; i++)
            character[i]->m_localCellx++;
        m_x += CELL_SIZE;
        emit moveScreen();
    });
    QTimer *dtimer = new QTimer(this);
    dtimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveDown()), dtimer, SLOT(start()));
    connect(this, SIGNAL(notMoveDown()),dtimer, SLOT(stop()));
    connect(dtimer, &QTimer::timeout, [this]()
    {
        if(-m_y >= MAP_HEIGHT - WINDOW_HEIGHT)
        {
            emit notMoveDown();
            return ;
        }
        for(int i = 0; i < characterNum; i++)
            character[i]->m_localCelly--;
        m_y -= CELL_SIZE;
        emit moveScreen();
    });

    QTimer *utimer = new QTimer(this);
    utimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveUp()), utimer, SLOT(start()));
    connect(this, SIGNAL(notMoveUp()),utimer, SLOT(stop()));
    connect(utimer, &QTimer::timeout, [this]()
    {
        if(m_y >= 0)
        {
            emit notMoveUp();
            return ;
        }
        for(int i = 0; i < characterNum; i++)
            character[i]->m_localCelly++;
        m_y += CELL_SIZE;
        emit moveScreen();
    });
}
