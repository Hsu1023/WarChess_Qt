#include "selectionscene.h"

SelectionScene::SelectionScene(int gameMode, QWidget* parent):
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    background.load(":/pic/selection_background.png");

    QPixmap m_pixmap = QPixmap(":/pic/chapter1_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[0] = new ClickLabel(200,115,m_pixmap,this,ClickLabel::TRANSPARENTSTYLE);
    button[0]->setGeometry(300,500,200,115);
    button[0]->raise();
    button[0]->show();

    m_pixmap = QPixmap(":/pic/chapter2_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[1] = new ClickLabel(200, 115,m_pixmap,this,ClickLabel::TRANSPARENTSTYLE);
    button[1]->setGeometry(725,350,200,115);
    button[1]->raise();
    button[1]->show();

    m_pixmap = QPixmap(":/pic/chaptermore_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[2] = new ClickLabel(200, 115,m_pixmap,this,ClickLabel::NOSTYLE);
    button[2]->setGeometry(1100,500,200,115);
    button[2]->raise();
    button[2]->show();

    m_pixmap = QPixmap(":/pic/chapterreturn_button.png");
    m_pixmap = m_pixmap.scaled(200, 115, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[3] = new ClickLabel(200, 115,m_pixmap,this,ClickLabel::TRANSPARENTSTYLE);
    button[3]->setGeometry(725,650,200,115);
    button[3]->raise();
    button[3]->show();

    lastSelection = -1;

    connect(button[0], &ClickLabel::clicked, [=](){
        lastSelection = 1;
        createGameScene(1 ,gameMode);
    });
    connect(button[1], &ClickLabel::clicked, [=](){
        lastSelection = 2;
        createGameScene(2, gameMode);
    });
    connect(button[3], &ClickLabel::clicked, [=](){
        this->close();
        emit exit();
    });

}
void SelectionScene::createGameScene(int chapter, int gameMode)
{
    gameScene = new GameScene(chapter, gameMode);

    connect(gameScene, &GameScene::exit, [=](){
        gameScene->hide();
        this->hide();
        emit exit();
    });
    connect(gameScene, &GameScene::restart,[=](){
        gameScene->close();
        emit button[lastSelection - 1]->clicked();
    });
    gameScene->show();
    QTime dieTime = QTime::currentTime().addMSecs(300);//延时300毫秒
    while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    this->hide();
}
void SelectionScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);
}
