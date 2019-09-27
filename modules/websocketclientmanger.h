#ifndef WEBSOCKETCLIENTMANAGER_H
#define WEBSOCKETCLIENTMANAGER_H

#include <QObject>
#include <QWebSocket>

class WebSocketClientManager : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketClientManager(QObject *parent = nullptr);
    ~WebSocketClientManager();

public:
    bool running() const;

    QString url() const;
    void setUrl(const QString &url);

signals:
    void signal_connected();
    void signal_disconnected();
    void signal_sendTextMessageResult(bool result);
    void signal_sendBinaryMessageResult(bool result);
    void signal_error(QString errorString);
    void signal_textFrameReceived(QString frame, bool isLastFrame);
    void signal_textMessageReceived(QString message);

public slots:
    void slot_start();
    void slot_stop();
    void slot_connected();
    void slot_disconnected();
    void slot_connectedTo(QString url);
    void slot_sendTextMessage(const QString &message);
    void slot_sendBinaryMessage(const QByteArray &data);

protected slots:
    void slot_error(QAbstractSocket::SocketError error);
    void slot_textFrameReceived(const QString &frame, bool isLastFrame);
    void slot_textMessageReceived(const QString &message);

private:
    bool m_running;
    QString m_url;
    bool m_connected;
    QWebSocket *m_pWebSocket;
};

#endif // WEBSOCKETCLIENTMANAGER_H
