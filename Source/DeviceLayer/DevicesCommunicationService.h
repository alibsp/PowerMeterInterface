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
    Q_PROPERTY(QHostAddress hostAddress READ hostAddress WRITE setHostAddress NOTIFY hostAddressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)


public:
    explicit DevicesCommunicationService(QObject *parent = nullptr);

    QHostAddress hostAddress() const;
    quint16 port() const;

public slots:
    void pmResultReady(const int &rtuNumber, const QList<PowerInfo> &powerInfos);
    void eventLogSlot(const QString &log);

    void setHostAddress(QHostAddress hostAddress);

    void setPort(quint16 port);

signals:
    void hostAddressChanged(QHostAddress hostAddress);
    void portChanged(quint16 port);
    void eventLog(const QString &log);

private:
    DeviceInterface deviceInterface;
    QHostAddress m_hostAddress;
    quint16 m_port;
    void configDatabase(QSettings &settings);
    void configDevice(QSettings &settings);
};

#endif // DEVICESCOMMUNICATIONSERVICE_H
