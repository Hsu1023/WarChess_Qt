#ifndef CHARACTERDLG_H
#define CHARACTERDLG_H

#include <QLabel>

class CharacterDlg : public QLabel
{
    Q_OBJECT
public:
    explicit CharacterDlg(QWidget *parent = nullptr);
    int m_cellx, m_celly; //  全局位置
    int m_localCellx, m_localCelly; // 桌面位置
signals:

};

#endif // CHARACTERDLG_H
