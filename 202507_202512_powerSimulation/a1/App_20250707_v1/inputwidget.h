#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H


#include <QLineEdit>


class inputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit inputWidget(QWidget *parent = nullptr);

signals:
    void vdataEntered(int voltage);
    void sdataEntered(double time);

private slots:
    void vInputPressed();
    void sInputPressed();

private:
    QLineEdit *vLineEdit;
    QLineEdit *sLineEdit;

};

#endif // INPUTWIDGET_H
