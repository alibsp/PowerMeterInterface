#include "DevicesCommunicationService.h"

#include <qapplication.h>

#include <DatabaseLayer/ConnectionManager.h>
using namespace Database;

DevicesCommunicationService::DevicesCommunicationService(QObject *parent) : QObject(parent)
{
    qDebug()<<"DevicesCommunicationService start";
    QSettings settings( QApplication::applicationDirPath()+"\\Settings.ini", QSettings::IniFormat);
    configDatabase(settings);
    configDevice(settings);
}

QHostAddress DevicesCommunicationService::hostAddress() const
{
    return m_hostAddress;
}

quint16 DevicesCommunicationService::port() const
{
    return m_port;
}

void DevicesCommunicationService::configDevice(QSettings &settings)
{
    QString host = settings.value("Host", "192.168.1.100").toString();
    quint16 port = settings.value("Port", 5000).toUInt();
    connect(&deviceInterface, &DeviceInterface::eventLog, this, &DevicesCommunicationService::eventLog);
    connect(&deviceInterface, &DeviceInterface::eventLog, this, &DevicesCommunicationService::eventLogSlot);
    connect(&deviceInterface, &DeviceInterface::pmResultReady, this, &DevicesCommunicationService::pmResultReady);

    deviceInterface.pmRead(129);
    deviceInterface.tryConnectToHost(QHostAddress(host), port);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        deviceInterface.pmRead(129);
    });
    timer->start(5000);
}
void DevicesCommunicationService::configDatabase(QSettings &settings)
{
    QString databaseAddress     = settings.value("DatabaseAddress"  , ".\\2017").toString();
    QString databaseName        = settings.value("DatabaseName"     , "EmginDB").toString();
    QString trustedConnection   = settings.value("TrustedConnection"  , "yes").toString();
    QString databseUsername     = settings.value("DatabseUsername"  , "sa").toString();
    QString databsePassword     = settings.value("DatabsePassword"  , "123456").toString();


    ConnectionManager *databaseConnection = ConnectionManager::instance();

    databaseConnection->setType("QODBC");
    //Trusted_Connection=Yes;
    databaseConnection->setDatabaseName(QString("DRIVER={SQL Server Native Client 11.0};SERVER=lpc:%1;Database=%2;Trusted_Connection=%3;")
                                        .arg(databaseAddress)
                                        .arg(databaseName)
                                        .arg(trustedConnection));
    databaseConnection->setUserName(databseUsername);
    databaseConnection->setPassword(databsePassword);
    if(!databaseConnection->open())
        QApplication::quit();
}

void DevicesCommunicationService::pmResultReady(const int &rtuNumber, const QList<PowerInfo> &powerInfos)
{

}

void DevicesCommunicationService::eventLogSlot(const QString &log)
{
    qDebug()<<log;
}

void DevicesCommunicationService::setHostAddress(QHostAddress hostAddress)
{
    if (m_hostAddress == hostAddress)
        return;

    m_hostAddress = hostAddress;
    emit hostAddressChanged(m_hostAddress);
}

void DevicesCommunicationService::setPort(quint16 port)
{
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged(m_port);
}
