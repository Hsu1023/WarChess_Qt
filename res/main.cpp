#include "maindialog.h"
#include "config.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 适应中文
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);

    MainDialog w;
    w.show();
    return a.exec();
}
