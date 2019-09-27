#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QWidget>
#include <QStringListModel>
#include <QListWidget>
#include "../modules/websocketservermanger.h"

namespace Ui {
class SocketServer;
}

class SocketServer : public QWidget
{
    Q_OBJECT

public:
    explicit SocketServer(QWidget *parent = 0);
    ~SocketServer();

protected slots:
    void slot_connected(QString ip, qint32 port);
    void slot_disconnected(QString ip,qint32 port);
    void slot_sendTextMessageResult(QString ip,quint32 port,bool result);
    void slot_sendBinaryMessageResult(QString ip,quint32 port,bool result);
    void slot_textFrameReceived(QString ip,quint32 port,QString frame, bool isLastFrame);
    void slot_textMessageReceived(QString ip,quint32 port,QString message);
    void slot_binaryFrameReceived(QString ip,quint32 port,QByteArray frame, bool isLastFrame);
    void slot_binaryMessageReceived(QString ip,quint32 port,QByteArray message);
    void slot_error(QString ip,quint32 port,QString errorString);
    void slot_close();


protected:
    void updateTextEdit();
    void updateUIListenRuning(bool bRuning);

private slots:
    void pushButton_listen_clicked();
    void pushButton_stop_clicked();
    void pushbutton_send_clicked();
    void pushButton_ipPort_clicked(const QModelIndex &index);


private:
    Ui::SocketServer *ui;
    WebSocketServerManger *m_pWebSocketServerManger;
    QStringList m_strList;
    QStringListModel m_model;
    QHash<QString,QString>m_hashIpPort2Message;


public:

};

#endif // SOCKETSERVER_H
