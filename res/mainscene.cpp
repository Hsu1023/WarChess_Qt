#include "mainscene.h"
#include "config.h"

MainScene::MainScene(QWidget* parent):
    QDialog(parent)
{
    // 初始配置
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);
    background.load(":/pic/background.png");

    // 设置三个按键
    QPixmap m_pixmap0 = QPixmap(":/pic/guide_button.png");
    QPixmap m_pixmap1 = QPixmap(":/pic/one_player_button.png");
    QPixmap m_pixmap2 = QPixmap(":/pic/two_players_button.png");

    // 按键1：显示教程
    button[0] = new ClickLabel(400,120,m_pixmap0,this,ClickLabel::TRANSPARENTSTYLE);
    button[0]->setGeometry(610,450,400,120);
    button[0]->raise();
    button[0]->show();

    // 按键2：人机模式
    button[1] = new ClickLabel(400,120,m_pixmap1,this,ClickLabel::TRANSPARENTSTYLE);
    button[1]->setGeometry(610,600,400,120);
    button[1]->raise();
    button[1]->show();

    // 按键3：双人模式
    button[2] = new ClickLabel(400,120,m_pixmap2,this,ClickLabel::TRANSPARENTSTYLE);
    button[2]->setGeometry(610,750,400,120);
    button[2]->raise();
    button[2]->show();

    guideScene = new GuideScene(this);

    // 按键1：显示教程
    connect(button[0], &ClickLabel::clicked, this, [=](){
        zoom(button[0]);
        guideScene->show();
        this->hide();
    });

    // 按键2：人机模式
    connect(button[1], &ClickLabel::clicked, this,[=](){
        zoom(button[1]);
        selectionScene = new SelectionScene(ONEPLAYER);
        // 连接选择界面的退出接口
        connect(selectionScene, &SelectionScene::exit,this, [=](){
            this->show();
            selectionScene->hide();
        });
        selectionScene->show();
        // 延时300毫秒，保证显示流畅
        QTime dieTime = QTime::currentTime().addMSecs(300);
        while (QTime::currentTime() < dieTime)
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
       this->hide();
    });

    // 按键3：双人模式
    connect(button[2], &ClickLabel::clicked, this,[=](){
        zoom(button[2]);
        selectionScene = new SelectionScene(TWOPLAYERS);
        // 连接选择界面的退出接口
        connect(selectionScene, &SelectionScene::exit,this, [=](){
            this->show();
            selectionScene->hide();
        });
        selectionScene->show();
        this->hide();
    });
}
void MainScene::paintEvent(QPaintEvent *)
{
    // 画背景
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);
}
