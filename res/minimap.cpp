#include "minimap.h"

/*  MiniMap是小地图类，通过override paintEvent显示小地图 */

Minimap::Minimap(Character** t_character, const int t_characterNum, const int t_maxCellx, const int t_maxCelly, QWidget* parent) :
    QWidget(parent)
{
    // 初始化变量
    m_character = t_character;
    m_characterNum = t_characterNum;
    m_maxCellx = t_maxCellx;
    m_maxCelly = t_maxCelly;
    setFixedSize(10 * m_maxCellx, 10 * m_maxCelly);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}
void Minimap::paintEvent(QPaintEvent*)
{
    // 列举几种可能用于绘制的颜色
    enum COLOR { GREEN, BLUE, RED, WATER, SAND };
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    for (int i = 1; i <= m_maxCellx; i++)
    {
        for (int j = 1; j <= m_maxCelly; j++)
        {
            // 默认为普通地形
            int color = GREEN;
            for (int k = 0; k < m_characterNum; k++)
            {
                // 人物死了则不画
                if (m_character[k]->m_characterState == Character::DEAD)continue;
                // 活人
                if (m_character[k]->m_cellx == i && m_character[k]->m_celly == j)
                {
                    // 蓝方涂蓝
                    if (m_character[k]->m_belong == MINE)color = BLUE;
                    // 红方涂红
                    else color = RED;

                }
            }
            // 如果仍为默认地形（没有角色）
            if (color == GREEN)
            {
                // 水地形
                if (GameMap::binMap[i][j] == 3)color = WATER;
                // 沙漠地形
                else if (GameMap::binMap[i][j] == 2)color = SAND;
            }
            // 设置对应颜色画刷
            if (color == GREEN)painter.setBrush(QColor(120, 255, 120, 100));//120 120 120
            else if (color == RED)painter.setBrush(QColor(255, 0, 100, 250));
            else if (color == BLUE)painter.setBrush(QColor(0, 0, 255, 250));
            else if (color == WATER)painter.setBrush(QColor(0, 50, 255, 100));
            else painter.setBrush(QColor(255, 255, 0, 100));
            painter.drawRect((i - 1) * 10, (j - 1) * 10, 10, 10);
        }
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, m_maxCellx * 10, m_maxCelly * 10);
}
