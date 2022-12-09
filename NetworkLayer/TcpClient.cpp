#include "TcpClient.h"
#include "Tools.h"

//--------------------------------------------------------------------------
//
//              TCPClientWorker Class
//
//---------------------------------------------------------------------------


void TCPClientWorker::init()
{
    connect(this, &QTcpSocket::readyRead, this, &TCPClientWorker::readyReadSlot);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &TCPClientWorker::tryConnect);
}



void TCPClientWorker::tryConnect()
{
    QString log;
    QTextStream in(&log);


    //emit eventLog(log);
    //if(m_port!=0 && state() == QTcpSocket::UnconnectedState)
    if(m_port!=0 && state() != QTcpSocket::ConnectedState)
    {
        //in<<"Try to Connect "<<m_hostAddress.toString()<<":"<<m_port;//<<". state:"<<Tools::enumToString(state());
        in<<Tools::jalaliNow()<<", Try to connect "<<m_hostAddress.toString()<<":"<<m_port<<". state:"<<Tools::enumToString(state());
        emit eventLog(log);
        connectToHost(m_hostAddress, m_port);
    }
}


void TCPClientWorker::tryConnectToHost(const QHostAddress &address, quint16 port)
{
    m_hostAddress = address;
    m_port     = port;
    connectToHost(address, port);
    m_timer->start();
}

void TCPClientWorker::sendData(const QByteArray &data)
{
    if(state()==QTcpSocket::ConnectedState)
        write(data);
}


void TCPClientWorker::readyReadSlot()
{
    int maxlen=bytesAvailable();
    QByteArray buffer=read(maxlen);
    //qDebug()<<"recievedData:"<<buffer.length()<<buffer;
    if(buffer.length()>0)
        emit recievedData(buffer);
}

//--------------------------------------------------------------------------
//
//              TCPClient Class
//
//---------------------------------------------------------------------------
TCPClient::TCPClient(QObject *parent)
{
    Q_UNUSED(parent)
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QHostAddress>("QHostAddress");

    TCPClientWorker *tcpClientWorker = new TCPClientWorker();
    tcpClientWorker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, tcpClientWorker, &QObject::deleteLater);

    connect(this, &TCPClient::initWorker,       tcpClientWorker, &TCPClientWorker::init);
    connect(this, &TCPClient::tryConnectToHost, tcpClientWorker, &TCPClientWorker::tryConnectToHost);
    connect(this, &TCPClient::tryConnectToHost, this, &TCPClient::setHostAndPort);

    //connect(this, &TCPClient::connectToHost, tcpClientWorker, &TCPClientWorker::connectToHost);
    connect(this, &TCPClient::connectToHost, this, &TCPClient::setHostAndPort);
    connect(this, &TCPClient::sendData,         tcpClientWorker, &TCPClientWorker::sendData);


    connect(tcpClientWorker, &TCPClientWorker::recievedData, this, &TCPClient::recievedData);
    connect(tcpClientWorker, &TCPClientWorker::connected,    this, &TCPClient::connected);
    connect(tcpClientWorker, &TCPClientWorker::disconnected, this, &TCPClient::disconnected);
    connect(tcpClientWorker, &TCPClientWorker::stateChanged, this, &TCPClient::setState);
    connect(tcpClientWorker, &TCPClientWorker::eventLog,     this, &TCPClient::eventLog);
    connect(tcpClientWorker, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TCPClient::error);


    workerThread.start();
    emit initWorker();
}

QAbstractSocket::SocketState TCPClient::state() const
{
    return m_state;
}

void TCPClient::setHostAddress(QHostAddress hostAddress)
{
    if (m_hostAddress == hostAddress)
        return;

    m_hostAddress = hostAddress;
    emit hostAddressChanged(m_hostAddress);
}

void TCPClient::setPort(quint16 port)
{
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged(m_port);
}

void TCPClient::setState(QAbstractSocket::SocketState socketState)
{
    m_state = socketState;
    emit stateChanged(socketState);
}

void TCPClient::setHostAndPort(const QHostAddress &address, quint16 port)
{
    m_hostAddress = address;
    m_port = port;
}

