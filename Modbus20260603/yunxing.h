#ifndef YUNXING_H
#define YUNXING_H

#include <QWidget>
#include<QString>
#include<QTableWidget>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include "digitalkeyboardcpp.h"
#include "ui_yunxing.h"

class MainWindow;

class Yun_Xing_Screen : public QWidget
{
    Q_OBJECT

public:
    explicit Yun_Xing_Screen(QWidget *parent = nullptr);
    ~Yun_Xing_Screen();
     Ui::yunxing *ui;
     QString formula;

     QLineEdit *LC_line1;
     QVector<QLabel*> LC_Label1;
     QVector<QPushButton*> LC_PushButton1;

     QLineEdit *LC_line2;
     QVector<QLabel*> LC_Label2;
     QVector<QPushButton*> LC_PushButton2;

     QLineEdit *LC_line3;
     QVector<QLabel*> LC_Label3;
     QVector<QPushButton*> LC_PushButton3;

     QVector<QLabel*> LC_Label5;
     QVector<QPushButton*> LC_PushButton5;
signals:
    void WriteHoldFloatRegisters_YunXing(int regAddr, float value);
    void WriteHoldRegisters_YunXing(int regAddr, uint16_t value);
    void WriteSingleCoil_YunXing(int coilAddr, bool value);
    void WriteMultipleCoils_YunXing(int startAddr, const QVector<bool> &values);
private slots:

    void on_listWidget_currentRowChanged(int currentRow);
    void setAllCellsCentered(QTableWidget *table);
    void Keypressed_handle_YunXing(QString text,QWidget* focusedWidget);
    void LC_PushButton10Handle();
    void LC_PushButton11Handle();
    void LC_PushButton12Handle();
    void LC_PushButton13Handle();
    void LC_PushButton14Handle();

    void LC_PushButton50Handle();
    void LC_PushButton51Handle();
    void LC_PushButton52Handle();

    void on_pushButton_8_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_57_clicked();

    void on_pushButton_58_clicked();

    void on_pushButton_59_clicked();

    void on_pushButton_95_clicked();

    void on_pushButton_96_clicked();

    void on_pushButton_98_clicked();

    void on_pushButton_97_clicked();

    void on_pushButton_99_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_103_clicked();

    void on_pushButton_101_clicked();

    void on_pushButton_104_clicked();

    void on_pushButton_105_clicked();

    void on_pushButton_111_clicked();

    void on_pushButton_109_clicked();

    void on_pushButton_112_clicked();

    void on_pushButton_117_clicked();

    void on_pushButton_119_clicked();

    void on_pushButton_106_clicked();

    void on_pushButton_102_clicked();

    void on_pushButton_122_clicked();

    void on_pushButton_118_clicked();

    void on_pushButton_107_clicked();

    void on_pushButton_113_clicked();

    void on_pushButton_121_clicked();

    void on_pushButton_114_clicked();

    void on_pushButton_123_clicked();

    void on_pushButton_108_clicked();

    void on_pushButton_115_clicked();

    void on_pushButton_100_clicked();

    void on_pushButton_120_clicked();

    void on_pushButton_110_clicked();

    void on_pushButton_116_clicked();

    void on_pushButton_128_clicked();

    void on_pushButton_129_clicked();

    void on_pushButton_130_clicked();

    void on_pushButton_132_clicked();

    void on_pushButton_131_clicked();

    void on_pushButton_94_clicked();


    void on_pushButton_60_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_12_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_10_clicked();

    void on_pushButton_124_clicked();

    void on_pushButton_127_clicked();

    void on_pushButton_125_clicked();

    void on_pushButton_126_clicked();

    void on_pushButton_134_clicked();

    void on_pushButton_135_clicked();

    void on_pushButton_133_clicked();

private:
    DigitalKeyboardcpp *keyboard_yunxing;
    MainWindow *my_mainwidget;

    void setTabletKongJian(QLineEdit *&line, QVector<QLabel *>& labellist, QVector<QPushButton *>& pushbuttonlist, QTableWidget *tab);
    void SetGongZuoQuXianTab();
};

#endif // YUNXING_H
