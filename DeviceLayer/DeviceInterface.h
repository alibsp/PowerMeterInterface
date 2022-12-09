#ifndef DEVICEINTERFACE_H
#define DEVICEINTERFACE_H

#include <QObject>
#include <QQueue>
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

struct CommandInfo
{
public :
    int registerNumber, value;
};



class DeviceInterface : public TCPClient
{
    Q_OBJECT
    Q_PROPERTY(quint16 radioNumber READ radioNumber WRITE setRadioNumber NOTIFY radioNumberChanged)
public:

    enum RequestType{PMRead, Pooling, Command};
    Q_ENUM(RequestType)

    struct RequestInfo
    {
    public:
        RequestType requestType;
        quint16 rtuNumber;
        quint8  registerNumber;
        quint16 registerValue;
    };


    explicit DeviceInterface(quint16 _radioNumber=1, QObject *parent = nullptr);
    //void connectToHost(const QHostAddress &address, quint16 port);

    void requetPMRead(quint8 rtuNumber);
    void requetPooling(quint8 rtuNumber);
    void requetCommand(quint16 rtuNumber, quint8 registerNumber, quint16 registerValue);

    quint16 calcCRC(QByteArray data);
    quint16 radioNumber() const;

private:
    QTimer clearBufferTimer;
    QTimer queueTimer;
    QQueue<RequestInfo> requestsQueue;
    quint16 m_radioNumber;
    QByteArray buffer;

    void clearBuffer();
    bool extractPMReadResponse(const QByteArray &data);
    bool extractPoolingResponse(const QByteArray &data);
    bool extractCommandResponse(const QByteArray &data);

    void sendPMRead(quint8 rtuNumber);
    void sendPooling(quint8 rtuNumber);
    void sendCommand(quint16 rtuNumber, quint8 registerNumber, quint16 registerValue);

    quint16 bytesToUint16HL(const QByteArray &bytes, int index);
    quint32 bytesToUint32HL(const QByteArray &bytes, int index);

    void appendToQueue(const RequestInfo &requestInfo);
private slots:
    void recievedData(const QByteArray &data);
    void checkQueue();

public slots:

    void setRadioNumber(quint16 radioNumber);
signals:
    void radioNumberChanged(quint16 radioNumber);
    //void recievedData(const quint16 &radioNumber, const QByteArray &data);
    void pmResultReady(const int &rtuNumber, const QList<PowerInfo> &powerInfos);
    void poolingResultReady(const int &rtuNumber,const StatusInfo &statusInfo);
    void commnadExecOk(const int &rtuNumber,const CommandInfo &commandInfo);

    // TCPClient interface
public slots:
    void stateChanged(QAbstractSocket::SocketState socketState);
    void disconnected();
    void connected();


};

#endif // DEVICEINTERFACE_H
