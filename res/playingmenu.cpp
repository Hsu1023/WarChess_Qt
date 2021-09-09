#include "playingmenu.h"

PlayingMenu::PlayingMenu(QWidget *parent) : QWidget(parent)
{ 
    // 设置默认值
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(1600,960);
    shadowDlg = new QWidget(this);
    QString str("QWidget{background-color:rgba(0,0,0,0.6);}");
    shadowDlg->setStyleSheet(str);
    shadowDlg->setGeometry(0, 0, 1600, 960);

    // 设置三个按钮
    QPixmap returnPixmap = QPixmap(":/pic/back_to_game.png");
    QPixmap restartPixmap = QPixmap(":/pic/restart.png");
    QPixmap exitPixmap = QPixmap(":/pic/exit_game.png");

    returnPixmap = returnPixmap.scaled(300, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    restartPixmap = restartPixmap.scaled(300, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    exitPixmap = exitPixmap.scaled(300, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    button[0] = new ClickLabel(300, 120, returnPixmap, this, ClickLabel::TRANSPARENTSTYLE);
    button[1] = new ClickLabel(300, 120, restartPixmap, this, ClickLabel::TRANSPARENTSTYLE);
    button[2] = new ClickLabel(300, 120, exitPixmap, this, ClickLabel::TRANSPARENTSTYLE);

    for(int i = 0; i < 3; i++)
    {
        button[i]->setGeometry(650,200+200*i,300,120);
        button[i]->raise();
    }

    // 按下第一个按钮：继续游戏
    connect(button[0], &ClickLabel::clicked, this, &PlayingMenu::hide);
    // 按下第二个按钮：发送重新开始游戏信号
    connect(button[1], &ClickLabel::clicked,this,[=](){emit restartGame();});
    // 按下第三个按钮：发送退出游戏信号
    connect(button[2], &ClickLabel::clicked,this,[=](){emit exitGame();});
}

ResultMenu::ResultMenu(QWidget *parent):
    QWidget(parent)
{
    // 设置三个按钮
    QPixmap restartPixmap = QPixmap(":/pic/restart.png");
    QPixmap exitPixmap = QPixmap(":/pic/exit_game.png");
    QPixmap videoPixmap = QPixmap(":/pic/review_button.png");

    restartPixmap = restartPixmap.scaled(200, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    exitPixmap = exitPixmap.scaled(200, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    videoPixmap = videoPixmap.scaled(200, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    button[1] = new ClickLabel(200,80,restartPixmap,this, ClickLabel::TRANSPARENTSTYLE);
    button[2] = new ClickLabel(200,80,exitPixmap,this, ClickLabel::TRANSPARENTSTYLE);
    button[0] = new ClickLabel(200,80,videoPixmap,this, ClickLabel::TRANSPARENTSTYLE);

    for(int i = 0; i < 3; i++)
    {
        button[i]->setGeometry(700,500+150*i,200,80);
        button[i]->raise();
    }
    // 按下第一个按钮：重新开始游戏
    connect(button[1], &ClickLabel::clicked,this,[=](){emit restartGame();});
    // 按下第二个按钮：退出游戏
    connect(button[2], &ClickLabel::clicked,this,[=](){emit exitGame();});
    // 按下第三个按钮：开始播放战斗回顾
    connect(button[0], &ClickLabel::clicked,this,[=](){emit startVideo();button[2]->hide();});
}
// 显示结果
void ResultMenu::setResult(bool blueWinOrNot, bool AIOpenOrNot)
{
    setFixedSize(1600,960);
    // 加阴影遮罩
    shadowDlg = new QWidget(this);
    QString str("QWidget{background-color:rgba(0,0,0,0.6);}");
    shadowDlg->setStyleSheet(str);
    shadowDlg->setGeometry(0, 0, 1600, 960);

    resultLabel = new HintLabel(this, 540, 200);

    // 显示按钮
    for(int i=0;i<3;i++)
        button[i]->raise();
    // 显示提示框
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
}
