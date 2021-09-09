#include "guidescene.h"
#include "config.h"

GuideScene::GuideScene(QWidget * parent):
    QDialog(parent)
{
    // 初始化
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    page = 0;
    pic[0]=QPixmap(GUIDE1_PATH);
    pic[1]=QPixmap(GUIDE2_PATH);
}
void GuideScene::mousePressEvent(QMouseEvent *)
{
    // 点击翻页
    page++;
    repaint();
}
void GuideScene::paintEvent(QPaintEvent *)
{
    // 如果到达尾页则退回游戏首页，否则显示
    QPainter painter(this);
    if(page==2){
        page = 0;
        parentWidget()->show();
        this->hide();
        return;
    }
    painter.drawPixmap(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,pic[page]);

}
