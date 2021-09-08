#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "map.h"
#include "character.h"
#include "config.h"
#include "algorithm.h"
#include "gamelabel.h"
#include "aicontroller.h"
#include "animation.h"
#include "playingmenu.h"

class GameScene : public QDialog
{
    Q_OBJECT
public:
    FindPathAlgorithm moveAl, attrackAl;
public:
    enum GAMESTATE{BEGIN,FINDPATH,FINDATTRACK,AI,END};
    GameScene(QWidget *parent = nullptr);
    ~GameScene();
    void paintEvent(QPaintEvent*)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void mousePressEvent(QMouseEvent*)override;
    void setScreenMoveTimer();
    void setButton();
    void checkScreenMove();
    void updateMousePosition(QMouseEvent*);
    void nextRound(int last);
    void AIRound();
    void AIMoveCharacter(int id);
protected:
    Map m_map;
    int m_x, m_y;
    int mousex, mousey;
    int mouseCellx, mouseCelly;
    int mouseLocalCellx, mouseLocalCelly;
    Character * character[100], *nowCharacter;
    int characterNum;
    int gameState;
    int aliveNum[2];
    int roundNum[2];
    bool roundBelonged;
    bool AIOpenOrNot;
    bool screenMoveOrNot;
    ClickLabel *cancelButton, *skipButton, *menuButton, *musicButton;
    HintLabel *hint;
    AIController *aiController;
    QSound *bgm, *attrackSound, *clickSound;
    MoveAnimation *ani;
    PlayingMenu *playingMenu;
    ResultMenu *resultMenu;
signals:
    void moveRight(); void notMoveRight();
    void moveLeft(); void notMoveLeft();
    void moveUp(); void notMoveUp();
    void moveDown(); void notMoveDown();
    void moveScreen();
    void myLoss(); void myWin();
    void restart(); void exit();
public slots:
    void redrawCharacter();
    void characterMoveEvent(Character*);
    void characterAttrackEvent(Character*);
    void endOneCharacterEvent(Character*);
    void dieOneCharacterEvent(Character*);
    void receiveHint(QString str);
    //void attrackAnimationWaiter();
};
#endif // GAMESCENE_H
