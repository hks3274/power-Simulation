#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include "dataManagement.h"
#include "settingstorage.h"
#include<QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QMessageBox>

class settingDialog : public QDialog
{
public:
    settingDialog(settingStorage& storage, dataManagement& dataMng, QWidget* parent = nullptr);

private:
    settingStorage& settings;
    dataManagement& dataMng;

    QListWidget* listWidget;
    QLineEdit* nameEdit;
    QPushButton* saveButton;
    QPushButton* loadButton;
    QPushButton* delButton;
    QLabel* settingView;

    void currentValueView();

    void mousePressEvent(QMouseEvent* event);

};

#endif // SETTINGDIALOG_H
