#ifndef MINIMAP_H
#define MINIMAP_H

#include <QWidget>
#include "character.h"
#include "gamemap.h"

class Minimap : public QWidget
{
    Q_OBJECT
protected:
    Character **m_character; // 角色指针
    int m_characterNum; // 总角色数
    int m_maxCellx, m_maxCelly; // 地图格点size
public:
    explicit Minimap(Character** t_character, const int t_characterNum, const int t_maxCellx, const int t_maxCelly,  QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent*)override;
signals:

};

#endif // MINIMAP_H
