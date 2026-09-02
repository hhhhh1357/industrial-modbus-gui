#ifndef SCREEN_1_H
#define SCREEN_1_H

#include <QWidget>
#include "ui_zhuye.h"

class MainWindow;

namespace Ui {
class screen_1;
}

class Zhu_Ye_Screen : public QWidget
{
    Q_OBJECT

public:
    explicit Zhu_Ye_Screen(QWidget *parent = nullptr);
    ~Zhu_Ye_Screen();
    Ui::screen_1 *ui;

signals:
    void WriteHoldRegisters_ZhuYe(int regAddr, uint16_t value);

private slots:
    void on_pushButton_clicked();

private:

    MainWindow *my_mainwidget;
};

#endif // SCREEN_1_H
