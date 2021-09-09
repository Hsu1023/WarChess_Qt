#ifndef MINIMAP_H
#define MINIMAP_H

#include <QWidget>
#include "character.h"
#include "gamemap.h"

class Minimap : public QWidget
{
    Q_OBJECT
public:
    Character **character;
    int characterNum;
    int maxCellx, maxCelly;
public:
    explicit Minimap(Character** t_character, int t_characterNum, int t_maxCellx, int t_maxCelly,  QWidget *parent = nullptr);
    void paintEvent(QPaintEvent*)override;
signals:

};

#endif // MINIMAP_H
