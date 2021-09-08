#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "gamescene.h"
#include "gamelabel.h"
#include "selectionscene.h"
#include <QWidget>

class MainScene :public QDialog
{
public:
    MainScene(QWidget* parent = nullptr);
public:
    SelectionScene *selectionScene;
    QPixmap background;
    void paintEvent(QPaintEvent*)override;
    ClickLabel *button;
};

#endif // MAINSCENE_H
