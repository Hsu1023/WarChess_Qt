#ifndef GAMELABEL_H
#define GAMELABEL_H

#include <QLabel>

class HintLabel: public QLabel
{
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
    explicit ClickLabel(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event)override;
signals:
    void clicked();
};

#endif // GAMELABEL_H
