#include "websocketservermanger.h"
#include <QNetworkInterface>
#include <QFile>

WebSocketServerManger::WebSocketServerManger(QString serverName,
                                             QWebSocketServer::SslMode secureMode,
                                             QObject *parent)
    : QObject(parent),
      m_serverName(serverName),
      m_ssMode(secureMode),
      b_running(false),
      m_pWebSocketServer(0)
{
}


WebSocketServerManger::~WebSocketServerManger()
{
    if(m_pWebSocketServer != 0 )
    {
        m_pWebSocketServer->deleteLater();
        m_pWebSocketServer = 0;
    }
}

QString WebSocketServerManger::getLocalIp()
{
    QString localIp;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(int index = 0; index<list.size(); index++)
    {
        if(list.at(index).protocol() == QAbstractSocket::IPv4Protocol)
        {
            //IPv4地址
            if (list.at(index).toString().contains("127.0."))
            {
                continue;
            }
            localIp = list.at(index).toString();
            break;
        }
    }
    return localIp;
}

bool WebSocketServerManger::running() const
{
    return b_running;
}

void WebSocketServerManger::slot_start(QHostAddress hostAddress, quint32 port)
{
    if(b_running)
    {
        qDebug()<<__FILE__<<__LINE__
               <<"Failed to "<<__FUNCTION__<<"it is already running...";
        return;
    }

    if(!m_pWebSocketServer)
    {
        m_pWebSocketServer = new QWebSocketServer(m_serverName,m_ssMode,0);
        connect(m_pWebSocketServer, SIGNAL(newConnection()), this, SLOT(slot_newConnection()));
        connect(m_pWebSocketServer, SIGNAL(closed()), this, SLOT(slot_closed()));
        connect(m_pWebSocketServer, SIGNAL(serverError(QWebSocketProtocol::CloseCode)),
                this             , SLOT(slot_serverError(QWebSocketProtocol::CloseCode)));

    }

    m_listenHostAddress = hostAddress;
    m_listenPort = port;
    if(m_pWebSocketServer->listen(m_listenHostAddress,m_listenPort))
    {
        qDebug()<<"Listening prot : "<<QString::number(m_listenPort);
        b_running = true;
    }
}



void WebSocketServerManger::slot_stop()
{
    if(!b_running)
    {
        qDebug() << __FILE__ << __LINE__
                 << "Failed to" << __FUNCTION__
                 << ", it's not running...";
        return;
    }
    b_running = false;
    m_pWebSocketServer->close();
}

void WebSocketServerManger::slot_sendDate(QString ip, quint32 port, QString message)
{
    QString key = QString("%1-%2").arg(ip).arg(port);
    if(m_hashIpPort2PWebSocket.contains(key))
    {
        m_hashIpPort2PWebSocket.value(key)->sendTextMessage(message);
    }
}

void WebSocketServerManger::slot_newConnection()
{
    QWebSocket *pWebSocket = m_pWebSocketServer->nextPendingConnection();
    connect(pWebSocket,SIGNAL(disconnected()),this,SLOT(slot_disconnected()));
    connect(pWebSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slot_error(QAbstractSocket::SocketError)));
    connect(pWebSocket, SIGNAL(textFrameReceived(QString,bool)),
            this, SLOT(slot_textFrameReceived(QString,bool)));

    connect(pWebSocket,&QWebSocket::binaryFrameReceived,
            this,&WebSocketServerManger::slot_binaryFrameReceived);

    connect(pWebSocket,&QWebSocket::binaryMessageReceived,
            this,&WebSocketServerManger::slot_binaryMessageReceived);


    // Add the hash
    QString key = QString("%1-%2").arg(pWebSocket->peerAddress().toString())
            .arg(pWebSocket->peerPort());
    m_hashIpPort2PWebSocket.insert(key,pWebSocket);

    qDebug()<<__FUNCTION__ <<pWebSocket->peerAddress().toString() << pWebSocket->peerPort()<<" Key "<<key;

    emit signal_connected(pWebSocket->peerAddress().toString(),pWebSocket->peerPort());
}

void WebSocketServerManger::slot_serverError(QWebSocketProtocol::CloseCode closeCode)
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    emit signal_error(pWebSocket->peerAddress().toString(), pWebSocket->peerPort(), m_pWebSocketServer->errorString());
    Q_UNUSED(closeCode);
}

void WebSocketServerManger::slot_closed()
{
    QList<QWebSocket *>m_listWebSocket = m_hashIpPort2PWebSocket.values();

    for(int index = 0; index< m_listWebSocket.size(); index++)
    {
        m_listWebSocket.at(index)->close();
    }

    m_hashIpPort2PWebSocket.clear();
    emit signal_close();
}

void WebSocketServerManger::slot_disconnected()
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
        return;
    qDebug()<< __FILE__ << __LINE__ <<__FUNCTION__;

    emit signal_disconnected(pWebSocket->peerAddress().toString(),pWebSocket->peerPort());
    m_hashIpPort2PWebSocket.remove(QString("%1-%2").arg(pWebSocket->peerAddress().toString())
                                   .arg(pWebSocket->peerPort()));
}


void WebSocketServerManger::slot_error(QAbstractSocket::SocketError error)
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    emit signal_error(pWebSocket->peerAddress().toString(), pWebSocket->peerPort(), pWebSocket->errorString());

    Q_UNUSED(error);
}

void WebSocketServerManger::slot_textFrameReceived(const QString &frame, bool isLastFrame)
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    qDebug() << __FILE__ << __LINE__ << frame << isLastFrame;
    emit signal_textFrameReceived(pWebSocket->peerAddress().toString(), pWebSocket->peerPort(), frame, isLastFrame);
}
void WebSocketServerManger::slot_binaryMessageReceived(const QByteArray &message)
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }

    qDebug() << __FILE__ << __LINE__ << message ;
    emit signal_binaryMessageReceived(pWebSocket->peerAddress().toString(), pWebSocket->peerPort(),message);
}

void WebSocketServerManger::slot_binaryFrameReceived(const QByteArray &frame, bool isLastFrame)
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }

   // qDebug() << __FILE__ << __LINE__ << frame << isLastFrame;
    emit signal_binaryFrameReceived(pWebSocket->peerAddress().toString(), pWebSocket->peerPort(), frame, isLastFrame);
}


void WebSocketServerManger::slot_textMessageReceived(const QString &message)
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    emit signal_textMessageReceived(pWebSocket->peerAddress().toString(), pWebSocket->peerPort(), message);
}
