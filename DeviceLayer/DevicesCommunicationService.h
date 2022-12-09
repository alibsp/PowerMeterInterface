#ifndef DEVICESCOMMUNICATIONSERVICE_H
#define DEVICESCOMMUNICATIONSERVICE_H

#include <QObject>
#include <QSettings>
#include "../DatabaseLayer/AsyncQuery.h"
#include "../DatabaseLayer/ConnectionManager.h"
#include "../DatabaseLayer/AsynqQueryModel.h"
#include "DeviceInterface.h"

class DevicesCommunicationService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool poolingEnable READ poolingEnable WRITE setPoolingEnable NOTIFY poolingEnableChanged)
    Q_PROPERTY(bool pmReadEnable  READ pmReadEnable WRITE setPmReadEnable NOTIFY pmReadEnableChanged)
    Q_PROPERTY(bool commandEnable READ commandEnable WRITE setCommandEnable NOTIFY commandEnableChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)


public:
    explicit DevicesCommunicationService(QObject *parent = nullptr);

    QHostAddress hostAddress() const;
    quint16 port() const;

    bool poolingEnable() const;
    bool pmReadEnable() const;
    bool commandEnable() const;

    int interval() const;

public slots:
    void pmResultReady(const int &rtuNumber, const QList<PowerInfo> &powerInfos);
    void poolingResultReady(const int &rtuNumber, const StatusInfo &statusInfo);
    void commnadExecOk(const int &rtuNumber, const CommandInfo &commandInfo);

    void eventLogSlot(const QString &log);

    void setPoolingEnable(bool poolingEnable);
    void setPmReadEnable(bool pmReadEnable);
    void setCommandEnable(bool commandEnable);

    void setInterval(int interval);

signals:
    void eventLog(const QString &log);
    void poolingEnableChanged(bool poolingEnable);
    void pmReadEnableChanged(bool pmReadEnable);
    void commandEnableChanged(bool commandEnable);

    void intervalChanged(int interval);

private:
    QTimer timer;
    QTimer timerPMRead;
    QTimer timerPooling;
    QTimer timerCommand;
    quint16 poolingInterval, pmReadInterval, commandInterval;
    int m_interval=200;

    DeviceInterface deviceInterface;
    bool m_poolingEnable=true;
    bool m_pmReadEnable=true;
    bool m_commandEnable=true;

    QList<int> rtuNumbers;
    void configDatabase();
    void loadRadioDeviceInfo();
    void loadRTUDevicesInfo();


    void radioDeviceLoaded();

private slots:

    //void timerTick();
    void pmReadTick();
    void poolingTick();
    void commandsTick();

};

#endif // DEVICESCOMMUNICATIONSERVICE_H
