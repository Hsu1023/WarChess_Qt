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
        button[i]->setGeometry(600,200+200*i,400,160);
        button[i]->raise();
    }
    button[0]->setPixmap(QPixmap(":/pic/back_to_game.png"));
    button[1]->setPixmap(QPixmap(":/pic/restart.png"));
    button[2]->setPixmap(QPixmap(":/pic/exit_game.png"));

    connect(button[0], &ClickLabel::clicked, this, &PlayingMenu::hide);
    //connect(button[2], &)
}
