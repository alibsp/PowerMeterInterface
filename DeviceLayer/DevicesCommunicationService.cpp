#include "DevicesCommunicationService.h"

#include <qapplication.h>

#include <DatabaseLayer/ConnectionManager.h>
using namespace Database;

DevicesCommunicationService::DevicesCommunicationService(QObject *parent) : QObject(parent)
{
    QString log = Tools::jalaliNow()+", DevicesCommunicationService start";
    qDebug()<<log;
    //QSettings settings( QApplication::applicationDirPath()+"\\Settings.ini", QSettings::IniFormat);
    configDatabase();
}



void DevicesCommunicationService::configDatabase()
{
    QSettings m("HKEY_CURRENT_USER\\SOFTWARE\\CCRF", QSettings::Registry64Format);

    /*QString databaseAddress     = settings.value("DatabaseAddress"  , ".\\2017").toString();
    QString databaseName        = settings.value("DatabaseName"     , "EmginDB").toString();
    QString trustedConnection   = settings.value("TrustedConnection"  , "yes").toString();
    QString databseUsername     = settings.value("DatabseUsername"  , "sa").toString();
    QString databsePassword     = settings.value("DatabsePassword"  , "123456").toString();*/
    QString databaseAddress     = m.value("Server"  , ".\\SQL2017").toString();
    QString databaseName        = m.value("Database"     , "EmginDB").toString();
    QString trustedConnection   = m.value("TrustedConnection"  , "Yes").toString();
    QString databseUsername     = m.value("loginName"  , "sa").toString();
    QString databsePassword     = m.value("Password"  , "123456").toString();


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
    else
    {
        loadRadioDeviceInfo();
        loadRTUDevicesInfo();
    }
}
void DevicesCommunicationService::loadRadioDeviceInfo()
{
    try
    {
        AsyncQuery *query = new AsyncQuery();
        QObject::connect (query, &AsyncQuery::execDone, this, [=](const AsyncQueryResult &result)
        {
            if (!result.isValid()) {
                eventLog(QString("%1, SqlError: %3").arg(Tools::jalaliNow()).arg(result.error().text()));
            } else {
                for (int row = 0; row < result.count(); row++)
                {
                    QString host    = result.value(row, "RadioIPAddress").toString();
                    poolingInterval     = result.value(row, "RTURead").toInt();
                    pmReadInterval      = result.value(row, "CounterRead").toInt();
                    commandInterval     = result.value(row, "Command").toInt();

                    connect(&deviceInterface, &DeviceInterface::eventLog, this, &DevicesCommunicationService::eventLog);
                    connect(&deviceInterface, &DeviceInterface::eventLog, this, &DevicesCommunicationService::eventLogSlot);
                    connect(&deviceInterface, &DeviceInterface::pmResultReady, this, &DevicesCommunicationService::pmResultReady);
                    connect(&deviceInterface, &DeviceInterface::poolingResultReady, this, &DevicesCommunicationService::poolingResultReady);
                    connect(&deviceInterface, &DeviceInterface::commnadExecOk, this, &DevicesCommunicationService::commnadExecOk);

                    connect(this, &DevicesCommunicationService::runChanged, &deviceInterface, &DeviceInterface::setRun);

                    deviceInterface.tryConnectToHost(QHostAddress(host), 5000);

                    /*connect(&timer, &QTimer::timeout, this, &DevicesCommunicationService::timerTick);
                    timer.start(m_interval);*/

                    connect(&timerPooling, &QTimer::timeout, this, &DevicesCommunicationService::poolingTick);
                    connect(&timerPMRead, &QTimer::timeout, this, &DevicesCommunicationService::pmReadTick);
                    connect(&timerCommand, &QTimer::timeout, this, &DevicesCommunicationService::commandsTick);

                    timerPooling.start(poolingInterval * m_interval);
                    timerPMRead .start(pmReadInterval  * m_interval);
                    timerCommand.start(commandInterval * m_interval);
                    /*timerPooling.start(m_interval*1000);
                    timerPMRead .start(m_interval*1000);
                    timerCommand.start(5000);*/
                    break;
                }
            }
        });
        query->startExec("SELECT * FROM dbo.SystemSetting");
    } catch (int ab)
    {

    }
}

