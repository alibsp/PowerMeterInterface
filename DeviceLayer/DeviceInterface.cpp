#include "DeviceInterface.h"
#include <QtConcurrent/QtConcurrent>
DeviceInterface::DeviceInterface(quint16 _radioNumber, QObject *parent) :
    TCPClient(parent),
    m_radioNumber(_radioNumber)
{
    //connect(this, &DeviceInterface::connectToHost, &tcpClient, &TCPClient::connectToHost);
    //connect(&tcpClient, &TCPClient::recievedData, this, &DeviceInterface::onRecievedData);
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &DeviceInterface::clearBuffer);
    timer->setSingleShot(true);
}

/*void DeviceInterface::connectToHost(const QHostAddress &address, quint16 port)
                    {
                        tcpClient.tryConnectToHost(address, port);
                    }*/

quint16 DeviceInterface::calcCRC(QByteArray data)
{
    quint16 crc=0xFFFF;
    for (int i=0;i<data.length();i++)
    {
        quint16 crc_b=data[i]&0xFF;
        crc=crc ^ crc_b;
        for (int j = 0; j<8;j++)
        {
            quint16 temp=crc & 1;
            if (temp==1)
            {
                crc = crc / 2;
                crc = crc ^ 0xA001;
            }
            else
            {
                crc = crc / 2;
            }
        }
    }
    return crc;
}

quint16 DeviceInterface::radioNumber() const
{
    return m_radioNumber;
}

void DeviceInterface::clearBuffer()
{
    buffer.clear();
    qDebug()<<"Buffer Cleared";

}

quint16 DeviceInterface::bytesToUint16HL(const QByteArray &bytes, int index)
{
    quint16 res=0;
    if(bytes.length()>index+1)
        res = ((quint16)buffer[index]<<8) + (buffer[index+1]&0xFF);
    return res;
}

quint32 DeviceInterface::bytesToUint32HL(const QByteArray &bytes, int index)
{
    quint32 res= ((quint32)bytesToUint16HL(bytes, index)<<16) + (bytesToUint16HL(bytes, index+2)&0xFFFF);
    return res;
}
bool DeviceInterface::extractPMReadResponse(const QByteArray &data)
{
    QString log="";
    if(data.length()!=205)
    {
        log=QString(Tools::jalaliNow()+", Error to extractPMReadResponse");
        emit eventLog(log);
        return false;
    }

    QList<PowerInfo> powerInfos;
    int rtuNumber=0;
    rtuNumber = bytesToUint16HL(data, 3);
    log=QString(Tools::jalaliNow()+",PMResults, RTU:%1").arg(rtuNumber);
    for (int i=0, k=1;i<160;i+=40, k++)
    {
        log+=QString(", Kontor:%1").arg(k);
        PowerInfo  powerInfo;
        for (int j=0;j<4;j++)
        {
            powerInfo.tarfeha[j] = bytesToUint32HL(data, i + j*4 + 51) / 100.0;
            log += QString(", Tarefe%1:%2").arg(j+1).arg(powerInfo.tarfeha[j], 0, 'f', 2);
        }
        powerInfo.cosFi     = bytesToUint16HL(data, i+67) / 100.0;
        powerInfo.volatge	= bytesToUint16HL(data, i+69) / 100.0;
        powerInfo.amper 	= bytesToUint16HL(data, i+71) / 100.0;
        powerInfos.append(powerInfo);

        log += QString(", CosFi:%2").arg(powerInfo.cosFi, 0, 'f', 2);
        log += QString(", Volatge:%2").arg(powerInfo.volatge, 0, 'f', 2);
        log += QString(", Amper:%2").arg(powerInfo.amper, 0, 'f', 2);
    }

    emit eventLog(log);
    emit pmResultReady(rtuNumber, powerInfos);
    return true;
}

bool DeviceInterface::extractPoolingResponse(const QByteArray &data)
{
    QString log="";
    if(data.length()!=41)
    {
        log=QString(Tools::jalaliNow()+", Error to extractPoolingResponse");
        emit eventLog(log);
        return false;
    }

    StatusInfo statusInfo;
    int rtuNumber=0, k=3;
    rtuNumber = bytesToUint16HL(data, k);k+=2;
    log=QString(Tools::jalaliNow()+",PoolingResults, RTU:%1\r\n").arg(rtuNumber);


    statusInfo.generalStatus= bytesToUint16HL(data, k);k+=2;
    statusInfo.tempStatus   = bytesToUint16HL(data, k);k+=2;
    statusInfo.doorStatus   = bytesToUint16HL(data, k);k+=2;

    log += QString("General Status:%1\r\n").arg(statusInfo.generalStatus);
    log += QString("Temperture Status Level:%1\r\n").arg(statusInfo.tempStatus);
    log += QString("Door Status:%1\r\n").arg(statusInfo.doorStatus);



    for (int i=0;i<4;i++, k+=2)
    {
        statusInfo.overCurrents[i]  = bytesToUint16HL(data, k);
        statusInfo.statuses[i]      = bytesToUint16HL(data, k+8);
        statusInfo.relays[i]        = bytesToUint16HL(data, k+16);
    }
    for (int i=0;i<4;i++)
        log += QString("Over Currnet Error for P.M #%1:%2\r\n").arg(i+1).arg(statusInfo.overCurrents[i]);
    for (int i=0;i<4;i++)
        log += QString("Status for P.M #%1:%2\r\n").arg(i+1).arg(statusInfo.statuses[i]);
    for (int i=0;i<4;i++)
        log += QString("Relay Statu for P.M #%1:%2\r\n").arg(i+1).arg(statusInfo.relays[i]);


    emit eventLog(log);
    emit poolingResultReady(rtuNumber, statusInfo);
    return true;
}

