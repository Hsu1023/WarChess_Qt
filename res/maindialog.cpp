#include "maindialog.h"
#include "character.h"
#include "hintlabel.h"
#include "algorithm.h"
//TODO: cancelButton大一点易看见变化
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    m_x = 0; m_y = 0;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);
    setScreenMoveTimer();

    setButton();
    hint = new HintLabel(this);
    hint->hide();

    characterNum = 4;
    character[0] = new Warrior(10, 10, m_x, m_y, this);characterBelonged[0]=YOURS;
    character[1] = new Warrior(10, 15, m_x, m_y, this);characterBelonged[1]=MINE;
    character[2] = new Warrior(15, 14, m_x, m_y, this);characterBelonged[2]=YOURS;
    character[3] = new Warrior(3, 3, m_x, m_y, this);characterBelonged[3]=MINE;

    aliveNum[MINE] = aliveNum[YOURS] = 0;
    for(int i=0;i<characterNum;i++)
        aliveNum[characterBelonged[i]]++;
    roundBelonged=MINE;
    receiveHint("蓝方回合");
    roundNum[MINE]=aliveNum[MINE];
    roundNum[YOURS]=aliveNum[YOURS];

    connect(this, &MainDialog::moveScreen, this, &MainDialog::redrawCharacter);
    connect(this, &MainDialog::moveScreen, [=](){
        for(int i=0;i<characterNum;i++)
        {
            if(character[i]->characterState!=Character::DEAD)
                character[i]->updateInfo();
        }
        repaint();
    });

    gameState = BEGIN;

    for(int i=0;i<characterNum;i++)
    {
        connect(character[i],&Character::characterMoveAction,this,&MainDialog::characterMoveEvent);
        connect(character[i],&Character::characterAttrackAction,this,&MainDialog::characterAttrackEvent);
        connect(character[i],&Character::endOneCharacter,this,&MainDialog::endOneCharacterEvent);
        connect(character[i],&Character::dieOneCharacter,this,&MainDialog::dieOneCharacterEvent);
        connect(character[i],&Character::submitHint, this, &MainDialog::receiveHint);
        connect(character[i],&Character::hideCancelButton, [=](){emit cancelButton->clicked();});
    }
    emit moveScreen();

}
MainDialog::~MainDialog()
{
}
void MainDialog::setButton()
{
    cancelButton = new ClickLabel(this);
    cancelButton->setPixmap(QPixmap(":/pic/cancel_button.png"));
    cancelButton->setGeometry(1150,830,60,48);
    cancelButton->setStyleSheet("border:none;");
    cancelButton->hide();

    skipButton = new ClickLabel(this);
    skipButton->setPixmap(QPixmap(":/pic/skip_button.png"));
    skipButton->setGeometry(1250,830,60,48);
    skipButton->setStyleSheet("border:none;");

    musicButton = new ClickLabel(this);
    musicButton->setPixmap(QPixmap(":/pic/music_button.png"));
    musicButton->setGeometry(1350,830,60,48);
    musicButton->setStyleSheet("border:none;");

    menuButton = new ClickLabel(this);
    menuButton->setPixmap(QPixmap(":/pic/menu_button.png"));
    menuButton->setGeometry(1450,830,60,48);
    menuButton->setStyleSheet("border:none;");

    connect(cancelButton, &ClickLabel::clicked, [=](){
       nowCharacter->characterState = BEGIN;
       //TODO : bug - selectionDlg无法显示
       nowCharacter->selectionDlg->show();
       nowCharacter->selectionDlg->raise();
       gameState = BEGIN;
       cancelButton->hide();
       repaint();
    });
    connect(skipButton, &ClickLabel::clicked, [=](){
        cancelButton->hide();
        nextRound(roundBelonged);
    });
    connect(musicButton, &ClickLabel::clicked, [=](){
        //TODO:
    });
    connect(menuButton, &ClickLabel::clicked, [=](){
        //TODO:
    });
}
void MainDialog::redrawCharacter()
{
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]->characterState==Character::DEAD)continue;
        character[i]->show();
        character[i]->setGeometry((character[i]->m_localCellx - 1) * CELL_SIZE,
                          (character[i]->m_localCelly - 1) * CELL_SIZE, 64, 64);
    }
}
void MainDialog::nextRound(int last)
{
    int next = last ^ 1;
    roundBelonged = next;
    gameState = BEGIN;
    //更新下一回合character
    for(int i = 0; i < characterNum; i++)
    {
        if(characterBelonged[i]==next&&character[i]->characterState!=Character::DEAD)
        {
            character[i]->characterState = Character::BEGIN;
            character[i]->attrackedOrNot = false;
            character[i]->m_move = character[i]->m_fullmove;
        }
    }
    roundNum[next] = aliveNum[next];
    if(last==MINE)receiveHint("红方回合");
    else receiveHint("蓝方回合");
    repaint();
}
void MainDialog::endOneCharacterEvent(Character* endedCharacter)
{
    int id=-1;
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]==endedCharacter)
        {
            id=i;
            break;
        }
    }
    roundNum[characterBelonged[id]]--;
    cancelButton->hide();
    qDebug()<<roundNum[characterBelonged[id]];
    if(roundNum[characterBelonged[id]]==0)
        nextRound(characterBelonged[id]);
}
void MainDialog::dieOneCharacterEvent(Character* deadCharacter)
{
    int id=-1;
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]==deadCharacter)
        {
            id=i;
            break;
        }
    }
    aliveNum[characterBelonged[id]]--;
    if(aliveNum[characterBelonged[id]])
    {
        if(characterBelonged[id]==MINE)emit myLoss();
        else emit myWin();
    }
}
void MainDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 画背景图
    painter.drawPixmap(m_x, m_y, m_map.m_map1);

    //右上角
    if(roundBelonged==MINE)
        painter.setBrush(Qt::blue);
    else painter.setBrush(Qt::red);
    painter.drawRect(1500, 100, 50, 50);
    painter.setBrush(Qt::NoBrush);


    if(roundBelonged==MINE)
    {
        painter.setPen(Qt::blue);
        QFont font("隶书",25,60,true);
        painter.setFont(font);
        painter.drawText(1290, 120, "蓝方回合");
        painter.setFont(QFont("宋体",15,40,false));
        painter.drawText(1295, 160, QString("可行棋子：%1/%2").arg(roundNum[roundBelonged]).arg(aliveNum[roundBelonged]));
    }
    else
    {
        painter.setPen(Qt::red);
        painter.setFont(QFont("隶书",25,60,true));
        painter.drawText(1290,120, "红方回合");
        painter.setFont(QFont("宋体",15,40,false));
        painter.drawText(1295, 160, QString("可行棋子：%1/%2").arg(roundNum[roundBelonged]).arg(aliveNum[roundBelonged]));
    }
    painter.setPen(Qt::NoPen);
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

        if(characterBelonged[i]==YOURS)//敌人
            painter.setBrush(Qt::red);
        else
            painter.setBrush(Qt::blue);
        painter.drawRect((character[i]->m_localCellx - 1) * CELL_SIZE,
                         (character[i]->m_localCelly - 1) * CELL_SIZE - HP_DISTANCE, (1.0 * character[i]->m_hp / character[i]->m_fullhp)*CELL_SIZE, HP_HEIGHT);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::black);
        painter.drawRect((character[i]->m_localCellx - 1) * CELL_SIZE,
                         (character[i]->m_localCelly - 1) * CELL_SIZE - HP_DISTANCE, CELL_SIZE, HP_HEIGHT);
        painter.setPen(Qt::NoPen);
    }
    //寻路，把路标蓝
    if(gameState == FINDPATH)
    {
        for(int i=1;i<=50;i++)
            for(int j=1;j<=30;j++)
                if(moveAl.resultMap[i][j]!=-1)
                {
                    painter.setPen(QPen(QColor(0,150,255),2,Qt::DotLine));
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
            //if(character[i]->characterState!=BEGIN&&character[i]->characterState!=Character::END)continue;
            if(character[i]->characterState==Character::DEAD)continue;
            if(characterBelonged[i]==roundBelonged)continue;//友军
            if(attrackAl.resultMap[character[i]->m_cellx][character[i]->m_celly]!=-1)
            {//找到可攻击对象标红
                painter.setPen(QPen(Qt::red,2));
                painter.drawRect((character[i]->m_cellx + m_x/CELL_SIZE - 1)*CELL_SIZE,
                                 (character[i]->m_celly + m_y/CELL_SIZE - 1)*CELL_SIZE,
                                 CELL_SIZE, CELL_SIZE);
                painter.setPen(Qt::NoPen);
            }
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
            //敌军
            if(characterBelonged[i]!=roundBelonged)continue;
            else
            {   //点错收回选择框
                if(character[i]->selectionDlg->isHidden()==false)
                    character[i]->selectionDlg->hide();
            }

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
                    character[i]->selectionDlg->updateData(mouseLocalCellx, mouseLocalCelly);
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
        //可行域
        if(moveAl.resultMap[mouseCellx][mouseCelly]!=-1)
        {
            nowCharacter->m_move-=moveAl.resultMap[mouseCellx][mouseCelly];
            nowCharacter->m_cellx = mouseCellx;
            nowCharacter->m_celly = mouseCelly;
            nowCharacter->m_localCellx = mouseLocalCellx;
            nowCharacter->m_localCelly = mouseLocalCelly;
            nowCharacter->move((mouseLocalCellx-1)*CELL_SIZE, (mouseLocalCelly-1)*CELL_SIZE);
            nowCharacter->updateInfo();
            //TODO: Moving animation
            gameState=BEGIN;
            nowCharacter->characterState=BEGIN;
            nowCharacter->selectionDlg->show();
            nowCharacter->selectionDlg->raise();
            cancelButton->hide();
        }
        else return;
    }
    else if(gameState==FINDATTRACK)
    {
        if(attrackAl.resultMap[mouseCellx][mouseCelly]!=-1)
        {
            for(int i=0;i<characterNum;i++)
            {
                if(characterBelonged[i]==roundBelonged)continue;//友军
                if(character[i]->m_cellx==mouseCellx&&character[i]->m_celly==mouseCelly)//点中
                {
                    emit character[i]->beAttracked(nowCharacter->m_attrack);
                    gameState=BEGIN;
                    nowCharacter->characterState=BEGIN;
                    nowCharacter->attrackedOrNot=true;
                    cancelButton->hide();
                    break;
                }
            }
        }
        else//偷懒了
        {
            gameState=BEGIN;
            nowCharacter->characterState=BEGIN;
            nowCharacter->attrackedOrNot=false;
        }
    }
}
void MainDialog::receiveHint(QString str)
{
    hint->setText(str);
    if(hint->isHidden()==true)
        hint->show();
    QTimer::singleShot(1500,[=](){hint->hide();});
}
void MainDialog::characterMoveEvent(Character* t_nowCharacter)
{
    cancelButton->show();
    nowCharacter=t_nowCharacter;
    memset(moveAl.resultMap, -1, sizeof moveAl.resultMap);
    gameState=FINDPATH;
    int tempx = nowCharacter->m_cellx;
    int tempy = nowCharacter->m_celly;
    moveAl.setMove(nowCharacter->m_move, nowCharacter);
    moveAl.findAvailableCell(tempx, tempy, 0,character,characterNum);
    for(int i=0;i<characterNum;i++)
        if(nowCharacter!=character[i])
        {
            moveAl.resultMap[character[i]->m_cellx][character[i]->m_celly]=-1;
        }
    if(moveAl.resultMap[tempx+1][tempy]==-1&&moveAl.resultMap[tempx-1][tempy]==-1
            &&moveAl.resultMap[tempx][tempy+1]==-1&&moveAl.resultMap[tempx][tempy-1]==-1)
    {
        receiveHint("没有可移至的地块");
        emit cancelButton->clicked();
    }
    repaint();
}
void MainDialog::characterAttrackEvent(Character* t_nowCharacter)
{
    cancelButton->show();
    nowCharacter=t_nowCharacter;
    memset(attrackAl.resultMap, -1, sizeof attrackAl.resultMap);
    gameState=FINDATTRACK;
    //attrackAl.setMove(nowCharacter->m_attrackable, nowCharacter);
   // attrackAl.findAttrackableCell(nowCharacter->m_cellx, nowCharacter->m_celly, 0,character,characterNum);
    bool findOneOrNot = false;
    //寻找可攻击目标并标出
    for(int i=0;i<characterNum;i++)
    {
        if(characterBelonged[i]==roundBelonged)continue;//友军
        if(character[i]->m_cellx==nowCharacter->m_cellx&&
                abs(character[i]->m_celly-nowCharacter->m_celly)<=nowCharacter->m_attrackable)
        {
            attrackAl.resultMap[character[i]->m_cellx][character[i]->m_celly]=1;
            findOneOrNot = true;
        }
        if(character[i]->m_celly==nowCharacter->m_celly&&
                abs(character[i]->m_cellx-nowCharacter->m_cellx)<=nowCharacter->m_attrackable)
        {
            attrackAl.resultMap[character[i]->m_cellx][character[i]->m_celly]=1;
            findOneOrNot = true;
        }
    }
    if(findOneOrNot == false)
    {
        receiveHint("没有可攻击的对象");
        emit cancelButton->clicked();
    }
    repaint();
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
        {
            character[i]->m_localCellx--;
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
        }
        m_y += CELL_SIZE;
        emit moveScreen();
    });
}
