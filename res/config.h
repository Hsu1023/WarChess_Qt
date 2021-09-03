#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include <QMouseEvent>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QTextCodec>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

// 主窗口参数
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 960
#define WINDOW_BOUNDARY 80

//
#define MAP_WIDTH 3200
#define MAP_HEIGHT 1920
#define CELL_SIZE 64

//
#define MOUSE_MOVE_TIMER_INTERVAL 100

#define HP_HEIGHT 8
#define HP_DISTANCE 10

#define MAP_PATH  "://pic/map1.png"
#define WARRIOR_PATH "://pic/warrior_0.png"



#endif // CONFIG_H
