#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "gamescene.h"
#include "gamelabel.h"
#include "selectionscene.h"
#include "guidescene.h"
#include <QWidget>

class MainScene :public QDialog
{
public:
    MainScene(QWidget* parent = nullptr);
public:
    SelectionScene *selectionScene;
    GuideScene *guideScene;
    QPixmap background;
    void paintEvent(QPaintEvent*)override;
    ClickLabel *button[3];
};

#endif // MAINSCENE_H
