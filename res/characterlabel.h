#ifndef CHARACTERLABEL_H
#define CHARACTERLABEL_H

#include <QLabel>
#include "config.h"

class CharacterProperty: public QLabel
{
    Q_OBJECT
public:
    explicit CharacterProperty(const QString, const int, const int, const int,const int, QWidget* parent = nullptr);
public:
    QGridLayout m_layout;
    QLabel m_nameLabel, m_hpLabel1, m_hpLabel2, m_moveLabel1, m_moveLabel2, m_attrackLabel1, m_attrackLabel2, m_attrackableLabel1, m_attrackableLabel2;
    // 更新属性栏数据
    void updateData(const int hp, const int fullhp, const int move, const int fullmove, const int localCellx, const int localCelly);
};

class CharacterSelection: public QLabel
{
    Q_OBJECT
public:
    explicit CharacterSelection(QWidget* parent = nullptr);
public:
    QVBoxLayout *m_layout;
    QPushButton *m_moveButton, *m_attrackButton, *m_skipButton; // 操作栏上三个按钮
    // 更新操作栏数据
    void updateData(const int localCellx, const int localCelly); // 更新数据
};

class Character;
class HPLabel: public QLabel
{
    Q_OBJECT
public:
    Character *belong; // 血条归属的人物
    explicit HPLabel(const int hp, const int totalhp, const bool belong, QWidget* parent = nullptr);
public:
    int m_hp, m_totalhp, m_belong; // 血量值
    void reset(const int hp, const int totalhp, const bool belong); //重置血量值
    void paintEvent(QPaintEvent*)override; // 绘制
};

#endif // CHARACTERLABEL_H
