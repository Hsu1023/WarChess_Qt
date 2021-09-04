#include "characterlabel.h"

CharacterProperty::CharacterProperty(QString name, int fullhp, int fullmove, int attrack, QWidget *parent):
    QLabel(parent)
{
    setFixedSize(165, 150);
    setPixmap(QPixmap(":/pic/dlg.png"));

    nameLabel.setText(name);
    hpLabel1.setText("血量");
    hpLabel2.setText(QString::number(fullhp)+"/"+QString::number(fullhp));

    layout.addWidget(&nameLabel,0,0);
    layout.addWidget(&hpLabel1,1,0);
    layout.addWidget(&hpLabel2,1,1);
    layout.setVerticalSpacing(0);
    setLayout(&layout);

    //show();
}
void CharacterProperty::updateData(int hp, int fullhp, int move, int fullmove, int localx, int localy)
{
    setGeometry(localx,localy,165,150);
    hpLabel2.setText(QString::number(hp)+"/"+QString::number(fullhp));
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
