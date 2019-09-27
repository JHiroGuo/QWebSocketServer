#ifndef WEBSOCKETSERVERMANGER_H
#define WEBSOCKETSERVERMANGER_H

#include <QObject>
#include <QPushButton>
#include <QWebSocketServer>
#include <QWebSocket>

class WebSocketServerManger : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketServerManger(QString serverName,
                                   QWebSocketServer::SslMode secureMode = QWebSocketServer::NonSecureMode,
                                   QObject *parent = nullptr);

    ~WebSocketServerManger();

public:
    bool running() const ;
    QString getLocalIp();



signals:
    void signal_connected(QString ip, qint32 port);
    void signal_disconnected(QString ip,qint32 port);
    void signal_sendTextMessageResult(QString ip,quint32 port,bool result);
    void signal_sendBinaryMessageResult(QString ip,quint32 port,bool result);
    void signal_textFrameReceived(QString ip,quint32 port,QString frame, bool isLastFrame);
    void signal_binaryFrameReceived(QString ip,quint32 port,QByteArray frame, bool isLastFrame);
    void signal_binaryMessageReceived(QString ip,quint32 port,QByteArray message);
    void signal_textMessageReceived(QString ip,quint32 port,QString message);
    void signal_error(QString ip,quint32 port,QString errorString);
    void signal_close();

public slots:
    void slot_start(QHostAddress hostAddress = QHostAddress(QHostAddress::Any),quint32 port = 10080);
    void slot_stop();
    void slot_closed();
    void slot_sendDate(QString ip,quint32 port,QString message);



protected slots:
    void slot_newConnection();
    void slot_serverError(QWebSocketProtocol::CloseCode closeCode);


protected slots:
    void slot_disconnected();
    void slot_error(QAbstractSocket::SocketError error);
    void slot_textFrameReceived(const QString &frame, bool isLastFrame);
    void slot_binaryMessageReceived(const QByteArray &message);
    void slot_binaryFrameReceived(const QByteArray &frame, bool isLastFrame);
    void slot_textMessageReceived(const QString &message);


private:
    QString m_serverName;
    QWebSocketServer::SslMode m_ssMode;
    bool b_running;
    QWebSocketServer *m_pWebSocketServer;
    QHash<QString ,QWebSocket*>m_hashIpPort2PWebSocket;
    QHostAddress m_listenHostAddress;
    qint32 m_listenPort;
};

#endif // WEBSOCKETSERVERMANGER_H
