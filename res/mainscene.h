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
    explicit MainScene(QWidget* parent = nullptr);
protected:
    SelectionScene *m_selectionScene; // 选择场景
    GuideScene *m_guideScene; // 教程场景
    QPixmap m_background; // 主页图片
    ClickLabel *m_button[3]; // 主页上三个选择案件
protected:
    void paintEvent(QPaintEvent*)override;
};

#endif // MAINSCENE_H
