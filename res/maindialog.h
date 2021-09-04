#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "map.h"
#include "character.h"
#include "config.h"


class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void paintEvent(QPaintEvent*)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void setScreenMoveTimer();
    void checkScreenMove();
    void updateMousePosition(QMouseEvent*);
protected:
    Map m_map;
    int m_x, m_y;
    int mousex, mousey;
    int mouseCellx, mouseCelly;
    int mouseLocalCellx, mouseLocalCelly;
    Character * character[10];
    int characterNum;
signals:
    void moveRight(); void notMoveRight();
    void moveLeft(); void notMoveLeft();
    void moveUp(); void notMoveUp();
    void moveDown(); void notMoveDown();
    void moveScreen();
public slots:
    void redrawCharacter();
};
#endif // MAINDIALOG_H
