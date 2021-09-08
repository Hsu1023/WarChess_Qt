#include "mainscene.h"
#include "config.h"

MainScene::MainScene(QWidget* parent):
    QDialog(parent)
{

    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    background.load(":/pic/background.png");

    QPixmap m_pixmap1 = QPixmap(":/pic/one_player_button.png");
    QPixmap m_pixmap2 = QPixmap(":/pic/two_players_button.png");

   // m_pixmap = m_pixmap.scaled(400, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    button[0] = new ClickLabel(400,120,m_pixmap1,this,ClickLabel::TRANSPARENTSTYLE);
    button[0]->setGeometry(610,550,400,120);
    button[0]->raise();
    button[0]->show();

    button[1] = new ClickLabel(400,120,m_pixmap2,this,ClickLabel::TRANSPARENTSTYLE);
    button[1]->setGeometry(610,750,400,120);
    button[1]->raise();
    button[1]->show();

    connect(button[0], &ClickLabel::clicked, this,[=](){
        selectionScene = new SelectionScene(ONEPLAYER);
        connect(selectionScene, &SelectionScene::exit,this, [=](){
            this->show();
            selectionScene->hide();
        });
        selectionScene->show();
        QTime dieTime = QTime::currentTime().addMSecs(300);//延时300毫秒
        while (QTime::currentTime() < dieTime)
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
       this->hide();
    });
    connect(button[1], &ClickLabel::clicked, this,[=](){
        selectionScene = new SelectionScene(TWOPLAYERS);
        connect(selectionScene, &SelectionScene::exit,this, [=](){
            this->show();
            selectionScene->hide();
        });
        selectionScene->show();
        QTime dieTime = QTime::currentTime().addMSecs(300);//延时300毫秒
        while (QTime::currentTime() < dieTime)
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
       this->hide();
    });

}
void MainScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);
}
