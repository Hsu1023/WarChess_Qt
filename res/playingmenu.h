#ifndef PLAYINGMENU_H
#define PLAYINGMENU_H

#include "gamelabel.h"
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

};

#endif // PLAYINGMENU_H
