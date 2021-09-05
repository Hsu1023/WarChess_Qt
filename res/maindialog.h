#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "map.h"
#include "character.h"
#include "config.h"
#include "algorithm.h"
#include "clicklabel.h"
#include "hintlabel.h"

class MainDialog : public QDialog
{
    Q_OBJECT
public:
    FindPathAlgorithm moveAl, attrackAl;
public:
    enum GAMESTATE{BEGIN,FINDPATH,FINDATTRACK};
    enum BELONGING{MINE,YOURS};
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void paintEvent(QPaintEvent*)override;
    void mouseMoveEvent(QMouseEvent*)override;

    void mousePressEvent(QMouseEvent*)override;
    void setScreenMoveTimer();
    void setButton();
    void checkScreenMove();
    void updateMousePosition(QMouseEvent*);
    void nextRound(int last);
protected:
    Map m_map;
    int m_x, m_y;
    int mousex, mousey;
    int mouseCellx, mouseCelly;
    int mouseLocalCellx, mouseLocalCelly;
    Character * character[100], *nowCharacter;
    bool characterBelonged[100];//我0你1
    int characterNum;
    int gameState;
    int aliveNum[2];
    int roundNum[2];
    bool roundBelonged;
    ClickLabel *cancelButton, *skipButton, *menuButton, *musicButton;
    HintLabel *hint;
signals:
    void moveRight(); void notMoveRight();
    void moveLeft(); void notMoveLeft();
    void moveUp(); void notMoveUp();
    void moveDown(); void notMoveDown();
    void moveScreen();
    void myLoss(); void myWin();
public slots:
    void redrawCharacter();
    void characterMoveEvent(Character*);
    void characterAttrackEvent(Character*);
    void endOneCharacterEvent(Character*);
    void dieOneCharacterEvent(Character*);
    void receiveHint(QString str);
};
#endif // MAINDIALOG_H
