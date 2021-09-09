#include "selectionscene.h"

SelectionScene::SelectionScene(int gameMode, QWidget* parent):
    QDialog(parent)
{
    // 初始化配置
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    background.load(":/pic/selection_background.png");

    // 按钮1：第一关
    QPixmap m_pixmap = QPixmap(":/pic/chapter1_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[0] = new ClickLabel(200,115,m_pixmap,this,ClickLabel::TRANSPARENTSTYLE);
    button[0]->setGeometry(300,500,200,115);
    button[0]->raise();
    button[0]->show();

    // 按钮2：第二关
    m_pixmap = QPixmap(":/pic/chapter2_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[1] = new ClickLabel(200, 115,m_pixmap,this,ClickLabel::TRANSPARENTSTYLE);
    button[1]->setGeometry(725,350,200,115);
    button[1]->raise();
    button[1]->show();

    // 按钮3："..."的装饰品
    m_pixmap = QPixmap(":/pic/chaptermore_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[2] = new ClickLabel(200, 115,m_pixmap,this,ClickLabel::NOSTYLE);
    button[2]->setGeometry(1100,500,200,115);
    button[2]->raise();
    button[2]->show();

    // 按钮4：返回首页
    m_pixmap = QPixmap(":/pic/chapterreturn_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[3] = new ClickLabel(200, 115,m_pixmap,this,ClickLabel::TRANSPARENTSTYLE);
    button[3]->setGeometry(725,650,200,115);
    button[3]->raise();
    button[3]->show();

    // 记录首页和当前页的选项，以备“重新游戏”使用
    lastSelection = -1;
    lastGameMode = -1;

    // 点击按钮1开启下一关
    connect(button[0], &ClickLabel::clicked, this, [=](){
        zoom(button[0]);
        lastSelection = 1;
        lastGameMode = gameMode;
        createGameScene(1 ,gameMode);
    });

    // 点击按钮2开启下一关
    connect(button[1], &ClickLabel::clicked, this, [=](){
        zoom(button[1]);
        lastSelection = 2;
        lastGameMode = gameMode;
        createGameScene(2, gameMode);
    });

    // 点击按钮3退回首页
    connect(button[3], &ClickLabel::clicked, this, [=](){
        zoom(button[3]);
        this->close();
        emit exit();
    });

}
void SelectionScene::createGameScene(int chapter, int gameMode)
{
    // 下一关的函数
    gameScene = new GameScene(chapter, gameMode);

    // 处理游戏界面的退出信号
    connect(gameScene, &GameScene::exit, this, [=](){
        gameScene->close();
        //delete gameScene;
        gameScene = nullptr;
        this->close();
        emit exit();
    });

    // 处理游戏界面的重新开始游戏界面
    connect(gameScene, &GameScene::restart, this, [=](){
        gameScene->close();
        //delete gameScene;
        gameScene = nullptr;
        createGameScene(lastSelection, lastGameMode);
    });
    gameScene->show();
    this->hide();
}
void SelectionScene::paintEvent(QPaintEvent *)
{
    // 画背景
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);
}
