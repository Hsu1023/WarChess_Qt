#include "mainscene.h"

MainScene::MainScene(QWidget* parent):
    QWidget(parent)
{

    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setMouseTracking(true);

    background.load(":/pic/background.png");

    QPixmap m_pixmap = QPixmap(":/pic/begin_button.png");

   // m_pixmap = m_pixmap.scaled(400, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    button = new ClickLabel(400,120,m_pixmap,this,ClickLabel::TRANSPARENTSTYLE);
    button->setGeometry(610,750,400,120);
    button->raise();
    button->show();

    connect(button, &ClickLabel::clicked, [=](){
        selectionScene = new SelectionScene;
        connect(selectionScene, &SelectionScene::exit, [=](){
            this->show();
            selectionScene->hide();
        });
        selectionScene->show();
        this->hide();
    });

}
void MainScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);
}
