#include "hintlabel.h"
#include "config.h"

HintLabel::HintLabel( QWidget * parent,int x, int y):
    QLabel(parent)
{
    setGeometry(x,y,width(),height());
    setPixmap(QPixmap(":/pic/hint.png"));

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
    qDebug()<<"1";
}

