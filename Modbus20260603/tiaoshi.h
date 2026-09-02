#ifndef TIAOSHI_H
#define TIAOSHI_H

#include <QWidget>
#include "digitalkeyboardcpp.h"
#include "ui_tiaoshi.h"

namespace Ui {
class tiaoshi;
}

class MainWindow;

class Tiao_Shi_Screen : public QWidget
{
    Q_OBJECT

public:
    explicit Tiao_Shi_Screen(QWidget *parent = nullptr);
    ~Tiao_Shi_Screen();
    Ui::tiaoshi *ui;

signals:
    void WriteSingleCoil_TiaoShi(int coilAddr, bool value);
    void WriteHoldRegisters_TiaoShi(int regAddr, uint16_t value);
    void ReadInputRegisters_TiaoShi(int regAddr, uint16_t value);
    void ReadHoldRegisters_TiaoShi(int regAddr, uint16_t value);
    void WriteHoldFloatRegisters_TiaoShi(int regAddr, float value);

private slots:
    void on_listWidget_currentRowChanged(int currentRow);
    void on_listWidget_2_currentRowChanged(int currentRow);
    void tiaoshi_Keypressed_handle(QString text,QWidget* focusedWidget);
    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();


    void on_pushButton_23_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_35_clicked();

    void on_pushButton_36_clicked();

    void on_pushButton_37_clicked();

    void on_pushButton_38_clicked();

    void on_pushButton_42_clicked();

    void on_pushButton_44_clicked();

    void on_pushButton_46_clicked();

    void on_pushButton_48_clicked();

private:
    DigitalKeyboardcpp *keyboard_tiaoshi;
    MainWindow *my_mainwidget;
};

#endif // TIAOSHI_H
