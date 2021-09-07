#include "gamescene.h"
#include "character.h"
#include "gamelabel.h"
#include "algorithm.h"
//TODO: cancelButton大一点易看见变化
GameScene::GameScene(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_x = 0; m_y = 0;

    nowCharacter = nullptr;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    playingMenu = new PlayingMenu(this);
    playingMenu->hide();
    connect(playingMenu, &PlayingMenu::exitGame, [=](){emit exit();});
    connect(playingMenu, &PlayingMenu::restartGame, [=](){emit restart();});

    setButton();
    hint = new HintLabel(this);
    hint->hide();

    setScreenMoveTimer();
    screenMoveOrNot = true;


    bgm = new QSound(":/music/bgm.wav",this);
    bgm->setLoops(QSound::Infinite);
    attrackSound = new QSound(":/music/attrack.wav", this);
    clickSound = new QSound(":/music/click.wav", this);

    characterNum = 3;
    character[0] = new Warrior(10, 10, m_x, m_y,YOURS, this);
    //character[3] = new Warrior(10, 15, m_x, m_y,MINE, this);
    character[1] = new Warrior(15, 14, m_x, m_y,YOURS, this);
    character[2] = new Warrior(3, 3, m_x, m_y, MINE, this);

    aliveNum[MINE] = aliveNum[YOURS] = 0;
    for(int i=0;i<characterNum;i++)
        aliveNum[character[i]->m_belong]++;
    roundBelonged=MINE;
    receiveHint("蓝方回合");
    roundNum[MINE]=aliveNum[MINE];
    roundNum[YOURS]=aliveNum[YOURS];

    connect(this, &GameScene::moveScreen, [=](){
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
        character[i]->setMouseTracking(true);
        connect(character[i],&Character::characterMoveAction,this,&GameScene::characterMoveEvent);
        connect(character[i],&Character::characterAttrackAction,this,&GameScene::characterAttrackEvent);
        connect(character[i],&Character::endOneCharacter,this,&GameScene::endOneCharacterEvent);
        connect(character[i],&Character::dieOneCharacter,this,&GameScene::dieOneCharacterEvent);
        connect(character[i],&Character::submitHint, this, &GameScene::receiveHint);
        connect(character[i],&Character::hideCancelButton, [=](){emit cancelButton->clicked();});
        connect(character[i]->mover,&MoveAnimation::animationStarted,[=](){screenMoveOrNot = false;});
        connect(character[i]->mover,&MoveAnimation::animationFinished,[=]()
        {
            //emit nowCharacter->infoChanged();
            screenMoveOrNot = true;
            if(AIOpenOrNot == false || character[i]->m_belong == MINE)
                character[i]->selectionDlg->show();
        });
        connect(character[i]->attracker,&AttrackAnimation::animationStarted,[=]()
        {
            screenMoveOrNot = false;
            attrackSound->play();
        });
        connect(character[i]->attracker,&AttrackAnimation::animationFinished,[=]()
        {
            //emit nowCharacter->infoChanged();
            screenMoveOrNot = true;
            if(AIOpenOrNot == false || character[i]->m_belong == MINE)
                character[i]->selectionDlg->show();
        });
    }

    AIOpenOrNot = true;
    aiController = new AIController(character, characterNum);
    connect(aiController, &AIController::AIRoundBegin, [=](){skipButton->hide();});
    connect(aiController, &AIController::AIRoundFinished, [=](){skipButton->show();nextRound(YOURS);});

    connect(this, &GameScene::myWin, [=](){
        resultMenu = new ResultMenu(1, AIOpenOrNot, this);
        resultMenu->show();
    });
    connect(this, &GameScene::myLoss, [=](){
        resultMenu = new ResultMenu(0, AIOpenOrNot, this);
        resultMenu->show();
    });

    emit moveScreen();
}
GameScene::~GameScene()
{
}
void GameScene::setButton()
{
    cancelButton = new ClickLabel(60,48,QPixmap(":/pic/cancel_button.png"),this);
    cancelButton->setGeometry(1150,830,60,48);
    cancelButton->setStyleSheet("border:none;");
    cancelButton->hide();

    skipButton = new ClickLabel(60,48,QPixmap(":/pic/skip_button.png"),this);
    skipButton->setGeometry(1250,830,60,48);
    skipButton->setStyleSheet("border:none;");

    musicButton = new ClickLabel(60,48,QPixmap(":/pic/music_button_on.png"),this);
    musicButton->setGeometry(1350,830,60,48);
    musicButton->setStyleSheet("border:none;");
    //bgm->play();


    menuButton = new ClickLabel(60,48,QPixmap(":/pic/menu_button.png"),this);
    menuButton->setGeometry(1450,830,60,48);
    menuButton->setStyleSheet("border:none;");

    connect(cancelButton, &ClickLabel::clicked, [=](){
       zoom(cancelButton);
       clickSound->play();
       if(nowCharacter->characterState == Character::DEAD)return;
       nowCharacter->characterState = Character::BEGIN;
       //TODO : bug - selectionDlg无法显示
       nowCharacter->selectionDlg->show();
       nowCharacter->selectionDlg->raise();
       gameState = BEGIN;
       cancelButton->hide();
       repaint();
    });
    connect(skipButton, &ClickLabel::clicked, [=](){
        zoom(skipButton);
        clickSound->play();
        //attrackSound->play();
        cancelButton->hide();
        if(nowCharacter)nowCharacter->selectionDlg->hide();
        nextRound(roundBelonged);
    });
    connect(musicButton, &ClickLabel::clicked, [=](){
        //emit myLoss();
        zoom(musicButton);
        clickSound->play();
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
        zoom(menuButton);
        clickSound->play();
        playingMenu->show();
        playingMenu->raise();
    });
}
void GameScene::redrawCharacter()
{
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]->characterState==Character::DEAD)continue;
        character[i]->show();
        character[i]->setGeometry((character[i]->m_localCellx - 1) * CELL_SIZE,
                          (character[i]->m_localCelly - 1) * CELL_SIZE, 64, 64);
    }
}
void GameScene::nextRound(int last)
{
    int next = last ^ 1;
    roundBelonged = next;
    gameState = BEGIN;
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
    //if(character[1]->characterState==Character::DEAD)qDebug()<<"DEAD";else qDebug()<<"EXISTING";
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
void GameScene::AIRound()
{
    aiController->reset(aliveNum[YOURS]);
    aiController->start();
}
void GameScene::characterMoveEvent(Character* t_nowCharacter)
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
void GameScene::characterAttrackEvent(Character* t_nowCharacter)
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
            //qDebug()<<i<<character[i]->m_cellx<<character[i]->m_celly;
        }
    }
    repaint();
    if(findOneOrNot == false)
    {
        receiveHint("没有可攻击的对象");
        QTimer::singleShot(500,[=](){emit cancelButton->clicked();});
    }
}
void GameScene::endOneCharacterEvent(Character* endedCharacter)
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
void GameScene::dieOneCharacterEvent(Character* deadCharacter)
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
    int alive = 0;
    for(int i = 0; i < characterNum; i++)
        if(character[i]->m_belong == deadCharacter->m_belong&&id!=i)
            alive++;
    aliveNum[character[id]->m_belong] = alive;
    if(aliveNum[character[id]->m_belong]==0)
    {
        hint->hide();
        if(character[id]->m_belong==MINE)emit myLoss();
        else emit myWin();
    }
    repaint();
}
void GameScene::paintEvent(QPaintEvent *)
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
        if(AIOpenOrNot == false)
            painter.drawText(1295, 160, QString("可行棋子：%1/%2").arg(roundNum[roundBelonged]).arg(aliveNum[roundBelonged]));
    }
    painter.setPen(Qt::NoPen);
    // 方框标白
    painter.setPen(QPen(Qt::white,4));
    painter.drawRect((mouseCellx + m_x/CELL_SIZE - 1)*CELL_SIZE,
                     (mouseCelly + m_y/CELL_SIZE - 1)*CELL_SIZE,
                     CELL_SIZE, CELL_SIZE);
    painter.setPen(Qt::NoPen);

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
        //画可攻击范围
        for(int i = nowCharacter->m_localCellx -nowCharacter->m_attrackable;
                i <= nowCharacter->m_localCellx +nowCharacter->m_attrackable; i++)
            for(int j = nowCharacter->m_localCelly -nowCharacter->m_attrackable;
                    j <= nowCharacter->m_localCelly +nowCharacter->m_attrackable; j++)
        {
            if(i<=1||i>=50)continue;
            if(j<=1||j>=30)continue;
            if(ManhattanDist(i,j,nowCharacter->m_localCellx,nowCharacter->m_localCelly)>nowCharacter->m_attrackable)continue;
            painter.setPen(QPen(QColor(255,150,0),3,Qt::DotLine));
            painter.drawRect((i-1)*CELL_SIZE,
                             (j-1)*CELL_SIZE,
                             CELL_SIZE, CELL_SIZE);
            painter.setPen(Qt::NoPen);
        }
    }
}
void GameScene::mousePressEvent(QMouseEvent* event)
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

                clickSound->play();
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

            clickSound->play();
            moveAl.findPath(nowCharacter->m_cellx, nowCharacter->m_celly, mouseCellx, mouseCelly, 0, moveAl.resultMap[mouseCellx][mouseCelly]);
            //for(int i=0;i<moveAl.path.size();i++){qDebug()<<moveAl.path[i];}
            nowCharacter->movePos(moveAl.resultMap[mouseCellx][mouseCelly],moveAl.path);
            gameState=BEGIN;
            cancelButton->hide();
            nowCharacter->characterState=BEGIN;
            //connect()
            repaint();
        }
        else return;
    }
    else if(gameState==FINDATTRACK)
    {
        if(attrackAl.resultMap[mouseCellx][mouseCelly]!=-1)//在攻击范围内
        {
            for(int i=0;i<characterNum;i++)
            {
                if(character[i]->m_belong==roundBelonged ||//友军
                    nowCharacter->characterState == Character::DEAD)//死了
                {
                    clickSound->play();
                    gameState=BEGIN;
                    nowCharacter->characterState=BEGIN;
                    nowCharacter->attrackedOrNot=false;
                    break;
                }
                if(character[i]->m_cellx==mouseCellx&&character[i]->m_celly==mouseCelly)//点中
                {
                    cancelButton->hide();
                    nowCharacter->attracker->startMove(nowCharacter, nowCharacter->m_localCellx, nowCharacter->m_localCelly,
                                             character[i]->m_localCellx, character[i]->m_localCelly);
                    connect(nowCharacter->attracker, &AttrackAnimation::animationFinished,[=](){
                        emit character[i]->beAttracked(nowCharacter->m_attrack);
                        gameState=BEGIN;
                        nowCharacter->characterState=BEGIN;
                        nowCharacter->attrackedOrNot=true;
                    });
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
void GameScene::receiveHint(QString str)
{
    hint->setText(str);
    if(hint->isHidden()==true)
        hint->show();
    QTimer::singleShot(1500,[=](){hint->hide();});
}
void GameScene::mouseMoveEvent(QMouseEvent* event)
{
    updateMousePosition(event);
    checkScreenMove();
    update();
}
void GameScene::updateMousePosition(QMouseEvent* event)
{
    QPointF point = event->localPos();
    mousex = point.x();
    mousey = point.y();

    mouseCellx = (-m_x + mousex) / CELL_SIZE + 1;
    mouseCelly = (-m_y + mousey) / CELL_SIZE + 1;

    mouseLocalCellx = (mousex) / CELL_SIZE + 1;
    mouseLocalCelly = (mousey) / CELL_SIZE + 1;
}
void GameScene::checkScreenMove()
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
void GameScene::setScreenMoveTimer()
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