void DevicesCommunicationService::loadRTUDevicesInfo()
{
    try
    {
        AsyncQuery *query = new AsyncQuery();
        QObject::connect (query, &AsyncQuery::execDone, [=](const AsyncQueryResult &result)
        {
            if (!result.isValid()) {
                eventLog(QString("%1, SqlError: %3").arg(Tools::jalaliNow()).arg(result.error().text()));
            } else {
                for (int row = 0; row < result.count(); row++)
                    rtuNumbers.append(result.value(row, "RTUNo").toInt());
            }
        });
        query->startExec("Select distinct RTUNo from Devices Order By RTUNo");
    } catch (int ab)
    {

    }

}

/*void DevicesCommunicationService::timerTick()
{

}*/

bool DevicesCommunicationService::poolingEnable() const
{
    return m_poolingEnable;
}

bool DevicesCommunicationService::pmReadEnable() const
{
    return m_pmReadEnable;
}

bool DevicesCommunicationService::commandEnable() const
{
    return m_commandEnable;
}

int DevicesCommunicationService::interval() const
{
    return m_interval;
}

bool DevicesCommunicationService::run() const
{
    return m_run;
}



void DevicesCommunicationService::pmReadTick()
{
    if(!m_pmReadEnable || rtuNumbers.count()==0 || !m_run)
        return;
    for (int i = 0; i < rtuNumbers.length(); ++i)
        deviceInterface.requetPMRead(rtuNumbers[i]);
    if(pmReadInterval*m_interval<rtuNumbers.length()*2*2000)
        timerPMRead.start(rtuNumbers.length()*2*2000);
}

void DevicesCommunicationService::poolingTick()
{
    if(!m_poolingEnable || rtuNumbers.count()==0 || !m_run)
        return;
    for (int i = 0; i < rtuNumbers.length(); ++i)
        deviceInterface.requetPooling(rtuNumbers[i]);
    if(poolingInterval*m_interval<rtuNumbers.length()*2*2)
        timerPooling.start(rtuNumbers.length()*2*2000);
}

void DevicesCommunicationService::requestMonitor(int mrId, quint8 rtuNumber)
{
    deviceInterface.requetPMRead(rtuNumber, true);
    AsyncQuery *query = new AsyncQuery();
    query->bindValue(":MRId", mrId);
    query->prepare("UPDATE MonitorRequest set Done=Done+1 WHERE MRId=:MRId");
    query->startExec();

}

void DevicesCommunicationService::commandsTick()
{
    if(!m_commandEnable || rtuNumbers.count()==0 || !m_run)
        return;
    if(commandInterval*m_interval<rtuNumbers.length()*2*2)
        timerCommand.start(rtuNumbers.length()*2*2000);
    try
    {
        AsyncQuery *query = new AsyncQuery();
        QObject::connect (query, &AsyncQuery::execDone, [=](const AsyncQueryResult &result)
        {
            if (!result.isValid()) {
                eventLog(QString("%1, SqlError: %3").arg(Tools::jalaliNow()).arg(result.error().text()));
            } else {
                for (int row = 0; row < result.count(); row++)
                {
                    quint16 rtuNumber       = result.value(row, "RTUNo").toInt();
                    quint8  registerNumber  = result.value(row, "RegisterNumber").toInt();
                    quint16 registerValue   = result.value(row, "Value").toInt();
                    deviceInterface.requetCommand(rtuNumber, registerNumber, registerValue);
                }
            }
        });
        query->startExec("select RTUNo, RegisterNumber, Value  from Command "
                         " Where Executed=0 "
                         " and Datediff(MINUTE,CDate,getdate())<=5");
    } catch (int ab)
    {

    }

    try
    {
        AsyncQuery *query = new AsyncQuery();
        QObject::connect (query, &AsyncQuery::execDone, [=](const AsyncQueryResult &result)
        {
            if (!result.isValid()) {
                eventLog(QString("%1, SqlError: %3").arg(Tools::jalaliNow()).arg(result.error().text()));
            } else {
                for (int row = 0; row < result.count(); row++)
                {
                    quint16 mrid            = result.value(row, "MRId").toInt();
                    quint16 rtuNumber       = result.value(row, "RTUNo").toInt();
                    requestMonitor(mrid, rtuNumber);
                }
            }
        });
        query->startExec("select MRId, RTUNo from MonitorRequest "
                         " Where Executed=0 AND Done<3"
                         " and Datediff(MINUTE,RTime,getdate())<=5");
    } catch (int ab)
    {

    }
}


