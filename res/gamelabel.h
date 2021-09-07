#ifndef GAMELABEL_H
#define GAMELABEL_H

#include <QLabel>

class HintLabel: public QLabel
{
    Q_OBJECT
public:
    HintLabel( QWidget* parent = nullptr,int x=550, int y=70);
    HintLabel(QString t_str, QWidget* parent = nullptr,int x=550, int y=70);
public:
    QString str;
    void paintEvent(QPaintEvent *)override;
    void setText(QString);
};

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    enum STYLE{NOSTYLE, TRANSPARENTSTYLE};
public:
    QPixmap pic;
    bool enterOrNot;
    int style;
    explicit ClickLabel(int w, int h, QPixmap map , QWidget *parent = nullptr, int style = NOSTYLE);
    void mousePressEvent(QMouseEvent *event)override;
    void enterEvent(QEvent*)override;
    void leaveEvent(QEvent*)override;
    void paintEvent(QPaintEvent*)override;
signals:
    void clicked();
};

#endif // GAMELABEL_H
