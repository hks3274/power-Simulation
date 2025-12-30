#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QIntValidator>
#include <deque>
#include <tuple>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void updateGraph(double last_time, std::deque<std::tuple<int, long long>> &dataDeque);

private slots:
    void updateData();
    void vReceived(int voltage);
    void sReceived(double time);

private:
    Ui::MainWindow *ui;
    QMdiArea *mdiarea; //메인 뷰
    QMdiSubWindow *inputSub;    //서브 윈도우 input(전압, 시간 입력)
    QMdiSubWindow *outputSub; // 서브 윈도우 output(출력, 그래프)
    std::deque<std::tuple<int, long long>> dataDeque; //depue (전압, 시간) 담을 그릇

    int last_voltage = 0; //초기 전압값
    double last_time = 0.1; //초기 시간값

    QTimer *timer;
};
#endif // MAINWINDOW_H
