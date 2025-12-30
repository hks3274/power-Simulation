#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDateTime>
#include <Qtimer>
#include <QStringListModel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QEvent>
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timer(new QTimer(this))
{
    ui->setupUi(this);


    datalist = new QStringList();
    model = new QStringListModel(this);
    ui->output_view->setModel(model);

    connect(ui->input_btn, &QPushButton::clicked, this, [=]() {
        QString userText = ui->input_text->text();
        datalist->append(userText);
        model->setStringList(*datalist);
        ui->input_text->clear();
    });

    connect(ui->input_text, &QLineEdit::returnPressed, this, [=]() {
        QString userText = ui->input_text->text();
        datalist->append(userText);
        model->setStringList(*datalist);
        ui->input_text->clear();
    });


    // 타이머 설정
    timer->start(1000);  // 1초 간격

    // 슬롯 연결
    connect(timer, &QTimer::timeout, this, &MainWindow::updateDateTime);

    // 처음 한 번 표시
    updateDateTime();
}


void MainWindow::updateDateTime()
{
    QDateTime real_timedate = QDateTime::currentDateTime();
    ui->datetime_view->setText(real_timedate.toString("yyyy-MM-dd hh:mm:ss"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
