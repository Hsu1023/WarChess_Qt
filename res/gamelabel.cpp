#include "gamelabel.h"
#include "config.h"

HintLabel::HintLabel( QWidget * parent, int x, int y):
    QLabel(parent)
{
    setPixmap(QPixmap(":/pic/hint.png"));
    setGeometry(x,y,width(),height());
}
HintLabel::HintLabel(QString t_str,QWidget * parent, int x, int y):
    QLabel(parent), str(t_str)
{
    setGeometry(x,y,width(),height());
    setPixmap(QPixmap(":/pic/hint.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(550, 150);
    update();
    show();
    raise();
}
void HintLabel::setText(QString t_str)
{
    str = t_str;
    setFixedSize(550, 150);
}
void HintLabel::paintEvent(QPaintEvent * ev)
{
    QLabel::paintEvent(ev);
    QPainter painter(this);
    QFont font("华文新魏", 30, 50, false);
    painter.setFont(font);
    painter.setPen(QPen(QColor(120,120,120)));
    QString title(str);
    int widthOfTitle = painter.fontMetrics().width(title);//字符串显示的像素大小
    painter.drawText(this->width()/2-widthOfTitle/2,90,title);
}

ClickLabel::ClickLabel(int w, int h, QPixmap map, QWidget *parent,int t_style) : QLabel(parent)
{
    setFixedSize(w, h);
    pic = map;
    enterOrNot = false;
    style = t_style;
}
void ClickLabel::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
    QLabel::mousePressEvent(event);
}
void ClickLabel::enterEvent(QEvent *)
{
    enterOrNot = true;
    repaint();
}
void ClickLabel::leaveEvent(QEvent *)
{
    enterOrNot = false;
    repaint();
}
void ClickLabel::paintEvent(QPaintEvent *eve)
{
    QLabel::paintEvent(eve);

    QPainter painter(this);
    if(style == TRANSPARENT)
    {
        if(enterOrNot)
            painter.setOpacity(0.6);
    }
    painter.drawPixmap(0, 0, pic);
}
void ClickLabel::setPixmap(QPixmap t_pic)
{
    pic = t_pic;
}

