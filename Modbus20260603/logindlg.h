#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QWidget>
#include "digitalkeyboardcpp.h"
#include "sqlitemanager.h"
#include <QMainWindow>

class MainWindow;

namespace Ui {
class loginDlg;
}

class loginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit loginDlg(SqliteManager &dbManager, QWidget *parent = nullptr);
    ~loginDlg();
private slots:
    void KeyPressedLogInDig(QString text,QWidget* target);
    void on_pushButton_2_clicked();

private:
    Ui::loginDlg *ui;

    SqliteManager &m_dbManager;
    QWidget *m_parent;
    DigitalKeyboardcpp *keyboard;
    MainWindow *my_mainWin;
};

#endif // LOGINDLG_H
