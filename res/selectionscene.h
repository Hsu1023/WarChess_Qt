#ifndef SELECTIONSCENE_H
#define SELECTIONSCENE_H

#include <QWidget>
#include "gamescene.h"
#include "gamelabel.h"

class SelectionScene : public QDialog
{
    Q_OBJECT
public:
    SelectionScene(int gameMode = ONEPLAYER, QWidget* parent = nullptr);
public:
    GameScene *gameScene;
    QPixmap background;
    void paintEvent(QPaintEvent*)override;
    ClickLabel *button[4];
    int lastSelection;
    int lastGameMode;
    void createGameScene(int chapter, int gameMode);
signals:
    void exit();
};

#endif // SELECTIONSCENE_H
