#include "websocketclientmanger.h"
#include <QDebug>

WebSocketClientManager::WebSocketClientManager(QObject *parent)
    : QObject(parent),
    m_running(false),
    m_pWebSocket(0),
    m_connected(false)
{

}

WebSocketClientManager::~WebSocketClientManager()
{
    if(m_pWebSocket != 0)
    {
        m_pWebSocket->deleteLater();
        m_pWebSocket = 0;
    }
}

bool WebSocketClientManager::running() const
{
    return m_running;
}

void WebSocketClientManager::slot_start()
{
    if(m_running)
    {
        qDebug() << __FILE__ << __LINE__
                 << "Failed to" << __FUNCTION__ << "it's already running...";
        return;
    }
    if(!m_pWebSocket)
    {
        m_pWebSocket = new QWebSocket();
        connect(m_pWebSocket, SIGNAL(connected())   , this, SLOT(slot_connected())   );
        connect(m_pWebSocket, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
        connect(m_pWebSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this       , SLOT(slot_error(QAbstractSocket::SocketError)));
        connect(m_pWebSocket, SIGNAL(textFrameReceived(QString,bool)),
                this       , SLOT(slot_textFrameReceived(QString,bool)));
        connect(m_pWebSocket, SIGNAL(textMessageReceived(QString)),
                this       , SLOT(slot_textMessageReceived(QString)));
    }
    m_running = true;
}

void WebSocketClientManager::slot_stop()
{
    if(!m_running)
    {
        qDebug() << __FILE__ << __LINE__
                 << "Failed to" << __FUNCTION__
                 << ", it's not running...";
        return;
    }
    m_running = false;
    m_pWebSocket->close();
}

void WebSocketClientManager::slot_connectedTo(QString url)
{
    if(!m_running)
    {
        qDebug() << __FILE__ << __LINE__
                 << "Failed to" << __FUNCTION__
                 << ", it's not running...";
        return;
    }
    m_pWebSocket->open(QUrl(url));
}

void WebSocketClientManager::slot_sendTextMessage(const QString &message)
{
    if(!m_running)
    {
        qDebug() << __FILE__ << __LINE__
                 << "Failed to" << __FUNCTION__
                 << ", it's not running...";
        return;
    }
    bool result = true;
    m_pWebSocket->sendTextMessage(message);
    emit signal_sendTextMessageResult(result);
}

void WebSocketClientManager::slot_sendBinaryMessage(const QByteArray &data)
{
    if(!m_running)
    {
        qDebug() << __FILE__ << __LINE__
                 << "Failed to" << __FUNCTION__
                 << ", it's not running...";
        return;
    }
    bool result = true;
    m_pWebSocket->sendBinaryMessage(data);
    emit signal_sendBinaryMessageResult(result);
}

void WebSocketClientManager::slot_connected()
{
    m_connected = true;
    qDebug() << __FILE__ << __LINE__ << "connected";
    emit signal_connected();
}

void WebSocketClientManager::slot_disconnected()
{
    m_connected = false;
    qDebug() << __FILE__ << __LINE__ << "disconnected";
    emit signal_disconnected();
}

void WebSocketClientManager::slot_error(QAbstractSocket::SocketError error)
{
    qDebug() << __FILE__ << __LINE__ << (int)error << m_pWebSocket->errorString();
    emit signal_error(m_pWebSocket->errorString());
}

void WebSocketClientManager::slot_textFrameReceived(const QString &frame, bool isLastFrame)
{
    emit signal_textFrameReceived(frame, isLastFrame);
}

void WebSocketClientManager::slot_textMessageReceived(const QString &message)
{
    emit signal_textMessageReceived(message);
}

QString WebSocketClientManager::url() const
{
    return m_url;
}

void WebSocketClientManager::setUrl(const QString &url)
{
    m_url = url;
}

