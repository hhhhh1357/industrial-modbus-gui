#include "mainwindow.h"
#include <QApplication>
#include <QStackedWidget>
#include "logindlg.h"

int main(int argc, char *argv[])
{
#ifdef IMX6_PLATFORM
    system("chmod 777 /dev/ttyAS4");
#endif
    qRegisterMetaType<QVector<uint16_t>>("QVector<uint16_t>");
    qRegisterMetaType<QVector<uint8_t>>("QVector<uint8_t>");
    qRegisterMetaType<uint16_t>("uint16_t");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    a.setStyleSheet(
           "QWidget {"
           "    color: rgb(0,0,0);"  // 保留原有的文字颜色设置
           "}"
           "QLabel { font-size: 18px; }"    // 全局 label 样式
            "QPushButton { min-width: 30px; min-height: 35px; }"
            "QComboBox { min-width: 30px; min-height: 35px; }"
            "QLineEdit  { min-width: 30px; min-height: 30px; }"
           "QPushButton:focus {"
           "    outline: none;"      // 移除焦点框
           "}"
                
           "QPushButton:pressed {"
           "    border: none;"       // 移除按下时的边框
           "}"
           "QListWidget {"
           "    outline: none;"           // 移除整个列表的焦点框
           "}"
           "QListWidget:focus {"
           "    outline: none;"           // 聚焦状态下去除虚线框
           "}"
           "QListWidget::item {"
           "    text-align: center;"      // 文字水平居中
           "}"
       );
    MainWindow w;
    //system("echo 7 > /sys/devices/platform/backlight/backlight/backlight/brightness");
    w.show();
    return a.exec();
}
