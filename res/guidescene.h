#ifndef GUIDESCENE_H
#define GUIDESCENE_H

#include <QDialog>

class GuideScene : public QDialog
{
    Q_OBJECT
public:
    int page;
    GuideScene(QWidget * parent = nullptr);
    void mousePressEvent(QMouseEvent*)override;

    void paintEvent(QPaintEvent*)override;
    QPixmap pic[2];
};

#endif // GUIDESCENE_H
