#include "selectionscene.h"

/*  MainScene是关于选择关卡页面的类，有游戏页面变量 */

SelectionScene::SelectionScene(const int gameMode, QWidget* parent) :
    QDialog(parent)
{
    // 初始化配置
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    m_background.load(":/pic/selection_background.png");

    // 按钮1：第一关
    QPixmap m_pixmap = QPixmap(":/pic/chapter1_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_button[0] = new ClickLabel(200, 115, m_pixmap, this, ClickLabel::TRANSPARENTSTYLE);
    m_button[0]->setGeometry(300, 500, 200, 115);
    m_button[0]->raise();
    m_button[0]->show();

    // 按钮2：第二关
    m_pixmap = QPixmap(":/pic/chapter2_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_button[1] = new ClickLabel(200, 115, m_pixmap, this, ClickLabel::TRANSPARENTSTYLE);
    m_button[1]->setGeometry(725, 350, 200, 115);
    m_button[1]->raise();
    m_button[1]->show();

    // 按钮3："..."的装饰品
    m_pixmap = QPixmap(":/pic/chaptermore_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_button[2] = new ClickLabel(200, 115, m_pixmap, this, ClickLabel::NOSTYLE);
    m_button[2]->setGeometry(1100, 500, 200, 115);
    m_button[2]->raise();
    m_button[2]->show();

    // 按钮4：返回首页
    m_pixmap = QPixmap(":/pic/chapterreturn_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_button[3] = new ClickLabel(200, 115, m_pixmap, this, ClickLabel::TRANSPARENTSTYLE);
    m_button[3]->setGeometry(725, 650, 200, 115);
    m_button[3]->raise();
    m_button[3]->show();

    // 记录首页和当前页的选项，以备“重新游戏”使用
    m_lastSelection = -1;
    m_lastGameMode = -1;

    // 点击按钮1开启下一关
    connect(m_button[0], &ClickLabel::clicked, this, [=]() {
        zoom(m_button[0]);
        m_lastSelection = 1;
        m_lastGameMode = gameMode;
        createGameScene(1, gameMode);
        });

    // 点击按钮2开启下一关
    connect(m_button[1], &ClickLabel::clicked, this, [=]() {
        zoom(m_button[1]);
        m_lastSelection = 2;
        m_lastGameMode = gameMode;
        createGameScene(2, gameMode);
        });

    // 点击按钮3退回首页
    connect(m_button[3], &ClickLabel::clicked, this, [=]() {
        zoom(m_button[3]);
        this->close();
        emit exit();
        });

}
void SelectionScene::createGameScene(const int chapter, const int gameMode)
{
    // 下一关的函数
    m_gameScene = new GameScene(chapter, gameMode);

    // 处理游戏界面的退出信号
    connect(m_gameScene, &GameScene::exit, this, [=]() {
        m_gameScene->close();
        //delete gameScene;
        m_gameScene = nullptr;
        this->close();
        emit exit();
        });

    // 处理游戏界面的重新开始游戏界面
    connect(m_gameScene, &GameScene::restart, this, [=]() {
        m_gameScene->close();
        //delete gameScene;
        m_gameScene = nullptr;
        createGameScene(m_lastSelection, m_lastGameMode);
        });
    m_gameScene->show();
    this->hide();
}
void SelectionScene::paintEvent(QPaintEvent*)
{
    // 画背景
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_background);
}
