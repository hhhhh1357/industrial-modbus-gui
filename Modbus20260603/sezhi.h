#ifndef SEZHI_H
#define SEZHI_H

#include <QWidget>
#include "digitalkeyboardcpp.h"
#include <QTableWidget>
#include "ui_sezhi.h"
class MainWindow;

namespace Ui {
class SeZhi;
}

class Se_Zhi_Screen : public QWidget
{
    Q_OBJECT

public:
    explicit Se_Zhi_Screen(QWidget *parent = nullptr);
    ~Se_Zhi_Screen();
    Ui::SeZhi *ui;
    QVector<QVector<QLineEdit*>> SinJiTiJi_line;

signals:
    void WriteHoldRegisters_SeZhi(int regAddr, uint16_t value);
    void WriteHoldFloatRegisters_SeZhi(int regAddr, float value);
    void ReadHoldRegisters_SeZhi(int regAddr, uint16_t value);
    void ReadInputRegisters_SeZhi(int regAddr, uint16_t value);
    void WriteMultipleHoldRegisters_SeZhi(int startAddr, const QVector<uint16_t> &values);

private slots:
    void Keypressed_handl_SeZhi(QString text,QWidget* focusedWidget);
    void on_pushButton_clicked();
    void on_listWidget_currentRowChanged(int currentRow);
    void on_pushButton_3_clicked();

private:

    DigitalKeyboardcpp *keyboard_sezhi;
    MainWindow *my_mainwidget;
    bool setSystemTimeWithDate(const QDateTime &dt);
};

#endif // SEZHI_H