bool DeviceInterface::extractCommandResponse(const QByteArray &data)
{

}


void DeviceInterface::stateChanged(QAbstractSocket::SocketState socketState)
{

}


void DeviceInterface::disconnected()
{

}

void DeviceInterface::connected()
{
    emit eventLog(QString("%1: Connected to %2:%3").arg(Tools::jalaliNow()).arg(hostAddress().toString()).arg(port()));
}

void DeviceInterface::recievedData(const QByteArray &data)
{
    QString log;
    QTextStream in(&log);
    in<<Tools::jalaliNow()<<", Recieved, Len:"<<data.length()<<", Data:"<<data.toHex(' ').toUpper();
    buffer.append(data);

    int len=buffer.length();
    if(buffer.length()>3)
    {
        quint16 crc = ((quint16)buffer[len-1]<<8) + (buffer[len-2]&0xFF);
        quint16 calcedCrc = calcCRC(buffer.left(len-2));
        if(crc==calcedCrc)
        {
            qDebug()<<"OK";
            //emit recievedData(m_radioNumber, buffer);
            switch ((int)buffer[1])
            {
            case 3:

                if(buffer.length()>100)
                    extractPMReadResponse(buffer);
                else
                    extractPoolingResponse(buffer);
                break;
            case 6:
                extractCommandResponse(buffer);
                break;
            }
            buffer.clear();
        }
    }
    timer->start(2000);
    if(buffer.length()>1000)
        buffer.clear();
    emit eventLog(log);
}

void DeviceInterface::setRadioNumber(quint16 radioNumber)
{
    if (m_radioNumber == radioNumber)
        return;

    m_radioNumber = radioNumber;
    emit radioNumberChanged(m_radioNumber);
}

void DeviceInterface::pmRead(quint8 rtuNumber)
{
    QByteArray data;
    data.append(rtuNumber);
    data.append(3);
    data.append((char)0);
    data.append(1);
    data.append((char)0);
    data.append(100);
    quint16 crc = calcCRC(data);
    data.append(crc);
    data.append(crc>>8);
    if(state()==QAbstractSocket::ConnectedState)
    {
        sendData(data);
        QString log;
        QTextStream in(&log);
        in<<Tools::jalaliNow()<<", Send PMRead, RTU:"<<rtuNumber<<", Data:"<<data.toHex(' ').toUpper();
        emit eventLog(log);
    }
}

void DeviceInterface::sendPooling(quint8 rtuNumber)
{
    QByteArray data;
    data.append(rtuNumber);
    data.append(3);
    data.append((char)0);
    data.append(1);
    data.append((char)0);
    data.append(18);
    quint16 crc = calcCRC(data);
    data.append(crc);
    data.append(crc>>8);
    if(state()==QAbstractSocket::ConnectedState)
    {
        sendData(data);
        QString log;
        QTextStream in(&log);
        in<<Tools::jalaliNow()<<", Send Pooling, RTU:"<<rtuNumber<<",Data:"<<data.toHex(' ').toUpper();
        emit eventLog(log);
    }
}

void DeviceInterface::sendCommand(quint16 rtuNumber, quint8 registerNumber, quint16 registerValue)
{
    QByteArray data;
    data.append(rtuNumber);
    data.append(6);
    data.append((char)0);
    data.append(registerNumber);
    data.append(registerValue>>8);
    data.append(registerValue);
    quint16 crc = calcCRC(data);
    data.append(crc);
    data.append(crc>>8);
    if(state()==QAbstractSocket::ConnectedState)
    {
        sendData(data);
        QString log;
        QTextStream in(&log);
        in<<Tools::jalaliNow()<<", Send Command,RTU:"<<rtuNumber<<"),Register:"<<registerNumber<<",Value:"<<registerValue<<","<<data.toHex(' ').toUpper();
        emit eventLog(log);
    }
}


