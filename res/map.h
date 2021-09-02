#ifndef MAP_H
#define MAP_H

#include "config.h"
#include <QWidget>

class Map : public QWidget
{
    Q_OBJECT
public:
    explicit Map(QWidget *parent = nullptr);
public:
    QPixmap m_map1;
    int m_x, m_y;
signals:

public slots:
    void paint(QPainter&)const;
    //void calculatePicPosition()const;
};

#endif // MAP_H
