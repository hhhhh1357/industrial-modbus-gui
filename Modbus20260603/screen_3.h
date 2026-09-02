#ifndef SCREEN_3_H
#define SCREEN_3_H

#include <QWidget>
#include <QTimer>
#include "digitalkeyboardcpp.h"

class MainWindow;

namespace Ui {
class screen_3;
}

class screen_3 : public QWidget
{
    Q_OBJECT

public:
    explicit screen_3(QWidget *parent = nullptr);
    ~screen_3();
    Ui::screen_3 *ui;

signals:
    void screen3_readHoldingRegisters(int startAddr, int numRegs);
    void screen3_writeSingleRegister(int regAddr, quint16 value);

public slots:
    void modbusstate_chang(bool connected);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void screen3_timer_handler(void);
    void Keypressed_handle_Screen3(QString text,QWidget* focusedWidget);
    void on_comboBox_2_currentIndexChanged(int index);
private:

    MainWindow *my_mainwidget;
    QTimer *screen3_timer;
    DigitalKeyboardcpp *keyboarscreen3;
};

#endif // SCREEN_3_H
