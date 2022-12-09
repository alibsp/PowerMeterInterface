#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

class TCPClientWorker : public QTcpSocket
{
    Q_OBJECT
    QTimer*             m_timer;

private:
    QHostAddress m_hostAddress;
    quint16 m_port = 0;
    void tryConnect();

private slots:
    void readyReadSlot();
public slots:
    void init();
    void tryConnectToHost(const QHostAddress &address, quint16 port);
    void sendData(const QByteArray &data);
signals:
    void recievedData(const QByteArray &data);
    void eventLog(const QString &log);
};

//------------------------------------------------------------------
class TCPClient : public QObject
{
    Q_OBJECT
    QThread workerThread;
    Q_PROPERTY(QAbstractSocket::SocketState state READ state())
    Q_PROPERTY(QHostAddress hostAddress READ hostAddress WRITE setHostAddress NOTIFY hostAddressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)

    QAbstractSocket::SocketState m_state = QAbstractSocket::UnconnectedState;
    QHostAddress m_hostAddress;
    quint16 m_port = 0;

public:
    explicit TCPClient(QObject *parent = nullptr);
    ~TCPClient()
    {
        workerThread.quit();
        workerThread.wait();
    }
    QAbstractSocket::SocketState state() const;
    QHostAddress hostAddress(){return m_hostAddress;}
    quint16 port(){return m_port;}

public slots:
    void setHostAddress(QHostAddress hostAddress);

    void setPort(quint16 port);

private slots:
    void setState(QAbstractSocket::SocketState socketState);
    void setHostAndPort(const QHostAddress &address, quint16 port);

signals:
    virtual void recievedData(const QByteArray &data);
    virtual void connected();
    virtual void disconnected();
    virtual void stateChanged(QAbstractSocket::SocketState socketState);
    virtual void error(QAbstractSocket::SocketError socketError);
    void tryConnectToHost(const QHostAddress &address, quint16 port);
    void connectToHost(const QHostAddress &address, quint16 port);



    void initWorker();
    quint64 sendData(const QByteArray &data);
    void eventLog(const QString &log);
    void hostAddressChanged(QHostAddress hostAddress);
    void portChanged(quint16 port);
};



#endif // TCPCLIENT_H
