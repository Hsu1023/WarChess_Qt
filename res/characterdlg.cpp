#include "characterdlg.h"
#include "config.h"

CharacterDlg::CharacterDlg(QWidget *parent) : QLabel(parent)
{
    setPixmap(QPixmap(":/pic/dlg.png"));
    QGridLayout *layout = new QGridLayout(this);
    QPushButton *button = new QPushButton;
    button->setText("12300");
    QLabel *label = new QLabel;
    label->setText("123");
    layout->addWidget(button , 0 , 0);
    layout -> addWidget(label , 1 ,0);
}
