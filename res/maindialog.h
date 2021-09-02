#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "map.h"
#include "config.h"


class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void paintEvent(QPaintEvent*)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void setMouseMoveTimer();
protected:
    Map m_map;
    int m_x, m_y;
signals:
    void moveRight();
    void notMoveRight();
    void moveLeft();
    void notMoveLeft();
};
#endif // MAINDIALOG_H
