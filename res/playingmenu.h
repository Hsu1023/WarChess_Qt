#ifndef PLAYINGMENU_H
#define PLAYINGMENU_H

#include "gamelabel.h"
#include "config.h"
#include <QWidget>
#include <QPushButton>

class PlayingMenu : public QWidget
{
    Q_OBJECT
public:
    explicit PlayingMenu(QWidget *parent = nullptr);
    ClickLabel *button[3];
    QWidget *shadowDlg;
signals:
    void restartGame(); void exitGame();
};

class ResultMenu : public QWidget
{
    Q_OBJECT
public:
    explicit ResultMenu( QWidget *parent = nullptr);//蓝方获胜1 红方获胜0
    ClickLabel *button[3];
    HintLabel * resultLabel;
    QWidget *shadowDlg;
    void setResult(bool blueWinOrNot, bool AIOpenOrNot);
signals:
    void restartGame();
    void exitGame();
    void startVideo();
};
#endif // PLAYINGMENU_H
