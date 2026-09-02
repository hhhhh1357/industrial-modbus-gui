#ifndef DIGITALKEYBOARDCPP_H
#define DIGITALKEYBOARDCPP_H
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QLineEdit>

class DigitalKeyboardcpp : public QWidget
{
    Q_OBJECT
public:
    explicit DigitalKeyboardcpp(QWidget *parent = nullptr);
    void setTargetWidget(QWidget *target);
signals:
    void OkKeyPressed(QString text,QWidget* target);
private slots:
    void handleButtonClick(const QString &text);
    void onInputFocusChanged(QWidget *old, QWidget *now);
private:
    void createLayout();
    void createButton(const QString &text, int row, int col, int rowSpan = 1, int colSpan = 1);
     void setupFocusPolicy();

    QGridLayout *layout;
    QSignalMapper *signalMapper;
    QWidget *targetWidget;
    QLineEdit *key_lineedit;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // DIGITALKEYBOARDCPP_H
