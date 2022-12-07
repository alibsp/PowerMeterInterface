#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    deviceService = new DevicesCommunicationService() ;
    connect(deviceService, &DevicesCommunicationService::eventLog, this, &MainWindow::onEventLog);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onEventLog(QString log)
{
    ui->pteLog->appendPlainText(log);
}

