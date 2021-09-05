#include "clicklabel.h"

ClickLabel::ClickLabel(QWidget *parent) : QLabel(parent)
{
}
void ClickLabel::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
    QLabel::mousePressEvent(event);
}
