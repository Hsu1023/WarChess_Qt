#ifndef SELECTIONSCENE_H
#define SELECTIONSCENE_H

#include <QWidget>
#include "gamescene.h"
#include "gamelabel.h"

class SelectionScene : public QDialog
{
    Q_OBJECT
public:
    explicit SelectionScene(const int gameMode = ONEPLAYER, QWidget* parent = nullptr);
protected:
    GameScene *m_gameScene; // 游戏场景
    QPixmap m_background; // 背景图片
    ClickLabel *m_button[4]; // 选择关卡上的四个按键
    int m_lastSelection; // 记录上次创建游戏场景的数据
    int m_lastGameMode; // 记录上次创建游戏场景的数据
    void paintEvent(QPaintEvent*)override;
    void createGameScene(const int chapter, const int gameMode); // 创建游戏场景
signals:
    void exit()const ; // 退出信号
};

#endif // SELECTIONSCENE_H
