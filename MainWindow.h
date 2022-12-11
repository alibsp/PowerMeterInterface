#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <DeviceLayer/DevicesCommunicationService.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSlider *sldInterval;
    DevicesCommunicationService *deviceService = nullptr;

public slots:
    void onEventLog(QString log);

private slots:
    void on_chbCommand_stateChanged(int arg1);
    void on_chbPMRead_stateChanged(int arg1);
    void on_chbPooling_stateChanged(int arg1);
    void on_sldInterval_valueChanged(int value);

    void on_btnPlay_triggered(QAction *arg1);
    void on_btnClear_triggered(QAction *arg1);
    void on_btnPlay_clicked();
    void on_btnClear_clicked();
};
#endif // MAINWINDOW_H
