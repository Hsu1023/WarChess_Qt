#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QWidget>
#include <QLabel>
#include "config.h"

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickLabel(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event)override;
signals:
    void clicked();
};

#endif // CLICKLABEL_H
