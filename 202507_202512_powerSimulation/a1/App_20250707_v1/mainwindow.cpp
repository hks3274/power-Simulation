#include "mainwindow.h"
#include "inputWidget.h"
#include "inputslidewidget.h"
#include "outputWidget.h"
//#include "./ui_mainwindow.h"
#include <iostream>
#include <chrono> //C++에서 Time을 다루기 위해 제공되는 표준 라이브러리


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    resize(1280, 720);

    mdiarea = new QMdiArea(this);
    setCentralWidget(mdiarea);

    //데이터 입력
    inputSub = new QMdiSubWindow();
    inputWidget *iWidget = new inputWidget();
    // *iWidget = new inputSlideWidget();
    inputSub->setWidget(iWidget);
    inputSub->setGeometry(0,0,640,720);
    mdiarea->addSubWindow(inputSub);
    inputSub->show();

    //그래프 입력
    outputSub = new QMdiSubWindow();
    outputWidget *oWidget = new outputWidget();
    outputSub->setWidget(oWidget);
    outputSub->setGeometry(640,0,640,720);
    mdiarea->addSubWindow(outputSub);
    outputSub->show();

    mdiarea->addSubWindow(outputSub);
    outputSub->show();

    //타이머 실행
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateData); //타이머 실행시 updateData
    timer->start(last_time * 1000);

    // iWidget-> inputWidget에서 만약 vdataEnterd를 실행하면 메인윈도우의 vReceived를 실행
    connect(iWidget, &inputWidget::vdataEntered, this, &MainWindow::vReceived);
    connect(iWidget, &inputWidget::sdataEntered, this, &MainWindow::sReceived);
    //connect(iWidget, &inputSlideWidget::vdataEntered, this, &MainWindow::vReceived);
    //connect(iWidget, &inputSlideWidget::sdataEntered, this, &MainWindow::sReceived);


    connect(this, &MainWindow::updateGraph, oWidget, &outputWidget::updateGraph);

}



void MainWindow::vReceived (int voltage)
{
    last_voltage = voltage;
}
void MainWindow::sReceived (double time)
{

    last_time = time;

    timer->stop();
    timer->start(last_time * 1000);
}

void MainWindow::updateData(){
    int voltage = last_voltage;

    //std::cout << voltage << std::endl; //값 확인용

    //depue에 전압값과 현재시간(유니온 타임스탬프)을 입력

    //현재시간을 초 단위(double)로 캡처
    // std::chrono::system_clock -> 현재 시스템 시간을 의미
    // auto -> 알아서 변수의 타입을 정해줌 -> 복잡한 타입일 때 사용하면 좋음
    const auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();  // long long 타입 정수 반환


    // deque에 전압값과 현재 시간(유닉스 타임스탬프)를 넣는다
    dataDeque.push_back(std::make_tuple(voltage, millis));

    // ✅ 데이터가 100개 초과하면 가장 오래된 것 제거
    if (dataDeque.size() > 100) {
        dataDeque.pop_front();
    }

    emit updateGraph(last_time, dataDeque);
}

MainWindow::~MainWindow()
{
    //delete ui;
}
