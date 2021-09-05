#ifndef HINTLABEL_H
#define HINTLABEL_H

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

#endif // HINTLABEL_H