void DevicesCommunicationService::pmResultReady(const int &rtuNumber, const QList<PowerInfo> &powerInfos)
{
    try
    {
        for (int i = 0; i < powerInfos.count(); ++i)
        {
            AsyncQuery *query = new AsyncQuery();
            QObject::connect (query, &AsyncQuery::execDone, [=](const AsyncQueryResult &result)
            {
                if (!result.isValid())
                    eventLog(QString("%1, SqlError: %3").arg(Tools::jalaliNow()).arg(result.error().text()));
                 else
                    for (int row = 0; row < result.count(); row++)
                        eventLog(Tools::jalaliNow()+", SP_InsertCounterPower exex result:"+result.value(row, 0).toString());
            });
            query->prepare("{CALL dbo.SP_InsertCounterPower (:RTUNo, :Serial_number, :P1, :P2, :P3, :P4, :Cosfi, :Voltage, :Amper)}");
            query->bindValue(":RTUNo", rtuNumber);
            query->bindValue(":Serial_number", powerInfos[i].serialNumber);
            for (int j = 0; j < 4; ++j)
                query->bindValue(QString(":P%1").arg(j+1), powerInfos[i].tarfeha[j]);
            query->bindValue(":Cosfi"  , powerInfos[i].cosFi);
            query->bindValue(":Voltage", powerInfos[i].volatge);
            query->bindValue(":Amper"  , powerInfos[i].amper);
            //query->bindValue("@CDate"  , QString("%1/%2/%3").arg(powerInfos[i].year).arg(powerInfos[i].month).arg(powerInfos[i].day));
            //query->bindValue("@CTime"  , QString("%1:%2").arg(powerInfos[i].hour).arg(powerInfos[i].minute));
            query->startExec();
        }

    } catch (int ab)
    {

    }
}

void DevicesCommunicationService::poolingResultReady(const int &rtuNumber, const StatusInfo &statusInfo)
{

}

void DevicesCommunicationService::commnadExecOk(const int &rtuNumber, const CommandInfo &commandInfo)
{

    try
    {
        AsyncQuery *query = new AsyncQuery();
        QObject::connect (query, &AsyncQuery::execDone, [=](const AsyncQueryResult &result)
        {
            if (!result.isValid())
                eventLog(QString("%1, SqlError: %3").arg(Tools::jalaliNow()).arg(result.error().text()));
             else
                for (int row = 0; row < result.count(); row++)
                    eventLog(Tools::jalaliNow()+", SP_UpdateCommand exec ok, result:"+result.value(row, 0).toString());
        });
        query->bindValue(":RTUNo", rtuNumber);
        query->bindValue(":RegisterNumber", commandInfo.registerNumber);
        query->bindValue(":value", commandInfo.value);
        query->prepare("{CALL SP_UpdateCommand (:RTUNo, :RegisterNumber, :value)}");
        query->startExec();
    } catch (int ab)
    {

    }
}

void DevicesCommunicationService::eventLogSlot(const QString &log)
{
    qDebug()<<log;
}


void DevicesCommunicationService::setPoolingEnable(bool poolingEnable)
{
    if (m_poolingEnable == poolingEnable)
        return;

    m_poolingEnable = poolingEnable;
    emit poolingEnableChanged(m_poolingEnable);
}

void DevicesCommunicationService::setPmReadEnable(bool pmReadEnable)
{
    if (m_pmReadEnable == pmReadEnable)
        return;

    m_pmReadEnable = pmReadEnable;
    emit pmReadEnableChanged(m_pmReadEnable);
}

void DevicesCommunicationService::setCommandEnable(bool commandEnable)
{
    if (m_commandEnable == commandEnable)
        return;

    m_commandEnable = commandEnable;
    emit commandEnableChanged(m_commandEnable);
}

void DevicesCommunicationService::setInterval(int interval)
{
    if (m_interval == interval)
        return;

    m_interval = interval;
    timerPooling.setInterval(poolingInterval * m_interval);
    timerPMRead. setInterval(pmReadInterval   * m_interval);
    timerCommand.setInterval(commandInterval * m_interval);
    emit intervalChanged(m_interval);}

void DevicesCommunicationService::setRun(bool run)
{
    if (m_run == run)
        return;

    m_run = run;
    emit runChanged(m_run);
}
