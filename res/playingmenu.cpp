#include "playingmenu.h"

PlayingMenu::PlayingMenu(QWidget *parent) : QWidget(parent)
{
    setFixedSize(1600,960);
    shadowDlg = new QWidget(this);
    QString str("QWidget{background-color:rgba(0,0,0,0.6);}");
    shadowDlg->setStyleSheet(str);
    shadowDlg->setGeometry(0, 0, 1600, 960);

    for(int i = 0; i < 3; i++)
    {
        button[i] = new ClickLabel(this);
        button[i]->setGeometry(650,200+200*i,300,120);
        button[i]->raise();
    }

    QPixmap returnPixmap = QPixmap(":/pic/back_to_game.png");
    QPixmap restartPixmap = QPixmap(":/pic/restart.png");
    QPixmap exitPixmap = QPixmap(":/pic/exit_game.png");

    returnPixmap = returnPixmap.scaled(300, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    restartPixmap = restartPixmap.scaled(300, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    exitPixmap = exitPixmap.scaled(300, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    button[0]->setPixmap(returnPixmap);
    button[1]->setPixmap(restartPixmap);
    button[2]->setPixmap(exitPixmap);

    connect(button[0], &ClickLabel::clicked, this, &PlayingMenu::hide);
    connect(button[1], &ClickLabel::clicked,[=](){emit restartGame();});
    connect(button[2], &ClickLabel::clicked,[=](){emit exitGame();});
}

ResultMenu::ResultMenu(bool blueWinOrNot, bool AIOpenOrNot, QWidget *parent):
    QWidget(parent)
{
    setFixedSize(1600,960);
    shadowDlg = new QWidget(this);
    QString str("QWidget{background-color:rgba(0,0,0,0.6);}");
    shadowDlg->setStyleSheet(str);
    shadowDlg->setGeometry(0, 0, 1600, 960);

    resultLabel = new HintLabel(this, 540, 200);
    if(AIOpenOrNot)
    {
        if(blueWinOrNot)
            resultLabel->setText("您获胜了！");
        else
            resultLabel->setText("您失败了！");
    }
    else
    {
        if(blueWinOrNot)
            resultLabel->setText("蓝方获胜");
        else
            resultLabel->setText("红方获胜");
    }
    resultLabel->show();

    for(int i = 0; i < 2; i++)
    {
        button[i] = new ClickLabel(this);
        button[i]->setGeometry(700,450+150*i,200,80);
        button[i]->raise();
    }
    QPixmap restartPixmap = QPixmap(":/pic/restart.png");
    QPixmap exitPixmap = QPixmap(":/pic/exit_game.png");
    restartPixmap = restartPixmap.scaled(200, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    exitPixmap = exitPixmap.scaled(200, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    button[0]->setPixmap(restartPixmap);
    button[1]->setPixmap(exitPixmap);

    connect(button[0], &ClickLabel::clicked,[=](){emit restartGame();});
    connect(button[1], &ClickLabel::clicked,[=](){emit exitGame();});
}
