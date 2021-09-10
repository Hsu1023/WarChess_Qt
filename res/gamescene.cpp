#include "gamescene.h"
#include "characterrole.h"
#include "gamelabel.h"
#include "algorithm.h"
#include <QApplication>
#include <QPixmap>
#include <QScreen>

/*  GameScene是关于游戏页面的类，包含游戏运行的所有内容，可以视为游戏的运行引擎 */

std::vector<QImage> GameScene::m_imageSaver{};

GameScene::GameScene(const int chapter, const int gameMode, QWidget* parent)
    : QDialog(parent), m_map(GameMap(chapter))
{

    m_x = 0; m_y = 0;
    m_gameState = BEGIN;

    // 设置屏幕格式
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    // 战斗复盘相关
    m_videoShower = new QTimer(this);
    m_videoLabel = new QLabel(this);

    // 设置音乐和音效
    m_bgm = new QSound(BGM_PATH, this);
    m_bgm->setLoops(QSound::Infinite);
    m_attrackSound = new QSound(ATTRACK_MUSIC_PATH, this);
    m_clickSound = new QSound(CLICK_MUSIC_PATH, this);

    // 设置右下角四个按钮
    setButton();

    // 设置消息提示框
    m_hint = new HintLabel(this);
    m_hint->hide();

    // 设置滚动屏幕相关内容
    setScreenMoveTimer();
    m_screenMoveOrNot = true;
    connect(this, &GameScene::moveScreen, [=]() {
        for (int i = 0; i < m_characterNum; i++)
        {
            redrawCharacter();
            if (m_character[i]->m_characterState != Character::DEAD)
                m_character[i]->updateInfo();
        }
        repaint();
        });

    // 角色创建相关内容
    createCharacter();
    m_aliveNum[MINE] = m_aliveNum[YOURS] = 0;
    for (int i = 0; i < m_characterNum; i++)
        m_aliveNum[m_character[i]->m_belong]++;
    m_roundBelonged = MINE;
    receiveHint("蓝方回合");
    m_roundNum[MINE] = m_aliveNum[MINE];
    m_roundNum[YOURS] = m_aliveNum[YOURS];

    m_nowCharacter = nullptr;
    // 与角色相关的信号与槽
    for (int i = 0; i < m_characterNum; i++)
    {

        m_character[i]->installEventFilter(this);
        // 重绘屏幕
        connect(m_character[i], &Character::repaintScreen, this, [=]() {repaint(); });
        // 触发人物移动事件
        connect(m_character[i], &Character::characterMoveAction, this, &GameScene::characterMoveEvent);
        // 触发人物攻击事件
        connect(m_character[i], &Character::characterAttrackAction, this, &GameScene::characterAttrackEvent);
        // 触发跳过一个人事件
        connect(m_character[i], &Character::endOneCharacter, this, &GameScene::endOneCharacterEvent);
        // 触发死去一个人事件
        connect(m_character[i], &Character::dieOneCharacter, this, &GameScene::dieOneCharacterEvent);
        // 触发显示提示框事件
        connect(m_character[i], &Character::submitHint, this, &GameScene::receiveHint);
        // 触发隐藏取消键事件
        connect(m_character[i], &Character::hideCancelButton, this, [=]() {emit m_cancelButton->clicked(); });
        // 动画开始触发停止屏幕移动事件
        connect(m_character[i]->m_mover, &MoveAnimation::animationStarted, this, [=]() {m_screenMoveOrNot = false; });
        // 动画结束触发允许屏幕移动事件
        connect(m_character[i]->m_mover, &MoveAnimation::animationFinished, this, [=]()
            {
                m_screenMoveOrNot = true;
                if (m_AIOpenOrNot == false || m_character[i]->m_belong == MINE)
                    m_character[i]->m_selectionDlg->show();
            });
        // 人物移动一格触发小地图重绘事件
        connect(m_character[i]->m_mover, &MoveAnimation::moveOneCellFinished, this, [=]()
            {
                m_minimap->repaint();
            });
        // 攻击触发停止屏幕移动和添加攻击音效事件
        connect(m_character[i]->m_attracker, &AttrackAnimation::animationStarted, this, [=]()
            {
                m_screenMoveOrNot = false;
                m_attrackSound->play();
            });
        // 攻击完成触发允许屏幕移动事件
        connect(m_character[i]->m_attracker, &AttrackAnimation::animationFinished, this, [=]()
            {
                m_screenMoveOrNot = true;
                if (m_AIOpenOrNot == false || m_character[i]->m_belong == MINE)
                    m_character[i]->m_selectionDlg->show();
            });
    }

    // 与AI有关的初始化
    m_AIOpenOrNot = gameMode;
    m_aiController = new AIController(m_character, m_characterNum, this);
    connect(m_aiController, &AIController::AIRoundBegin, this, [=]() {m_skipButton->hide(); });
    connect(m_aiController, &AIController::AIRoundFinished, this, [=]() {m_skipButton->show(); nextRound(YOURS); });

    // 与显示游戏结果有关的初始化
    m_resultMenu = new ResultMenu(this);
    m_resultMenu->hide();
    connect(m_resultMenu, &ResultMenu::exitGame, this, [=]() {m_videoShower->stop(); m_bgm->stop(); emit exit(); });
    connect(m_resultMenu, &ResultMenu::restartGame, this, [=]() {m_videoShower->stop(); m_bgm->stop(); emit restart(); });
    connect(this, &GameScene::myWin, [=]() {
        m_hint->close();
        m_skipButton->close();
        m_menuButton->close();
        m_musicButton->close();
        m_resultMenu->setResult(1, m_AIOpenOrNot);
        m_resultMenu->show();
        m_resultMenu->raise();
        });
    connect(this, &GameScene::myLoss, [=]() {
        m_hint->close();
        m_skipButton->close();
        m_menuButton->close();
        m_musicButton->close();
        m_resultMenu->setResult(0, m_AIOpenOrNot);
        QTimer::singleShot(500, this, [=]() {

            m_resultMenu->show();
            m_resultMenu->raise();
            });
        });


    // 与录制屏幕有关的初始化
    m_screenCapturing = true;
    m_imageSaver.clear();
    m_screenCaptureTimer = new QTimer(this);
    m_screenCaptureTimer->setInterval(CAPTURE_SCREEN_INTERVAL);
    m_screenCaptureTimer->start();
    connect(m_screenCaptureTimer, &QTimer::timeout, this, [=]() {
        if (m_screenCapturing == false) // 状态为停止录制，则停止定时器
        {
            m_screenCaptureTimer->stop();
            return;
        }
        else saveImage(); // 否则调用函数截一张图
        });
    // 结束界面开始放映键与放映动画相连
    connect(m_resultMenu, &ResultMenu::startVideo, this, &GameScene::showVideo);

    // 与小地图有关的初始化
    m_minimap = new Minimap(m_character, m_characterNum, m_map.m_maxCellx, m_map.m_maxCelly, this);
    m_minimap->setGeometry(64, 64, m_map.m_maxCellx * 10, m_map.m_maxCelly * 10);
    m_minimap->show();

    // 与游戏时菜单按钮有关的初始化
    m_playingMenu = new PlayingMenu(this);
    m_playingMenu->hide();
    connect(m_playingMenu, &PlayingMenu::exitGame, this, [=]() {m_videoShower->stop(); m_bgm->stop(); emit exit(); });
    connect(m_playingMenu, &PlayingMenu::restartGame, this, [=]() {m_videoShower->stop(); m_bgm->stop(); emit restart(); });


    // 初始时重绘人物
    for (int i = 0; i < m_characterNum; i++)
    {
        redrawCharacter();
        if (m_character[i]->m_characterState != Character::DEAD)
            m_character[i]->updateInfo();
    }
}
GameScene::~GameScene()
{
}
// 创建人物函数
void GameScene::createCharacter()
{
    // 人物个数：8
    m_characterNum = 8;
    // 临时记录8个人物的格子位置
    int tempx[8], tempy[8];

    // 循环随机生成位置直至位置合法
    for (int i = 0; i < 8; i++)
    {
        while (true)
        {
            bool success = true;
            // 随机生成位置
            tempx[i] = rand() % m_map.m_maxCellx + 1;
            tempy[i] = rand() % m_map.m_maxCelly + 1;
            // 生成的在阻挡地形上则不成功
            if (GameMap::binMap[tempx[i]][tempy[i]] == 0)
                success = false;
            // 生成的与前面重复则不成功
            for (int j = 0; j < i; j++)
            {
                if (tempx[j] == tempx[i] && tempy[j] == tempy[i])
                    success = false;
            }
            // 成功则退出循环
            if (success)break;
        }
    }
    // 为红方设置人物
    m_character[0] = new Warrior(tempx[0], tempy[0], m_x, m_y, YOURS, this);
    m_character[1] = new Warrior(tempx[1], tempy[1], m_x, m_y, YOURS, this);
    m_character[2] = new Swordsman(tempx[2], tempy[2], m_x, m_y, YOURS, this);
    m_character[3] = new Ninja(tempx[3], tempy[3], m_x, m_y, YOURS, this);

    // 为蓝方设置人物
    m_character[4] = new Warrior(tempx[4], tempy[4], m_x, m_y, MINE, this);
    m_character[5] = new Warrior(tempx[5], tempy[5], m_x, m_y, MINE, this);
    m_character[6] = new Swordsman(tempx[6], tempy[6], m_x, m_y, MINE, this);
    m_character[7] = new Ninja(tempx[7], tempy[7], m_x, m_y, MINE, this);
}
// 播放截取的图片
void GameScene::showVideo()
{
    // 保存最后一帧图像
    saveImage();
    // 停止截取图片循环
    m_screenCapturing = false;
    m_screenCaptureTimer->stop();
    // 设置画布
    m_videoLabel->setFixedSize(900, 600);
    m_videoLabel->setGeometry(350, 0, 900, 600);
    m_videoLabel->raise();
    // 设置放映循环
    m_videoShower->setInterval(500);
    m_videoShower->start();
    m_imageCnt = 0;
    // 隐藏原有菜单
    m_resultMenu->m_button[0]->hide();
    m_resultMenu->m_button[1]->hide();
    m_resultMenu->m_button[2]->hide();
    m_resultMenu->m_resultLabel->hide();
    // 放映循环，播放下一张
    connect(m_videoShower, &QTimer::timeout, this, [=]() {
        m_videoLabel->setPixmap(QPixmap::fromImage(m_imageSaver[m_imageCnt]));
        m_videoLabel->show();
        m_videoLabel->raise();
        m_imageCnt++;
        // 如果播放到最后一张，则退出，并显示原有菜单
        if (ull(m_imageCnt) == m_imageSaver.size())
        {
            m_videoShower->stop();
            m_resultMenu->m_button[1]->show();
            m_resultMenu->m_button[2]->show();
        }
        });
}
// 截取图片的函数，调用一次截取一张
void GameScene::saveImage() const
{
    // 如果状态并非截取状态则跳过
    if (!m_screenCapturing)return;
    // 截取图片
    QScreen* screen = QApplication::primaryScreen();
    QPixmap pix = screen->grabWindow(this->winId());
    // 保存图片
    QImage image = pix.toImage().convertToFormat(QImage::Format_RGBA8888);
    image = image.scaled(900, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_imageSaver.push_back(image);
}
// 设置事件过滤器，防止人物将主界面挡住无法获取鼠标坐标
bool GameScene::eventFilter(QObject* watched, QEvent* event)
{
    // 如果被观察的是Character类
    if (watched->metaObject()->className() == QStringLiteral("Character"))
    {
        Character* now = qobject_cast<Character*>(watched);
        // 如果鼠标触发进入人物事件，则根据人物坐标更新当前鼠标坐标
        if (event->type() == QEvent::Enter)
        {
            m_mouseCellx = now->m_cellx;
            m_mouseCelly = now->m_celly;
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
    m_cancelButton = new ClickLabel(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, QPixmap(":/pic/cancel_button.png").scaled(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation), this);
    m_cancelButton->setGeometry(1060, 830, GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT);
    m_cancelButton->setStyleSheet("border:none;");
    m_cancelButton->hide();

    // 跳过按钮
    m_skipButton = new ClickLabel(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, QPixmap(":/pic/skip_button.png").scaled(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation), this);
    m_skipButton->setGeometry(1190, 830, GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT);
    m_skipButton->setStyleSheet("border:none;");
    m_skipButton->raise();

    // 音乐按钮
    m_musicButton = new ClickLabel(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, QPixmap(":/pic/music_button_off.png").scaled(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation), this);
    m_musicButton->setGeometry(1320, 830, GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT);
    m_musicButton->setStyleSheet("border:none;");
    m_musicButton->raise();

    // 菜单按钮
    m_menuButton = new ClickLabel(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, QPixmap(":/pic/menu_button.png").scaled(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation), this);
    m_menuButton->setGeometry(1450, 830, GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT);
    m_menuButton->setStyleSheet("border:none;");
    m_menuButton->raise();

    // 如果触发取消按钮，则取消寻路/攻击，展开人物操作栏
    connect(m_cancelButton, &ClickLabel::clicked, this, [=]() {
        zoom(m_cancelButton);
        m_clickSound->play();
        if (m_nowCharacter->m_characterState == Character::DEAD)return;
        m_nowCharacter->m_characterState = Character::BEGIN;
        m_nowCharacter->m_selectionDlg->show();
        m_nowCharacter->m_selectionDlg->raise();
        m_gameState = BEGIN;
        m_cancelButton->hide();
        repaint();
        });
    // 如果触发跳过按钮，则开启下一轮
    connect(m_skipButton, &ClickLabel::clicked, this, [=]() {
        zoom(m_skipButton);
        m_clickSound->play();
        m_cancelButton->hide();
        if (m_nowCharacter)m_nowCharacter->m_selectionDlg->hide();
        nextRound(m_roundBelonged);
        });
    // 如果触发音乐按钮，则判断当前音乐状态并切换图片和音乐状态
    connect(m_musicButton, &ClickLabel::clicked, this, [=]() {
        zoom(m_musicButton);
        m_clickSound->play();
        if (m_bgm->isFinished() == true)
        {
            m_musicButton->setPixmap(QPixmap(":/pic/music_button_on.png").scaled(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_musicButton->repaint();
            m_bgm->play();
        }
        else
        {
            m_musicButton->setPixmap(QPixmap(":/pic/music_button_off.png").scaled(GAME_BUTTON_WIDTH, GAME_BUTTON_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_musicButton->repaint();
            m_bgm->stop();
        }
        });
    // 如果触发菜单按键，则展开菜单
    connect(m_menuButton, &ClickLabel::clicked, this, [=]() {
        zoom(m_menuButton);
        m_playingMenu->show();
        m_playingMenu->raise();
        m_clickSound->play();
        });
}
void GameScene::redrawCharacter()const
{
    // 重画所有人物
    for (int i = 0; i < m_characterNum; i++)
    {
        // 如果人物已死则不画
        if (m_character[i]->m_characterState == Character::DEAD)continue;
        m_character[i]->show();
        m_character[i]->setGeometry((m_character[i]->m_localCellx - 1) * CELL_SIZE,
            (m_character[i]->m_localCelly - 1) * CELL_SIZE, 64, 64);
    }
}
void GameScene::nextRound(int last)
{
    if (m_gameState == END)return;
    // 改变游戏基本状态
    int next = last ^ 1;
    m_roundBelonged = next;
    m_gameState = BEGIN;

    for (int i = 0; i < m_characterNum; i++)
    {
        // 人物已死则不操作
        if (m_character[i]->m_characterState == Character::DEAD)continue;
        // 更新下一回合character，满血、满行动力
        if (m_character[i]->m_belong == next)
        {
            m_character[i]->m_characterState = Character::BEGIN;
            m_character[i]->m_attrackedOrNot = false;
            m_character[i]->m_move = m_character[i]->m_fullmove;
        }
        // 更新上一回合character，如果处在损伤地形则减血
        else
        {
            m_character[i]->m_selectionDlg->hide();
            if (GameMap::binMap[m_character[i]->m_cellx][m_character[i]->m_celly] == 2)
            {
                emit m_character[i]->beAttracked(10);
            }
        }
    }
    m_roundNum[next] = m_aliveNum[next];
    // 如果没开ai则提示红方/蓝方
    if (m_AIOpenOrNot == false)
    {
        if (last == MINE)receiveHint("红方回合");
        else receiveHint("蓝方回合");
    }
    // 如果开了ai则提示对方/己方
    else
    {
        if (last == MINE)receiveHint("对方回合");
        else receiveHint("我方回合");
    }
    // 如果开了ai且下一轮为ai，则调用函数
    if (m_AIOpenOrNot == true && next == YOURS)
    {
        m_gameState = AI;
        AIRound();
    }
    repaint();
}
// 调用aicontroller以完成ai回合
void GameScene::AIRound()const
{
    m_aiController->reset(m_aliveNum[YOURS]);
    m_aiController->start();
}
// 人物将要移动
void GameScene::characterMoveEvent(Character* t_nowCharacter)
{
    // 改变人物状态
    m_gameState = FINDPATH;
    m_cancelButton->show();
    m_nowCharacter = t_nowCharacter;
    int tempx = m_nowCharacter->m_cellx;
    int tempy = m_nowCharacter->m_celly;
    // 让Algorithm为人物寻路
    moveAl.init(m_nowCharacter->m_move, m_nowCharacter);
    moveAl.findAvailableCell(tempx, tempy, 0, m_character, m_characterNum);
    // 如果四周都没有路，则给出提示
    if (moveAl.m_resultMap[tempx + 1][tempy] == -1 && moveAl.m_resultMap[tempx - 1][tempy] == -1
        && moveAl.m_resultMap[tempx][tempy + 1] == -1 && moveAl.m_resultMap[tempx][tempy - 1] == -1)
    {
        receiveHint("没有可移至的地块");
        emit m_cancelButton->clicked();
    }
    repaint();
}
void GameScene::characterAttrackEvent(Character* t_nowCharacter)
{
    // 改变人物状态
    m_gameState = FINDATTRACK;
    m_cancelButton->show();
    m_nowCharacter = t_nowCharacter;
    attrackAl.init(m_nowCharacter->m_attrackable, m_nowCharacter);
    bool findOneOrNot = false;
    //寻找可攻击目标并标出
    for (int i = 0; i < m_characterNum; i++)
    {
        if (m_character[i]->m_belong == m_roundBelonged || m_character[i]->m_characterState == Character::DEAD)continue;//友军
        if (ManhattanDist(m_character[i]->m_cellx, m_character[i]->m_celly,
            m_nowCharacter->m_cellx, m_nowCharacter->m_celly) <= m_nowCharacter->m_attrackable)
        {
            attrackAl.m_resultMap[m_character[i]->m_cellx][m_character[i]->m_celly] = 1;
            attrackAl.m_availableCell.push_back(node(m_character[i]->m_cellx, m_character[i]->m_celly));
            findOneOrNot = true;
        }
    }
    repaint();
    // 如果没有可攻击的对象
    if (findOneOrNot == false)
    {
        receiveHint("没有可攻击的对象");
        QTimer::singleShot(500, this, [=]() {emit m_cancelButton->clicked(); });
    }
}
// 跳过一个人后触发
void GameScene::endOneCharacterEvent(Character* endedCharacter)
{
    int id = -1;
    // 找到跳过的人id
    for (int i = 0; i < m_characterNum; i++)
    {
        if (m_character[i] == endedCharacter)
        {
            id = i;
            break;
        }
    }
    // 当前回合人物数改变，如果为0则下一回合
    m_roundNum[m_character[id]->m_belong]--;
    m_cancelButton->hide();
    if (m_roundNum[m_character[id]->m_belong] == 0)
        nextRound(m_character[id]->m_belong);
}
// 死人后触发
void GameScene::dieOneCharacterEvent(Character* deadCharacter)
{
    int id = -1;
    // 找到死人id
    for (int i = 0; i < m_characterNum; i++)
    {
        if (m_character[i] == deadCharacter)
        {
            id = i;
            break;
        }
    }
    int alive = 0;
    for (int i = 0; i < m_characterNum; i++)
        if (m_character[i]->m_belong == deadCharacter->m_belong && id != i && m_character[i]->m_characterState != Character::DEAD)
            alive++;
    m_aliveNum[m_character[id]->m_belong] = alive;
    // 如果人死完了则判定游戏结果
    if (m_aliveNum[m_character[id]->m_belong] == 0)
    {
        m_hint->hide();
        m_gameState = END;
        if (m_character[id]->m_belong == MINE)emit myLoss();
        else emit myWin();
        return;
    }
    repaint();
}
void GameScene::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    // 画背景图
    painter.drawPixmap(m_x, m_y, m_map.m_pic);
    // 如果游戏结束就不画了
    if (m_gameState == END)return;

    // 将右下角四个按钮置顶
    if (m_skipButton->isHidden() == false)
        m_skipButton->raise();
    if (m_cancelButton->isHidden() == false)
        m_cancelButton->raise();
    m_menuButton->raise();
    m_musicButton->raise();



    // 画右上角回合提示
    if (m_roundBelonged == MINE)
        painter.setBrush(Qt::blue);
    else painter.setBrush(Qt::red);
    painter.drawRect(1500, 100, 50, 50);
    painter.setBrush(Qt::NoBrush);
    if (m_roundBelonged == MINE) // 如果是蓝方
    {
        painter.setPen(Qt::blue);
        QFont font("隶书", 25, 60, true);
        painter.setFont(font);
        if (m_AIOpenOrNot == false)
            painter.drawText(1290, 120, "蓝方回合");
        else
            painter.drawText(1290, 120, "我方回合");
        painter.setFont(QFont("宋体", 15, 40, false));
        painter.drawText(1295, 160, QString("可行棋子：%1/%2").arg(m_roundNum[m_roundBelonged]).arg(m_aliveNum[m_roundBelonged]));
    }
    else // 如果是红方
    {
        painter.setPen(Qt::red);
        painter.setFont(QFont("隶书", 25, 60, true));
        if (m_AIOpenOrNot == false)
            painter.drawText(1290, 120, "红方回合");
        else
            painter.drawText(1290, 120, "对方回合");
        painter.setFont(QFont("宋体", 15, 40, false));
        if (m_AIOpenOrNot == false)
            painter.drawText(1295, 160, QString("可行棋子：%1/%2").arg(m_roundNum[m_roundBelonged]).arg(m_aliveNum[m_roundBelonged]));
    }
    painter.setPen(Qt::NoPen);

    // 当前地块用方框标白
    painter.setPen(QPen(Qt::white, 4));
    painter.drawRect((m_mouseCellx + m_x / CELL_SIZE - 1) * CELL_SIZE,
        (m_mouseCelly + m_y / CELL_SIZE - 1) * CELL_SIZE,
        CELL_SIZE, CELL_SIZE);
    painter.setPen(Qt::NoPen);

    // 如果游戏状态是寻路状态，则标蓝可行域
    if (m_gameState == FINDPATH)
    {
        for (int i = 1; i <= m_map.m_maxCellx; i++)
            for (int j = 1; j <= m_map.m_maxCelly; j++)
                if (moveAl.m_resultMap[i][j] != -1)
                {
                    painter.setPen(QPen(QColor(0, 150, 255), 2, Qt::DotLine));
                    painter.setBrush(QBrush(QColor(0, 20, 255, 180)));
                    painter.drawRect((i + m_x / CELL_SIZE - 1) * CELL_SIZE,
                        (j + m_y / CELL_SIZE - 1) * CELL_SIZE,
                        CELL_SIZE, CELL_SIZE);
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(Qt::NoBrush);
                }
    }

    // 如果游戏状态是寻找攻击对象状态，则标出攻击范围和可攻击对象
    else if (m_gameState == FINDATTRACK)
    {
        // 寻找可攻击对象
        for (ull i = 0; i < attrackAl.m_availableCell.size(); i++)
        {
            painter.setPen(QPen(Qt::red, 3));
            painter.setBrush(QBrush(QColor(255, 20, 0, 180)));
            painter.drawRect((attrackAl.m_availableCell[i].first + m_x / CELL_SIZE - 1) * CELL_SIZE,
                (attrackAl.m_availableCell[i].second + m_y / CELL_SIZE - 1) * CELL_SIZE,
                CELL_SIZE, CELL_SIZE);
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::NoBrush);
        }
        // 画可攻击范围
        for (int i = m_nowCharacter->m_localCellx - m_nowCharacter->m_attrackable;
            i <= m_nowCharacter->m_localCellx + m_nowCharacter->m_attrackable; i++)
            for (int j = m_nowCharacter->m_localCelly - m_nowCharacter->m_attrackable;
                j <= m_nowCharacter->m_localCelly + m_nowCharacter->m_attrackable; j++)
        {
            if (i <= 1 || i >= 50)continue;
            if (j <= 1 || j >= 30)continue;
            if (ManhattanDist(i, j, m_nowCharacter->m_localCellx, m_nowCharacter->m_localCelly) > m_nowCharacter->m_attrackable)continue;
            painter.setPen(QPen(QColor(255, 150, 0), 3, Qt::DotLine));
            painter.drawRect((i - 1) * CELL_SIZE,
                (j - 1) * CELL_SIZE,
                CELL_SIZE, CELL_SIZE);
            painter.setPen(Qt::NoPen);
        }
    }
}
void GameScene::mousePressEvent(QMouseEvent* event)
{

    // 如果是右击则相当于取消按钮
    if (event->button() == Qt::RightButton)
    {
        if (m_cancelButton->isHidden() == false)
            emit m_cancelButton->clicked();
        return;
    }
    // 更新当前鼠标位置
    updateMousePosition(event);

    // 如果是ai回合则不用等待进一步处理点击
    if (m_AIOpenOrNot == true && m_gameState == AI)return;


    // 如果是选择人物阶段
    if (m_gameState == BEGIN)
    {
        //选择对话框
        for (int i = 0; i < m_characterNum; i++)
        {
            // 人物并非被选择阶段则跳过
            if (m_character[i]->m_characterState == Character::DEAD)continue;
            // 敌军跳过
            if (m_character[i]->m_belong != m_roundBelonged)continue;
            else
            {   // 点错收回选择框
                if (m_character[i]->m_selectionDlg->isHidden() == false)
                    m_character[i]->m_selectionDlg->hide();
            }
            // 点中了人物则展示操作框
            if (mouseLocalCellx == m_character[i]->m_localCellx &&
                mouseLocalCelly == m_character[i]->m_localCelly)
            {
                // 如果跳过了或/////////////////////////////////////////////////////
                if (m_character[i]->m_characterState == Character::END)
                {
                    receiveHint("本回合已跳过该角色");
                    qDebug() << "123";
                    continue;
                }
                else if (m_character[i]->m_characterState == Character::DEAD)continue;

                m_clickSound->play();
                //显示选择框
                if (m_character[i]->m_selectionDlg->isHidden())
                {
                    m_character[i]->m_selectionDlg->show();
                    m_character[i]->m_selectionDlg->updateData(mouseLocalCellx, mouseLocalCelly);
                    m_character[i]->m_selectionDlg->raise();
                }
                break;
            }
            else
            {   //点错收回选择框
                if (m_character[i]->m_selectionDlg->isHidden() == false)
                    m_character[i]->m_selectionDlg->hide();
            }
        }
    }
    // 如果是寻路阶段
    else if (m_gameState == FINDPATH)
    {
        // 如果点中可行域，则执行人物移动
        if (moveAl.m_resultMap[m_mouseCellx][m_mouseCelly] != -1)
        {
            m_clickSound->play();
            moveAl.findPath(m_nowCharacter->m_cellx, m_nowCharacter->m_celly, m_mouseCellx, m_mouseCelly, 0, moveAl.m_resultMap[m_mouseCellx][m_mouseCelly]);
            m_nowCharacter->movePos(moveAl.m_resultMap[m_mouseCellx][m_mouseCelly], moveAl.m_path);
            m_cancelButton->hide();
            m_gameState = BEGIN;
            if (m_nowCharacter->m_characterState != Character::DEAD)
                m_nowCharacter->m_characterState = BEGIN;
            repaint();
        }
        else return;
    }

    // 如果是寻找攻击目标阶段
    else if (m_gameState == FINDATTRACK)
    {
        //在攻击范围内
        if (attrackAl.m_resultMap[m_mouseCellx][m_mouseCelly] != -1)
        {
            for (int i = 0; i < m_characterNum; i++)
            {
                if (m_character[i]->m_cellx == m_mouseCellx && m_character[i]->m_celly == m_mouseCelly)//点中
                {
                    if (m_character[i]->m_belong == m_roundBelonged ||//友军
                        m_character[i]->m_characterState == Character::DEAD)//死了
                    {
                        m_clickSound->play();
                        m_gameState = BEGIN;
                        m_nowCharacter->m_characterState = BEGIN;
                        m_nowCharacter->m_attrackedOrNot = false;
                        break;
                    }
                    m_cancelButton->hide();
                    m_nowCharacter->m_attracker->startMove(m_nowCharacter, m_nowCharacter->m_localCellx, m_nowCharacter->m_localCelly,
                        m_character[i]->m_localCellx, m_character[i]->m_localCelly);
                    // 等待攻击完毕，发动结算机制
                    connect(m_nowCharacter->m_attracker, &AttrackAnimation::animationFinished, this, [=]() {
                        emit m_character[i]->beAttracked(m_nowCharacter->m_attrack);
                        disconnect(m_nowCharacter->m_attracker, 0, this, 0); //仅结算一次，因此要断连
                        m_gameState = BEGIN;
                        m_nowCharacter->m_characterState = BEGIN;
                        m_nowCharacter->m_attrackedOrNot = true;
                        });
                    break;
                }
            }
        }
        else//偷懒了
        {
            m_gameState = BEGIN;
            m_nowCharacter->m_characterState = BEGIN;
            m_nowCharacter->m_attrackedOrNot = false;
        }
        repaint();
    }

}
// 调用则显示提示框
void GameScene::receiveHint(const QString str)const
{
    m_hint->setText(str);
    m_hint->show();
    m_hint->raise();
    QTimer::singleShot(1500, this, [=]() {m_hint->hide(); });
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
    m_mousex = point.x();
    m_mousey = point.y();
    // 绝对格子坐标
    m_mouseCellx = (-m_x + m_mousex) / CELL_SIZE + 1;
    m_mouseCelly = (-m_y + m_mousey) / CELL_SIZE + 1;
    // 相对格子坐标
    mouseLocalCellx = (m_mousex) / CELL_SIZE + 1;
    mouseLocalCelly = (m_mousey) / CELL_SIZE + 1;
}
// 如果鼠标移至屏幕边界处则滚动屏幕，反之不滚动
void GameScene::checkScreenMove()const
{
    // 向右滚
    if (m_mousex >= WINDOW_WIDTH - WINDOW_BOUNDARY
        && -m_x <= MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE)
        emit moveRight();
    // 停止向右
    if (-m_x > MAP_WIDTH - WINDOW_WIDTH - CELL_SIZE
        || m_mousex < WINDOW_WIDTH - WINDOW_BOUNDARY)
        emit notMoveRight();

    // 向左滚
    if (m_mousex <= WINDOW_BOUNDARY && m_x <= 0)
        emit moveLeft();
    // 停止向左滚
    if (m_x > 0 || m_mousex > WINDOW_BOUNDARY)
        emit notMoveLeft();

    // 向下滚
    if (m_mousey >= WINDOW_HEIGHT - WINDOW_BOUNDARY
        && -m_y <= MAP_HEIGHT - WINDOW_HEIGHT - CELL_SIZE)
        emit moveDown();
    // 停止向下滚
    if (-m_y > MAP_HEIGHT - WINDOW_HEIGHT - CELL_SIZE
        || m_mousey < WINDOW_HEIGHT - WINDOW_BOUNDARY)
        emit notMoveDown();

    // 向上滚
    if (m_mousey <= WINDOW_BOUNDARY && m_y <= 0)
        emit moveUp();
    // 停止向上滚
    if (m_y > 0 || m_mousey > WINDOW_BOUNDARY)
        emit notMoveUp();
}
// 设置滚动界面定时器
void GameScene::setScreenMoveTimer()
{
    // 向右滚
    QTimer* rtimer = new QTimer(this);
    rtimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    // 将向右滚动定时器连接向右/停止向右
    connect(this, SIGNAL(moveRight()), rtimer, SLOT(start()));
    connect(this, SIGNAL(notMoveRight()), rtimer, SLOT(stop()));
    connect(rtimer, &QTimer::timeout, this, [this]()
        {
            if (m_screenMoveOrNot == false)return;
            if (-m_x >= MAP_WIDTH - WINDOW_WIDTH)
            {
                emit notMoveRight();
                return;
            }
            // 更新人物坐标
            for (int i = 0; i < m_characterNum; i++)
            {
                m_character[i]->m_localCellx--;
            }
            m_x -= CELL_SIZE;
            // 发送屏幕滚动信号
            emit moveScreen();
        });

    // 向左滚
    QTimer* ltimer = new QTimer(this);
    ltimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveLeft()), ltimer, SLOT(start()));
    connect(this, SIGNAL(notMoveLeft()), ltimer, SLOT(stop()));
    connect(ltimer, &QTimer::timeout, this, [this]()
        {
            if (m_screenMoveOrNot == false)return;
            if (m_x >= 0)
            {
                emit notMoveLeft();
                return;
            }

            for (int i = 0; i < m_characterNum; i++)
            {
                m_character[i]->m_localCellx++;
            }
            m_x += CELL_SIZE;
            emit moveScreen();
        });

    // 向下滚
    QTimer* dtimer = new QTimer(this);
    dtimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveDown()), dtimer, SLOT(start()));
    connect(this, SIGNAL(notMoveDown()), dtimer, SLOT(stop()));
    connect(dtimer, &QTimer::timeout, this, [this]()
        {
            if (m_screenMoveOrNot == false)return;
            if (-m_y >= MAP_HEIGHT - WINDOW_HEIGHT)
            {
                emit notMoveDown();
                return;
            }
            for (int i = 0; i < m_characterNum; i++)
            {
                m_character[i]->m_localCelly--;
            }
            m_y -= CELL_SIZE;
            emit moveScreen();
        });

    // 向上滚
    QTimer* utimer = new QTimer(this);
    utimer->setInterval(MOUSE_MOVE_TIMER_INTERVAL);
    connect(this, SIGNAL(moveUp()), utimer, SLOT(start()));
    connect(this, SIGNAL(notMoveUp()), utimer, SLOT(stop()));
    connect(utimer, &QTimer::timeout, this, [this]()
        {
            if (m_screenMoveOrNot == false)return;
            if (m_y >= 0)
            {
                emit notMoveUp();
                return;
            }
            for (int i = 0; i < m_characterNum; i++)
            {
                m_character[i]->m_localCelly++;
            }
            m_y += CELL_SIZE;
            emit moveScreen();
        });
}
