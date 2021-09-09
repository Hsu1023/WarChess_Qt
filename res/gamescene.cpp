#include "gamescene.h"
#include "characterrole.h"
#include "gamelabel.h"
#include "algorithm.h"
#include <QApplication>
#include <QPixmap>
#include <QScreen>
//TODO: cancelButton大一点易看见变化

std::vector<QImage> GameScene::imageSaver{};

GameScene::GameScene(int chapter, int gameMode, QWidget *parent)
    : QDialog(parent), m_map(GameMap(chapter))
{

    m_x = 0; m_y = 0;

    nowCharacter = nullptr;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    videoShower = new QTimer(this);
    videoLabel = new QLabel(this);

    setButton();

    hint = new HintLabel(this);
    hint->hide();

    setScreenMoveTimer();
    screenMoveOrNot = true;

    bgm = new QSound(":/music/bgm.wav", this);
    bgm->setLoops(QSound::Infinite);
    attrackSound = new QSound(":/music/attrack.wav", this);
    clickSound = new QSound(":/music/click.wav", this);

    createCharacter();

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

        character[i]->installEventFilter(this);
        connect(character[i],&Character::repaintScreen,this,[=](){repaint();});
        connect(character[i],&Character::characterMoveAction,this,&GameScene::characterMoveEvent);
        connect(character[i],&Character::characterAttrackAction,this,&GameScene::characterAttrackEvent);
        connect(character[i],&Character::endOneCharacter,this,&GameScene::endOneCharacterEvent);
        connect(character[i],&Character::dieOneCharacter,this,&GameScene::dieOneCharacterEvent);
        connect(character[i],&Character::submitHint, this, &GameScene::receiveHint);
        connect(character[i],&Character::hideCancelButton,this, [=](){emit cancelButton->clicked();});
        connect(character[i]->mover,&MoveAnimation::animationStarted,this,[=](){screenMoveOrNot = false;});
        connect(character[i]->mover,&MoveAnimation::animationFinished,this,[=]()
        {
            screenMoveOrNot = true;
            if(AIOpenOrNot == false || character[i]->m_belong == MINE)
                character[i]->selectionDlg->show();
        });
        connect(character[i]->mover,&MoveAnimation::moveOneCellFinished,this,[=]()
        {
            minimap->repaint();
        });
        connect(character[i]->attracker,&AttrackAnimation::animationStarted, this, [=]()
        {
            screenMoveOrNot = false;
            attrackSound->play();
        });
        connect(character[i]->attracker,&AttrackAnimation::animationFinished, this, [=]()
        {
            //emit nowCharacter->infoChanged();
            screenMoveOrNot = true;
            if(AIOpenOrNot == false || character[i]->m_belong == MINE)
                character[i]->selectionDlg->show();
        });
    }

    AIOpenOrNot = gameMode;
    aiController = new AIController(character, characterNum, this);
    connect(aiController, &AIController::AIRoundBegin, this, [=](){skipButton->hide();});
    connect(aiController, &AIController::AIRoundFinished, this, [=](){skipButton->show();nextRound(YOURS);});

    resultMenu = new ResultMenu(this);
    resultMenu->hide();
    connect(resultMenu, &ResultMenu::exitGame, this, [=](){videoShower->stop();bgm->stop();emit exit();});
    connect(resultMenu, &ResultMenu::restartGame, this, [=](){videoShower->stop();bgm->stop();emit restart();});
    connect(this, &GameScene::myWin, [=](){
        hint->close();
        skipButton->close();
        menuButton->close();
        musicButton->close();
        resultMenu->setResult(1, AIOpenOrNot);
        resultMenu->show();
        resultMenu->raise();
    });
    connect(this, &GameScene::myLoss, [=](){
        hint->close();
        skipButton->close();
        menuButton->close();
        musicButton->close();
        resultMenu->setResult(0, AIOpenOrNot);
        QTimer::singleShot(500,this,[=](){

            resultMenu->show();
            resultMenu->raise();
        });
    });

    for(int i=0;i<characterNum;i++)
    {
        redrawCharacter();
        if(character[i]->characterState!=Character::DEAD)
            character[i]->updateInfo();
    }

    repaint();
    screenCapturing = true;
    imageSaver.clear();
    screenCaptureTimer = new QTimer(this);
    screenCaptureTimer->setInterval(CAPTURE_SCREEN_INTERVAL);
    screenCaptureTimer->start();
    connect(screenCaptureTimer, &QTimer::timeout, this, [=](){
        if(screenCapturing == false)
        {
            screenCaptureTimer -> stop();
            return;
        }
        else saveImage();
    });
    connect(resultMenu, &ResultMenu::startVideo, this, &GameScene::showVideo);

    minimap = new Minimap(character, characterNum, m_map.maxCellx, m_map.maxCelly, this);
    minimap->setGeometry(64,64,m_map.maxCellx*10,m_map.maxCelly*10);
    minimap->show();


    playingMenu = new PlayingMenu(this);
    playingMenu->hide();
    connect(playingMenu, &PlayingMenu::exitGame, this, [=](){videoShower->stop();bgm->stop();emit exit();});
    connect(playingMenu, &PlayingMenu::restartGame, this, [=](){videoShower->stop();bgm->stop();emit restart();});

}
GameScene::~GameScene()
{
}
// 创建人物函数
void GameScene::createCharacter()
{
    // 人物个数：8
    characterNum = 8;
    // 临时记录8个人物的格子位置
    int tempx[8], tempy[8];

    // 循环随机生成位置直至位置合法
    for(int i = 0; i < 8; i++)
    {
        while(true)
        {
            bool success = true;
            // 随机生成位置
            tempx[i] = rand() % m_map.maxCellx + 1;
            tempy[i] = rand() % m_map.maxCelly + 1;
            // 生成的在阻挡地形上则不成功
            if(GameMap::binMap[tempx[i]][tempy[i]] == 0)
                success = false;
            // 生成的与前面重复则不成功
            for(int j = 0; j < i; j++)
            {
                if(tempx[j]==tempx[i]&&tempy[j]==tempy[i])
                    success = false;
            }
            // 成功则退出循环
            if(success)break;
        }
    }
    // 为红方设置人物
    character[0] = new Warrior(tempx[0], tempy[0], m_x, m_y,YOURS, this);
    character[1] = new Warrior(tempx[1], tempy[1], m_x, m_y,YOURS, this);
    character[2] = new Swordsman(tempx[2], tempy[2], m_x, m_y,YOURS, this);
    character[3] = new Ninja(tempx[3], tempy[3], m_x, m_y,YOURS, this);

    // 为蓝方设置人物
    character[4] = new Warrior(tempx[4], tempy[4], m_x, m_y, MINE, this);
    character[5] = new Warrior(tempx[5], tempy[5], m_x, m_y, MINE, this);
    character[6] = new Swordsman(tempx[6], tempy[6], m_x, m_y, MINE, this);
    character[7] = new Ninja(tempx[7], tempy[7], m_x, m_y, MINE, this);
}
// 播放截取的图片
void GameScene::showVideo()
{
    // 保存最后一帧图像
    saveImage();
    // 停止截取图片循环
    screenCapturing = false;
    screenCaptureTimer->stop();
    // 设置画布
    videoLabel->setFixedSize(900,600);
    videoLabel->setGeometry(350,0,900,600);
    videoLabel->raise();
    // 设置放映循环
    videoShower->setInterval(500);
    videoShower->start();
    imageCnt = 0;
    // 隐藏原有菜单
    resultMenu->button[0]->hide();
    resultMenu->button[1]->hide();
    resultMenu->button[2]->hide();
    resultMenu->resultLabel->hide();
    // 放映循环，播放下一张
    connect(videoShower, &QTimer::timeout,this,[=](){
        videoLabel->setPixmap(QPixmap::fromImage(imageSaver[imageCnt]));
        videoLabel->show();
        videoLabel->raise();
        imageCnt++;
        // 如果播放到最后一张，则退出，并显示原有菜单
        if(ull(imageCnt) == imageSaver.size())
        {
            videoShower->stop();
            resultMenu->button[1]->show();
            resultMenu->button[2]->show();
        }
    });
}
// 截取图片的函数，调用一次截取一张
void GameScene::saveImage()
{
    // 如果状态并非截取状态则跳过
    if(!screenCapturing)return;
    // 截取图片
    QScreen *screen = QApplication::primaryScreen();
    QPixmap pix = screen->grabWindow(this->winId());
    // 保存图片
    QImage image = pix.toImage().convertToFormat(QImage::Format_RGBA8888);
    image = image.scaled(900, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageSaver.push_back(image);
}
// 设置事件过滤器，防止人物将主界面挡住无法获取鼠标坐标
bool GameScene::eventFilter(QObject * watched, QEvent *event)
{
    // 如果被观察的是Character类
    if (watched->metaObject()->className() == QStringLiteral("Character"))
    {
        Character * now = qobject_cast<Character*>(watched); 
        // 如果鼠标触发进入人物事件，则根据人物坐标更新当前鼠标坐标
        if(event->type() == QEvent::Enter)
        {
            mouseCellx = now->m_cellx;
            mouseCelly = now->m_celly;
            repaint();
            return false;
        }
    }
    return false;
}
// 设置屏幕右下角四个按钮
void GameScene::setButton()
{
    int GAME_BUTTON_WIDTH = 96;
    int GAME_BUTTON_HEIGHT = 64;
    // 取消按钮
    cancelButton = new ClickLabel(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT,QPixmap(":/pic/cancel_button.png").scaled(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation),this);
    cancelButton->setGeometry(1060,830,GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT);
    cancelButton->setStyleSheet("border:none;");
    cancelButton->hide();

    // 跳过按钮
    skipButton = new ClickLabel(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT,QPixmap(":/pic/skip_button.png").scaled(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation),this);
    skipButton->setGeometry(1190,830,GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT);
    skipButton->setStyleSheet("border:none;");
    skipButton->raise();

    // 音乐按钮
    musicButton = new ClickLabel(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT,QPixmap(":/pic/music_button_off.png").scaled(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation),this);
    musicButton->setGeometry(1320,830,GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT);
    musicButton->setStyleSheet("border:none;");
    musicButton->raise();

    // 菜单按钮
    menuButton = new ClickLabel(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT,QPixmap(":/pic/menu_button.png").scaled(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation),this);
    menuButton->setGeometry(1450,830,GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT);
    menuButton->setStyleSheet("border:none;");
    menuButton->raise();

    // 如果触发取消按钮，则取消寻路/攻击，展开人物操作栏
    connect(cancelButton, &ClickLabel::clicked, this, [=](){
       zoom(cancelButton);
       clickSound->play();
       if(nowCharacter->characterState == Character::DEAD)return;
       nowCharacter->characterState = Character::BEGIN;
       nowCharacter->selectionDlg->show();
       nowCharacter->selectionDlg->raise();
       gameState = BEGIN;
       cancelButton->hide();
       repaint();
    });
    // 如果触发跳过按钮，则开启下一轮
    connect(skipButton, &ClickLabel::clicked, this, [=](){
        zoom(skipButton);
        clickSound->play();
        cancelButton->hide();
        if(nowCharacter)nowCharacter->selectionDlg->hide();
        nextRound(roundBelonged);
    });
    // 如果触发音乐按钮，则判断当前音乐状态并切换图片和音乐状态
    connect(musicButton, &ClickLabel::clicked, this, [=](){
        zoom(musicButton);
        clickSound->play();
        if(bgm->isFinished()==true)
        {
            musicButton->setPixmap(QPixmap(":/pic/music_button_on.png").scaled(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            musicButton->repaint();
            bgm->play();
        }
        else
        {
            musicButton->setPixmap(QPixmap(":/pic/music_button_off.png").scaled(GAME_BUTTON_WIDTH,GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            musicButton->repaint();
            bgm->stop();
        }
    });
    // 如果触发菜单按键，则展开菜单
    connect(menuButton, &ClickLabel::clicked, this, [=](){
        zoom(menuButton);
        clickSound->play();
        playingMenu->show();
        playingMenu->raise();
    });
}
void GameScene::redrawCharacter()
{
    // 重画所有人物
    for(int i=0;i<characterNum;i++)
    {
        // 如果人物已死则不画
        if(character[i]->characterState==Character::DEAD)continue;
        character[i]->show();
        character[i]->setGeometry((character[i]->m_localCellx - 1) * CELL_SIZE,
                          (character[i]->m_localCelly - 1) * CELL_SIZE, 64, 64);
    }
}
void GameScene::nextRound(int last)
{
    if(gameState == END)return;
    // 改变游戏基本状态
    int next = last ^ 1;
    roundBelonged = next;
    gameState = BEGIN;

    for(int i = 0; i < characterNum; i++)
    {
        // 人物已死则不操作
        if(character[i]->characterState==Character::DEAD)continue;
        // 更新下一回合character，满血、满行动力
        if(character[i]->m_belong==next)
        {
            character[i]->characterState = Character::BEGIN;
            character[i]->attrackedOrNot = false;
            character[i]->m_move = character[i]->m_fullmove;
        }
        // 更新上一回合character，如果处在损伤地形则减血
        else
        {
            character[i]->selectionDlg->hide();
            if(GameMap::binMap[character[i]->m_cellx][character[i]->m_celly]==2)
            {
                emit character[i]->beAttracked(10);
            }
        }
    }
    roundNum[next] = aliveNum[next];
    // 如果没开ai则提示红方/蓝方
    if(AIOpenOrNot==false)
    {
        if(last==MINE)receiveHint("红方回合");
        else receiveHint("蓝方回合");
    }
    // 如果开了ai则提示对方/己方
    else
    {
        if(last==MINE)receiveHint("对方回合");
        else receiveHint("我方回合");
    }
    // 如果开了ai且下一轮为ai，则调用函数
    if(AIOpenOrNot == true && next == YOURS)
    {
        gameState = AI;
        AIRound();
    }
    repaint();
}
void GameScene::AIRound()
{
    // 调用aicontroller以完成ai回合
    aiController->reset(aliveNum[YOURS]);
    aiController->start();
}
// 人物将要移动
void GameScene::characterMoveEvent(Character* t_nowCharacter)
{
    // 改变人物状态
    gameState=FINDPATH;
    cancelButton->show();
    nowCharacter=t_nowCharacter;
    int tempx = nowCharacter->m_cellx;
    int tempy = nowCharacter->m_celly;
    // 让Algorithm为人物寻路
    moveAl.init(nowCharacter->m_move, nowCharacter);
    moveAl.findAvailableCell(tempx, tempy, 0,character,characterNum);
    // 如果四周都没有路，则给出提示
    if(moveAl.resultMap[tempx + 1][tempy]==-1&&moveAl.resultMap[tempx - 1][tempy]==-1
            &&moveAl.resultMap[tempx][tempy + 1]==-1&&moveAl.resultMap[tempx][tempy-1]==-1)
    {
        receiveHint("没有可移至的地块");
        emit cancelButton->clicked();
    }
    repaint();
}
void GameScene::characterAttrackEvent(Character* t_nowCharacter)
{
    // 改变人物状态
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
        }
    }
    repaint();
    // 如果没有可攻击的对象
    if(findOneOrNot == false)
    {
        receiveHint("没有可攻击的对象");
        QTimer::singleShot(500,this,[=](){emit cancelButton->clicked();});
    }
}
// 跳过一个人后触发
void GameScene::endOneCharacterEvent(Character* endedCharacter)
{
    int id=-1;
    // 找到跳过的人id
    for(int i=0;i<characterNum;i++)
    {
        if(character[i]==endedCharacter)
        {
            id=i;
            break;
        }
    }
    // 当前回合人物数改变，如果为0则下一回合
    roundNum[character[id]->m_belong]--;
    cancelButton->hide();
    if(roundNum[character[id]->m_belong]==0)
        nextRound(character[id]->m_belong);
}
// 死人后触发
void GameScene::dieOneCharacterEvent(Character* deadCharacter)
{
    int id=-1;
    // 找到死人id
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
        if(character[i]->m_belong == deadCharacter->m_belong&&id!=i&&character[i]->characterState!=Character::DEAD)
            alive++;
    aliveNum[character[id]->m_belong] = alive;
    // 如果人死完了则判定游戏结果
    if(aliveNum[character[id]->m_belong]==0)
    {
        hint->hide();
        gameState = END;
        if(character[id]->m_belong==MINE)emit myLoss();
        else emit myWin();
        return;
    }
    repaint();
}
void GameScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // 画背景图
    painter.drawPixmap(m_x, m_y, m_map.m_pic);
    // 如果游戏结束就不画了
    if(gameState == END)return;

    // 将右下角四个按钮置顶
    if(skipButton->isHidden()==false)
        skipButton->raise();
    if(cancelButton->isHidden()==false)
        cancelButton->raise();
    menuButton ->raise();
    musicButton ->raise();



    // 画右上角回合提示
    if(roundBelonged==MINE)
        painter.setBrush(Qt::blue);
    else painter.setBrush(Qt::red);
    painter.drawRect(1500, 100, 50, 50);
    painter.setBrush(Qt::NoBrush);
    if(roundBelonged==MINE) // 如果是蓝方
    {
        painter.setPen(Qt::blue);
        QFont font("隶书",25,60,true);
        painter.setFont(font);
        if(AIOpenOrNot == false)
            painter.drawText(1290, 120, "蓝方回合");
        else
            painter.drawText(1290,120, "我方回合");
        painter.setFont(QFont("宋体",15,40,false));
        painter.drawText(1295, 160, QString("可行棋子：%1/%2").arg(roundNum[roundBelonged]).arg(aliveNum[roundBelonged]));
    }
    else // 如果是红方
    {
        painter.setPen(Qt::red);
        painter.setFont(QFont("隶书",25,60,true));
        if(AIOpenOrNot == false)
            painter.drawText(1290,120, "红方回合");
        else
            painter.drawText(1290,120, "对方回合");
        painter.setFont(QFont("宋体",15,40,false));
        if(AIOpenOrNot == false)
            painter.drawText(1295, 160, QString("可行棋子：%1/%2").arg(roundNum[roundBelonged]).arg(aliveNum[roundBelonged]));
    }
    painter.setPen(Qt::NoPen);

    // 当前地块用方框标白
    painter.setPen(QPen(Qt::white,4));
    painter.drawRect((mouseCellx + m_x/CELL_SIZE - 1)*CELL_SIZE,
                     (mouseCelly + m_y/CELL_SIZE - 1)*CELL_SIZE,
                     CELL_SIZE, CELL_SIZE);
    painter.setPen(Qt::NoPen);

    // 如果游戏状态是寻路状态，则标蓝可行域
    if(gameState == FINDPATH)
    {
        for(int i=1;i<=m_map.maxCellx;i++)
            for(int j=1;j<=m_map.maxCelly;j++)
                if(moveAl.resultMap[i][j]!=-1)
                {
                    painter.setPen(QPen(QColor(0,150,255),2,Qt::DotLine));
                    painter.setBrush(QBrush(QColor(0,20,255,180)));
                    painter.drawRect((i + m_x/CELL_SIZE - 1)*CELL_SIZE,
                                     (j + m_y/CELL_SIZE - 1)*CELL_SIZE,
                                     CELL_SIZE, CELL_SIZE);
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(Qt::NoBrush);
                }
    }

    // 如果游戏状态是寻找攻击对象状态，则标出攻击范围和可攻击对象
    else if(gameState==FINDATTRACK)
    {
        // 寻找可攻击对象
        for(ull i = 0; i < attrackAl.v.size(); i++)
        {
            painter.setPen(QPen(Qt::red,3));
            painter.setBrush(QBrush(QColor(255,20,0,180)));
            painter.drawRect((attrackAl.v[i].first + m_x/CELL_SIZE - 1)*CELL_SIZE,
                             (attrackAl.v[i].second + m_y/CELL_SIZE - 1)*CELL_SIZE,
                             CELL_SIZE, CELL_SIZE);
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::NoBrush);
        }
        // 画可攻击范围
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
    // 如果是右击则相当于取消按钮
    if(event->button() == Qt::RightButton)
    {
        if(cancelButton->isHidden()==false)
            emit cancelButton->clicked();
        return;
    }
    // 更新当前鼠标位置
    updateMousePosition(event);

    // 如果是ai回合则不用等待进一步处理点击
    if(AIOpenOrNot==true&&gameState==AI)return;

    // 如果是选择人物阶段
    if(gameState==BEGIN)
    {
        //选择对话框
        for(int i = 0; i <characterNum; i++)
        {
            // 人物并非被选择阶段则跳过
            if(character[i]->characterState==Character::DEAD)continue;
            // 敌军跳过
            if(character[i]->m_belong!=roundBelonged)continue;
            else
            {   // 点错收回选择框
                if(character[i]->selectionDlg->isHidden()==false)
                    character[i]->selectionDlg->hide();
            }
            // 点中了人物则展示操作框
            if(mouseLocalCellx==character[i]->m_localCellx&&
                    mouseLocalCelly==character[i]->m_localCelly)
            {
                // 如果跳过了或/////////////////////////////////////////////////////
                if(character[i]->characterState==Character::END)
                {
                    receiveHint("本回合已跳过该角色");
                    qDebug()<<"123";
                    continue;
                }
                else if(character[i]->characterState==Character::DEAD)continue;

                clickSound->play();
                //显示选择框
                if(character[i]->selectionDlg->isHidden())
                {
                    character[i]->selectionDlg->show();
                    character[i]->selectionDlg->updateData(mouseLocalCellx, mouseLocalCelly);
                    character[i]->selectionDlg->raise();
                }
                break;
            }
            else
            {   //点错收回选择框
                if(character[i]->selectionDlg->isHidden()==false)
                    character[i]->selectionDlg->hide();
            }
        }
    }
    // 如果是寻路阶段
    else if(gameState==FINDPATH)
    {
        // 如果点中可行域，则执行人物移动
        if(moveAl.resultMap[mouseCellx][mouseCelly]!=-1)
        {
            clickSound->play();
            moveAl.findPath(nowCharacter->m_cellx, nowCharacter->m_celly, mouseCellx, mouseCelly, 0, moveAl.resultMap[mouseCellx][mouseCelly]);
            nowCharacter->movePos(moveAl.resultMap[mouseCellx][mouseCelly],moveAl.path);
            cancelButton->hide();
            gameState=BEGIN;
            if(nowCharacter->characterState!=Character::DEAD)
                nowCharacter->characterState=BEGIN;
            repaint();
        }
        else return;
    }

    // 如果是寻找攻击目标阶段
    else if(gameState==FINDATTRACK)
    {
        //在攻击范围内
        if(attrackAl.resultMap[mouseCellx][mouseCelly]!=-1)
        {
            for(int i=0;i<characterNum;i++)
            {
                if(character[i]->m_cellx==mouseCellx&&character[i]->m_celly==mouseCelly)//点中
                {
                    if(character[i]->m_belong==roundBelonged ||//友军
                        character[i]->characterState == Character::DEAD)//死了
                    {
                        clickSound->play();
                        gameState=BEGIN;
                        nowCharacter->characterState=BEGIN;
                        nowCharacter->attrackedOrNot=false;
                        break;
                    }
                    cancelButton->hide();
                    nowCharacter->attracker->startMove(nowCharacter, nowCharacter->m_localCellx, nowCharacter->m_localCelly,
                                             character[i]->m_localCellx, character[i]->m_localCelly);
                    // 等待攻击完毕，发动结算机制
                    connect(nowCharacter->attracker, &AttrackAnimation::animationFinished,this,[=](){
                        emit character[i]->beAttracked(nowCharacter->m_attrack);
                        disconnect(nowCharacter->attracker,0,this,0); //仅结算一次，因此要断连
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
// 调用则显示提示框
void GameScene::receiveHint(QString str)
{
    hint->setText(str);
    hint->show();
    hint->raise();
    QTimer::singleShot(1500,this,[=](){hint->hide();});
}
// 如果鼠标移动则更新鼠标坐标
void GameScene::mouseMoveEvent(QMouseEvent* event)
{
    updateMousePosition(event);
    checkScreenMove();
    update();
}
// 更新鼠标坐标
void GameScene::updateMousePosition(QMouseEvent* event)
{
    QPointF point = event->localPos();
    // 绝对坐标
    mousex = point.x();
    mousey = point.y();
    // 绝对格子坐标
    mouseCellx = (-m_x + mousex) / CELL_SIZE + 1;
    mouseCelly = (-m_y + mousey) / CELL_SIZE + 1;
    // 相对格子坐标
    mouseLocalCellx = (mousex) / CELL_SIZE + 1;
    mouseLocalCelly = (mousey) / CELL_SIZE + 1;
}
#define MAP_HEIGHT m_map.height
#define MAP_WIDTH m_map.width
// 如果鼠标移至屏幕边界处则滚动屏幕，反之不滚动
void GameScene::checkScreenMove()
{
    // 向右滚
    if(mousex >= WINDOW_WIDTH - WINDOW_BOUNDARY
            && -m_x <= MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE)
        emit moveRight();
    // 停止向右
    if(-m_x > MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE
            || mousex < WINDOW_WIDTH - WINDOW_BOUNDARY)
        emit notMoveRight();

    // 向左滚
    if(mousex <= WINDOW_BOUNDARY && m_x <= 0)
        emit moveLeft();
    // 停止向左滚
    if(m_x > 0 || mousex > WINDOW_BOUNDARY)
        emit notMoveLeft();

    // 向下滚
    if(mousey >= WINDOW_HEIGHT - WINDOW_BOUNDARY
            && -m_y <= MAP_HEIGHT - WINDOW_HEIGHT - CELL_SIZE)
        emit moveDown();
    // 停止向下滚
    if(-m_y > MAP_HEIGHT - WINDOW_HEIGHT - CELL_SIZE
            || mousey < WINDOW_HEIGHT - WINDOW_BOUNDARY)
        emit notMoveDown();

    // 向上滚
    if(mousey <= WINDOW_BOUNDARY && m_y <= 0)
        emit moveUp();
    // 停止向上滚
    if(m_y > 0 || mousey > WINDOW_BOUNDARY)
        emit notMoveUp();
}
// 设置滚动界面定时器
void GameScene::setScreenMoveTimer()
{
    // 向右滚
    QTimer *rtimer = new QTimer(this);
    rtimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    // 将向右滚动定时器连接向右/停止向右
    connect(this, SIGNAL(moveRight()), rtimer, SLOT(start()));
    connect(this, SIGNAL(notMoveRight()),rtimer, SLOT(stop()));
    connect(rtimer, &QTimer::timeout, this,[this]()
    {
        if(screenMoveOrNot == false)return;
        if(-m_x >= MAP_WIDTH - WINDOW_WIDTH)
        {
            emit notMoveRight();
            return ;
        }
        // 更新人物坐标
        for(int i = 0; i < characterNum; i++)
        {
            character[i]->m_localCellx--;
        }
        m_x -= CELL_SIZE;
        // 发送屏幕滚动信号
        emit moveScreen();
    });

    // 向左滚
    QTimer *ltimer = new QTimer(this);
    ltimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveLeft()), ltimer, SLOT(start()));
    connect(this, SIGNAL(notMoveLeft()),ltimer, SLOT(stop()));
    connect(ltimer, &QTimer::timeout, this,[this]()
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

    // 向下滚
    QTimer *dtimer = new QTimer(this);
    dtimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveDown()), dtimer, SLOT(start()));
    connect(this, SIGNAL(notMoveDown()),dtimer, SLOT(stop()));
    connect(dtimer, &QTimer::timeout, this,[this]()
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

    // 向上滚
    QTimer *utimer = new QTimer(this);
    utimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveUp()), utimer, SLOT(start()));
    connect(this, SIGNAL(notMoveUp()),utimer, SLOT(stop()));
    connect(utimer, &QTimer::timeout, this,[this]()
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
