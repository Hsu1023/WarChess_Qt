#include "characterlabel.h"

CharacterProperty::CharacterProperty(QString name, int fullhp, int fullmove, int attrack, QWidget *parent):
    QLabel(parent)
{
    setFixedSize(165, 150);
    setPixmap(QPixmap(":/pic/dlg.png"));

    nameLabel.setText(name);
    hpLabel1.setText("血量");
    hpLabel2.setText(QString::number(fullhp)+"/"+QString::number(fullhp));
    moveLabel1.setText("行动力");
    moveLabel2.setText(QString::number(fullmove)+"/"+QString::number(fullmove));
    attrackLabel1.setText("攻击力");
    attrackLabel2.setText(QString::number(attrack));

    layout.addWidget(&nameLabel,0,0);
    layout.addWidget(&hpLabel1,1,0);
    layout.addWidget(&hpLabel2,1,1);
    layout.addWidget(&moveLabel1,2,0);
    layout.addWidget(&moveLabel2,2,1);
    layout.addWidget(&attrackLabel1,3,0);
    layout.addWidget(&attrackLabel2,3,1);
    layout.setVerticalSpacing(0);
    setLayout(&layout);
}
void CharacterProperty::updateData(int hp, int fullhp, int move1, int fullmove, int localCellx, int localCelly)
{
    bool ishidden = isHidden();
    int x = 0;
    if((localCellx-1)*CELL_SIZE-165 >= 0)
        x=(localCellx-1)*CELL_SIZE-165;
    else
        x=(localCellx-1)*CELL_SIZE+32;
    move(x,(localCelly-1)*CELL_SIZE);
    hpLabel2.setText(QString::number(hp)+"/"+QString::number(fullhp));
    moveLabel2.setText(QString::number(move1)+"/"+QString::number(fullmove));
    if(ishidden)hide();
    else show();
}

CharacterSelection::CharacterSelection(QWidget* parent):
    QLabel(parent)
{
    layout = new QVBoxLayout;
    moveButton = new QPushButton;
    attrackButton = new QPushButton;
    skipButton = new QPushButton;
    setFixedSize(90,90);
    moveButton->setText("移动");
    attrackButton->setText("攻击");
    skipButton->setText("跳过");
    layout->addWidget(moveButton);
    layout->addWidget(attrackButton);
    layout->addWidget(skipButton);
    layout->setSpacing(0);
    setLayout(layout);
}
void CharacterSelection::updateData(int localCellx, int localCelly)
{
    bool ishidden = isHidden();
    move((localCellx-1)*CELL_SIZE+64,(localCelly-1)*CELL_SIZE-10);
    if(ishidden)hide(); else
    {
        show();
        raise();
    }
}
