#include "maindialog.h"
#include "character.h"
#include "algorithm.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    m_x = 0; m_y = 0;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    setMouseTracking(true);
    setScreenMoveTimer();

    characterNum = 2;
    character[0] = new Warrior(2, 2, m_x, m_y, this);
    character[1] = new Warrior(3, 2, m_x, m_y, this);

    connect(this, &MainDialog::moveScreen, this, &MainDialog::redrawCharacter);
    connect(this, &MainDialog::moveScreen, [=](){repaint();});

    gameState = BEGIN;

    for(int i=0;i<characterNum;i++)
    {
        connect(character[i],&Character::characterMoveAction,this,&MainDialog::characterMoveEvent);

        connect(character[i],&Character::characterAttrackAction,this,&MainDialog::characterAttrackEvent);
    }
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
    }
}
void MainDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // 画背景图
    painter.drawPixmap(m_x, m_y, m_map.m_map1);

    // 方框标白
    painter.setPen(QPen(Qt::white,4));
    painter.drawRect((mouseCellx + m_x/CELL_SIZE - 1)*CELL_SIZE,
                     (mouseCelly + m_y/CELL_SIZE - 1)*CELL_SIZE,
                     CELL_SIZE, CELL_SIZE);
    painter.setPen(Qt::NoPen);

    // 加入血条
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]->characterState==Character::DEAD)continue;
        painter.setBrush(Qt::red);
        painter.drawRect((character[i]->m_localCellx - 1) * CELL_SIZE,
                         (character[i]->m_localCelly - 1) * CELL_SIZE - HP_DISTANCE, (1.0 * character[i]->m_hp / character[i]->m_fullhp)*CELL_SIZE, HP_HEIGHT);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::black);
        painter.drawRect((character[i]->m_localCellx - 1) * CELL_SIZE,
                         (character[i]->m_localCelly - 1) * CELL_SIZE - HP_DISTANCE, CELL_SIZE, HP_HEIGHT);
        painter.setPen(Qt::NoPen);
    }
    if(gameState == FINDPATH)
    {
        for(int i=1;i<=50;i++)
            for(int j=1;j<=30;j++)
                if(moveAl.resultMap[i][j]!=-1)
                {
                    painter.setPen(QPen(Qt::blue,2));
                    painter.drawRect((i + m_x/CELL_SIZE - 1)*CELL_SIZE,
                                     (j + m_y/CELL_SIZE - 1)*CELL_SIZE,
                                     CELL_SIZE, CELL_SIZE);
                    painter.setPen(Qt::NoPen);
                }
        //TODO: hint there's no way
    }

    else if(gameState==FINDATTRACK)
    {
        //寻找可攻击对象
        for(int i=0;i<characterNum;i++)
        {
            if(character[i]->characterState!=BEGIN&&character[i]->characterState!=Character::END)continue;
            if(character[i]==nowCharacter)continue;
            //TODO: enemy continue
            if(attrackAl.resultMap[character[i]->m_cellx][character[i]->m_celly]!=-1)
            {
                painter.setPen(QPen(Qt::red,2));
                painter.drawRect((character[i]->m_cellx + m_x/CELL_SIZE - 1)*CELL_SIZE,
                                 (character[i]->m_celly + m_y/CELL_SIZE - 1)*CELL_SIZE,
                                 CELL_SIZE, CELL_SIZE);
                painter.setPen(Qt::NoPen);
            }

            //TODO: hint Nobody can be attracked
        }
    }
}
void MainDialog::mousePressEvent(QMouseEvent* event)
{
    updateMousePosition(event);
    if(gameState==BEGIN)
    {
        //选择对话框
        for(int i = 0; i <characterNum; i++)
        {
            if(character[i]->characterState!=BEGIN)continue;
            //点中了人物
            if(mouseLocalCellx==character[i]->m_localCellx&&
                    mouseLocalCelly==character[i]->m_localCelly)
            {
                if(character[i]->characterState==Character::END||
                        character[i]->characterState==Character::DEAD)continue;
                //显示选择框
                if(character[i]->selectionDlg->isHidden())
                {
                    character[i]->selectionDlg->show();
                    character[i]->selectionDlg->updateData(mouseCellx, mouseCelly);
                    character[i]->selectionDlg->raise();
                }
            }
            else
            {   //点错收回选择框
                if(character[i]->selectionDlg->isHidden()==false)
                    character[i]->selectionDlg->hide();
            }
        }
    }
    else if(gameState==FINDPATH)
    {
        if(moveAl.resultMap[mouseCellx][mouseCelly]!=-1)
        {
            nowCharacter->m_move-=moveAl.resultMap[mouseCellx][mouseCelly];
            nowCharacter->m_cellx = mouseCellx;
            nowCharacter->m_celly = mouseCelly;
            nowCharacter->m_localCellx = mouseLocalCellx;
            nowCharacter->m_localCelly = mouseLocalCelly;
            nowCharacter->move((mouseCellx-1)*CELL_SIZE, (mouseCelly-1)*CELL_SIZE);
            emit nowCharacter->infoChanged();
            //TODO: Moving animation
            gameState=BEGIN;
            nowCharacter->characterState=BEGIN;
            nowCharacter->selectionDlg->show();
            nowCharacter->selectionDlg->raise();
        }
        else return;
    }
    else if(gameState==FINDATTRACK)
    {
        if(attrackAl.resultMap[mouseCellx][mouseCelly]!=-1)
        {
            for(int i=0;i<characterNum;i++)
            {
                if(character[i]->m_cellx==mouseCellx&&character[i]->m_celly==mouseCelly)
                {
                    emit character[i]->beAttracked(nowCharacter->m_attrack);
                    gameState=BEGIN;
                    nowCharacter->characterState=BEGIN;
                    nowCharacter->attrackedOrNot=true;
                    break;
                }
            }
        }
            //TODO:
    }
}
void MainDialog::characterMoveEvent(Character* t_nowCharacter)
{
    nowCharacter=t_nowCharacter;
    memset(moveAl.resultMap, -1, sizeof moveAl.resultMap);
    gameState=FINDPATH;
    moveAl.setMove(nowCharacter->m_move, nowCharacter);
    moveAl.findAvailableCell(nowCharacter->m_cellx, nowCharacter->m_celly, 0,character,characterNum);
    for(int i=0;i<characterNum;i++)
        if(nowCharacter!=character[i])
        {
            moveAl.resultMap[character[i]->m_cellx][character[i]->m_celly]=-1;
        }
    repaint();
}
void MainDialog::characterAttrackEvent(Character* t_nowCharacter)
{
    nowCharacter=t_nowCharacter;
    memset(attrackAl.resultMap, -1, sizeof attrackAl.resultMap);
    gameState=FINDATTRACK;
    //attrackAl.setMove(nowCharacter->m_attrackable, nowCharacter);
   // attrackAl.findAttrackableCell(nowCharacter->m_cellx, nowCharacter->m_celly, 0,character,characterNum);
    //寻找可攻击目标并标出
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]==nowCharacter)continue;//TODO enemy
        if(character[i]->m_cellx==nowCharacter->m_cellx&&
                abs(character[i]->m_celly-nowCharacter->m_celly)<=nowCharacter->m_attrackable)
        {
            attrackAl.resultMap[character[i]->m_cellx][character[i]->m_celly]=1;
        }
        if(character[i]->m_celly==nowCharacter->m_celly&&
                abs(character[i]->m_cellx-nowCharacter->m_cellx)<=nowCharacter->m_attrackable)
        {
            attrackAl.resultMap[character[i]->m_cellx][character[i]->m_celly]=1;
        }
    }
    qDebug()<<attrackAl.resultMap[2][2];
    repaint();
}
void MainDialog::mouseMoveEvent(QMouseEvent* event)
{
    updateMousePosition(event);
    checkScreenMove();
    update();
    //qDebug()<<mouseCellx<<"/"<<mouseCelly;
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
        {
            character[i]->m_localCellx--;
            emit character[i]->infoChanged();
        }
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
        {
            character[i]->m_localCellx++;
            emit character[i]->infoChanged();
        }
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
        {
            character[i]->m_localCelly--;
            emit character[i]->infoChanged();
        }
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
        {
            character[i]->m_localCelly++;
            emit character[i]->infoChanged();
        }
        m_y += CELL_SIZE;
        emit moveScreen();
    });
}
