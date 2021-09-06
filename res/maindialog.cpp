#include "maindialog.h"
#include "character.h"
#include "hintlabel.h"
#include "algorithm.h"
//TODO: cancelButton大一点易看见变化
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    m_x = 0; m_y = 0;

    nowCharacter = nullptr;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    setButton();
    hint = new HintLabel(this);
    hint->hide();

    AItimer = new QTimer;
    gameAI = new GameAI;
    connect(gameAI, &GameAI::repaintScreen, [=](){repaint();});

    setScreenMoveTimer();
    screenMoveOrNot = true;

    characterNum = 4;
    character[0] = new Warrior(10, 10, m_x, m_y,YOURS, this);
    character[1] = new Warrior(10, 15, m_x, m_y,MINE, this);
    character[3] = new Warrior(15, 14, m_x, m_y,YOURS, this);
    character[2] = new Warrior(3, 3, m_x, m_y, MINE, this);


    aliveNum[MINE] = aliveNum[YOURS] = 0;
    for(int i=0;i<characterNum;i++)
        aliveNum[character[i]->m_belong]++;
    roundBelonged=MINE;
    receiveHint("蓝方回合");
    roundNum[MINE]=aliveNum[MINE];
    roundNum[YOURS]=aliveNum[YOURS];
    AIOpenOrNot = true;

    connect(this, &MainDialog::moveScreen, [=](){
        for(int i=0;i<characterNum;i++)
        {
            redrawCharacter();
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
        connect(character[i]->mover,&MoveAnimation::animationStarted,[=](){screenMoveOrNot = false;});
        connect(character[i]->mover,&MoveAnimation::animationFinished,[=]()
        {
            screenMoveOrNot = true;
            if(AIOpenOrNot == false || character[i]->m_belong == MINE)
                character[i]->selectionDlg->show();
        });
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
    musicButton->setPixmap(QPixmap(":/pic/music_button_on.png"));
    musicButton->setGeometry(1350,830,60,48);
    musicButton->setStyleSheet("border:none;");
    bgm = new QSound(":/music/bgm.wav",this);
    bgm->setLoops(QSound::Infinite);
    //bgm->play();

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
        if(nowCharacter)nowCharacter->selectionDlg->hide();
        nextRound(roundBelonged);
    });
    connect(musicButton, &ClickLabel::clicked, [=](){
        //std::vector<int>x; x.push_back(UP); x.push_back(DOWN); x.push_back(LEFT); x.push_back(RIGHT);ani->moveAlongPath(character[0],x);
        if(bgm->isFinished()==true)
        {
            musicButton->setPixmap(QPixmap(":/pic/music_button_on.png"));
            bgm->play();
        }
        else
        {
            musicButton->setPixmap(QPixmap(":/pic/music_button_off.png"));
            bgm->stop();
        }
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
    AItimer->stop();
    //更新下一回合character
    for(int i = 0; i < characterNum; i++)
    {
        if(character[i]->m_belong==next&&character[i]->characterState!=Character::DEAD)
        {
            character[i]->characterState = Character::BEGIN;
            character[i]->attrackedOrNot = false;
            character[i]->m_move = character[i]->m_fullmove;
        }
    }
    roundNum[next] = aliveNum[next];
    if(last==MINE)receiveHint("红方回合");
    else receiveHint("蓝方回合");
    if(AIOpenOrNot == true && next == YOURS)
    {
        gameState = AI;
        AIRound();
    }
    repaint();
}
int timercount = 0;
void MainDialog::AIRound()
{
    AItimer->setInterval(1000);
    AItimer->start();
    connect(AItimer, &QTimer::timeout,[&](){
        qDebug()<<"time";
        for(;timercount<characterNum;timercount++)
        {
            if(character[timercount]->m_belong == YOURS && character[timercount]->characterState!=Character::DEAD)
            {
                gameAI->moveCharacter(timercount,character,characterNum);
                repaint();
                timercount++;
                return;
            }
        }
        if(timercount==characterNum)
        {
            timercount=0;
            nextRound(YOURS);
        }
    });
}
void MainDialog::characterMoveEvent(Character* t_nowCharacter)
{
    gameState=FINDPATH;
    cancelButton->show();
    nowCharacter=t_nowCharacter;
    int tempx = nowCharacter->m_cellx;
    int tempy = nowCharacter->m_celly;
    moveAl.init(nowCharacter->m_move, nowCharacter);
    moveAl.findAvailableCell(tempx, tempy, 0,character,characterNum);
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
    gameState=FINDATTRACK;
    cancelButton->show();
    nowCharacter=t_nowCharacter;
    attrackAl.init(nowCharacter->m_attrackable, nowCharacter);
    bool findOneOrNot = false;
    //寻找可攻击目标并标出
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]->m_belong==roundBelonged||character[i]->characterState==Character::DEAD)continue;//友军
        if(ManhattanDist(character[i]->m_cellx, character[i]->m_celly,
                         nowCharacter->m_cellx, nowCharacter->m_celly)<=nowCharacter->m_attrackable)
        {
            attrackAl.resultMap[character[i]->m_cellx][character[i]->m_celly] = 1;
            attrackAl.v.push_back(node(character[i]->m_cellx, character[i]->m_celly));
            findOneOrNot = true;
            qDebug()<<i<<character[i]->m_cellx<<character[i]->m_celly;
        }
    }
    repaint();
    if(findOneOrNot == false)
    {
        receiveHint("没有可攻击的对象");
        QTimer::singleShot(500,[=](){emit cancelButton->clicked();});
    }
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
    roundNum[character[id]->m_belong]--;
    cancelButton->hide();
    if(roundNum[character[id]->m_belong]==0)
        nextRound(character[id]->m_belong);
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
    aliveNum[character[id]->m_belong]--;
    if(aliveNum[character[id]->m_belong]==0)
    {
        if(character[id]->m_belong==MINE)emit myLoss();
        else emit myWin();
    }
    repaint();
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
/*
    // 加入血条
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]->characterState==Character::DEAD)continue;

        if(character[i]->m_belong==YOURS)//敌人
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
    */
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
    }

    else if(gameState==FINDATTRACK)
    {
        //寻找可攻击对象
        for(ull i = 0; i < attrackAl.v.size(); i++)
        {
            painter.setPen(QPen(Qt::red,3));
            painter.setBrush(QBrush(Qt::red));
            painter.drawRect((attrackAl.v[i].first + m_x/CELL_SIZE - 1)*CELL_SIZE,
                             (attrackAl.v[i].second + m_y/CELL_SIZE - 1)*CELL_SIZE,
                             CELL_SIZE, CELL_SIZE);
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::NoBrush);
        }
        for(int i = nowCharacter->m_localCellx -nowCharacter->m_attrackable;
                i <= nowCharacter->m_localCellx +nowCharacter->m_attrackable; i++)
            for(int j = nowCharacter->m_localCelly -nowCharacter->m_attrackable;
                    j <= nowCharacter->m_localCelly +nowCharacter->m_attrackable; j++)
        {
            if(i<=1||i>=50)continue;
            if(j<=1||j>=30)continue;
            if(ManhattanDist(i,j,nowCharacter->m_localCellx,nowCharacter->m_localCelly)>nowCharacter->m_attrackable)continue;
            painter.setPen(QPen(QColor(255,150,0),2,Qt::DotLine));
            painter.drawRect((i-1)*CELL_SIZE,
                             (j-1)*CELL_SIZE,
                             CELL_SIZE, CELL_SIZE);
            painter.setPen(Qt::NoPen);
        }
    }
}
void MainDialog::mousePressEvent(QMouseEvent* event)
{
    updateMousePosition(event);

    if(gameState==AI)return;

    if(gameState==BEGIN)
    {
        //选择对话框
        for(int i = 0; i <characterNum; i++)
        {
            if(character[i]->characterState!=BEGIN)continue;
            //敌军
            if(character[i]->m_belong!=roundBelonged)continue;
            else
            {   //点错收回选择框
                if(character[i]->selectionDlg->isHidden()==false)
                    character[i]->selectionDlg->hide();
            }
            //应该封装
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
            moveAl.findPath(nowCharacter->m_cellx, nowCharacter->m_celly, mouseCellx, mouseCelly, 0, moveAl.resultMap[mouseCellx][mouseCelly]);
            //for(int i=0;i<moveAl.path.size();i++){qDebug()<<moveAl.path[i];}
            nowCharacter->movePos(mouseCellx, mouseCelly, mouseLocalCellx, mouseLocalCelly, moveAl.resultMap[mouseCellx][mouseCelly],moveAl.path);
            gameState=BEGIN;
            nowCharacter->characterState=BEGIN;
            nowCharacter->selectionDlg->show();
            nowCharacter->selectionDlg->raise();
            cancelButton->hide();
            repaint();
        }
        else return;
    }
    else if(gameState==FINDATTRACK)
    {
        if(attrackAl.resultMap[mouseCellx][mouseCelly]!=-1)
        {
            for(int i=0;i<characterNum;i++)
            {
                if(character[i]->m_belong==roundBelonged)continue;//友军
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
        repaint();
    }
}
void MainDialog::receiveHint(QString str)
{
    hint->setText(str);
    if(hint->isHidden()==true)
        hint->show();
    QTimer::singleShot(1500,[=](){hint->hide();});
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
        if(screenMoveOrNot == false)return;
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
        if(screenMoveOrNot == false)return;
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
        if(screenMoveOrNot == false)return;
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
        if(screenMoveOrNot == false)return;
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
