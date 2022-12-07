#ifndef DEVICEINTERFACE_H
#define DEVICEINTERFACE_H

#include <QObject>
#include "../NetworkLayer/TcpClient.h"
#include "Tools.h"

struct PowerInfo
{
public :
    QString serialNumber;
    double tarfeha[4], cosFi, volatge, amper;
    int year, month, day, hour, minute;
};

struct StatusInfo
{
public :
    int generalStatus, tempStatus, doorStatus;
    int overCurrents[4];
    int statuses[4];
    int relays[4];
};



class DeviceInterface : public TCPClient
{
    Q_OBJECT
    Q_PROPERTY(quint16 radioNumber READ radioNumber WRITE setRadioNumber NOTIFY radioNumberChanged)
    quint16 m_radioNumber;
    QByteArray buffer;
public:
    explicit DeviceInterface(quint16 _radioNumber=1, QObject *parent = nullptr);
    //void connectToHost(const QHostAddress &address, quint16 port);
    void pmRead(quint8 rtuNumber);
    void sendPooling(quint8 rtuNumber);
    void sendCommand(quint16 rtuNumber, quint8 registerNumber, quint16 registerValue);
    quint16 calcCRC(QByteArray data);
    quint16 radioNumber() const;

private:
    QTimer *timer;
    void clearBuffer();
    bool extractPMReadResponse(const QByteArray &data);
    bool extractPoolingResponse(const QByteArray &data);
    bool extractCommandResponse(const QByteArray &data);
    quint16 bytesToUint16HL(const QByteArray &bytes, int index);
    quint32 bytesToUint32HL(const QByteArray &bytes, int index);
private slots:
    void recievedData(const QByteArray &data);

public slots:

    void setRadioNumber(quint16 radioNumber);
signals:
    void radioNumberChanged(quint16 radioNumber);
    //void recievedData(const quint16 &radioNumber, const QByteArray &data);
    void pmResultReady(const int &rtuNumber, const QList<PowerInfo> &powerInfos);
    void poolingResultReady(const int &rtuNumber,const StatusInfo &statusInfo);

    // TCPClient interface
public slots:
    void stateChanged(QAbstractSocket::SocketState socketState);
    void disconnected();
    void connected();


};

#endif // DEVICEINTERFACE_H
