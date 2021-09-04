#include "character.h"

Character::Character( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, QWidget* parent):
    QWidget(parent),
    m_cellx(t_cellx), m_celly(t_celly),
    m_localCellx(t_cellx - LocalScreenx / CELL_SIZE),
    m_localCelly(t_celly - LocalScreeny / CELL_SIZE)
{
}
void Character::setLabel()
{

    dlg = new QLabel(this);
    dlg->setFixedSize(165, 150);
    dlg->setPixmap(QPixmap(":/pic/dlg.png"));
    if((m_localCellx-1)*CELL_SIZE-165>=0)
        dlg->setGeometry((m_localCellx-1)*CELL_SIZE-165,(m_localCelly-1)*CELL_SIZE,165,150);
    else
        dlg->setGeometry((m_localCellx-1)*CELL_SIZE+32,(m_localCelly-1)*CELL_SIZE,165,150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout *layout = new QGridLayout(dlg);
    QLabel * nameLabel = new QLabel(name);
    QLabel * hpLabel1 = new QLabel("血量");
    QLabel * hpLabel2 = new QLabel(QString::number(m_blood)+"/"+QString::number(m_fullblood));
    QLabel * attrackLabel1 = new QLabel("攻击力");
    QLabel * attrackLabel2 = new QLabel(QString::number(m_attrack));
    QLabel * moveLabel1 = new QLabel("行动力");
    QLabel * moveLabel2 = new QLabel(QString::number(m_move));
    layout->addWidget(nameLabel,0,0);
    layout->addWidget(hpLabel1,1,0);
    layout->addWidget(hpLabel2,1,1);
    layout->addWidget(attrackLabel1,2,0);
    layout->addWidget(attrackLabel2,2,1);

    layout->addWidget(moveLabel1,3,0);
    layout->addWidget(moveLabel2,3,1);
    layout->setVerticalSpacing(0);
    dlg->setLayout(layout);
    dlg->show();
}
Warrior::Warrior( int t_cellx, int t_celly, int LocalScreenx, int LocalScreeny, QWidget* parent):
    Character( t_cellx, t_celly, LocalScreenx, LocalScreeny, parent)
{
    m_blood =50; m_fullblood = 100;
    m_move = 4;
    m_attrack = 40;
    icon.load(WARRIOR_PATH);
    name = "勇士";
    setMouseTracking(true);
    setLabel();
}


