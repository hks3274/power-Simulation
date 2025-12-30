#include "mainwindow.h"

#include<QBoxLayout>

MainWindow::MainWindow(datamanagement& dataMng)
    : dataMng(dataMng)
{

    QWidget* central = new QWidget(this);
    QHBoxLayout* main_h_lay = new QHBoxLayout(central);

    inputWidget = new input_widget(dataMng);

    main_h_lay->addWidget(inputWidget);

    outputWidget = new output_widget(dataMng);

    main_h_lay->addWidget(outputWidget);
    outputWidget->show();

    setCentralWidget(central);
}

MainWindow::~MainWindow() {
}
