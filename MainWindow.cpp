#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QLabel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    deviceService = new DevicesCommunicationService() ;
    connect(deviceService, &DevicesCommunicationService::eventLog, this, &MainWindow::onEventLog);
    ui->setupUi(this);

    sldInterval = new QSlider(Qt::Horizontal, this);
    connect(sldInterval, &QSlider::valueChanged, this, &MainWindow::on_sldInterval_valueChanged);
    sldInterval->setTickPosition(QSlider::TicksBelow);

    sldInterval->setRange(200, 2000);
    sldInterval->setTickInterval(200);

    int count = (sldInterval->maximum()-sldInterval->minimum())/sldInterval->tickInterval()+1;
    ui->grlSlider->addWidget(sldInterval, 0, 0, 1, count);
    for (auto i=0;i<count;i++)
    {
        QLabel *label = new QLabel(QString("%1").arg(i*sldInterval->tickInterval()+sldInterval->minimum()), this);
        QSpacerItem *item = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
        if(i<count-1)
            ui->grlSlider->addItem(item, 1, i, 1, 1);
        ui->grlSlider->addWidget(label, 1, i, 1, 1);
    }
    sldInterval->setValue(deviceService->interval());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onEventLog(QString log)
{
    ui->pteLog->appendPlainText(log);
}


void MainWindow::on_chbCommand_stateChanged(int arg1)
{
    deviceService->setCommandEnable(ui->chbCommand->isChecked());
}

void MainWindow::on_chbPMRead_stateChanged(int arg1)
{
    deviceService->setPmReadEnable(ui->chbPMRead->isChecked());
}

void MainWindow::on_chbPooling_stateChanged(int arg1)
{
    deviceService->setPoolingEnable(ui->chbPooling->isChecked());
}

void MainWindow::on_sldInterval_valueChanged(int value)
{
    deviceService->setInterval(value);
}
