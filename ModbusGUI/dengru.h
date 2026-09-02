#ifndef DENGRU_H
#define DENGRU_H

#include <QWidget>
#include "digitalkeyboardcpp.h"

class MainWindow;

namespace Ui {
class Dengru;
}

class Deng_Lv_Screen : public QWidget
{
    Q_OBJECT

public:
    explicit Deng_Lv_Screen(QWidget *parent = nullptr);
    ~Deng_Lv_Screen();
private slots:
    void Keypressed_handl_Dengru(QString text,QWidget* target);

    void on_ComboBox_currentIndexChanged(int index);

private:
    Ui::Dengru *ui;

    MainWindow *my_mainwidget;
    DigitalKeyboardcpp *keyboard_dengru;
};

#endif // DENGRU_H
