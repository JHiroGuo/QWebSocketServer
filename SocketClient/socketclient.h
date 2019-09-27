#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QWidget>
#include "../modules/websocketclientmanger.h"

namespace Ui {
class SocketClient;
}

class SocketClient : public QWidget
{
    Q_OBJECT

public:
    explicit SocketClient(QWidget *parent = 0);
    ~SocketClient();
    void updateConnectUI(bool bConnect);


protected slots:
    void slot_connected();
    void slot_disconnected();
    void slot_error(QString errorString);
    void slot_textFrameReceived(const QString &frame, bool isLastFrame);
    void slot_textMessageReceived(const QString &message);

private slots:
    void on_send_button_clicked();
    void on_stop_button_clicked();
    void on_connect_button_clicked();

    void on_send_file_button_clicked();

private:
    Ui::SocketClient *ui;
    WebSocketClientManager *m_pWebSocketClientManager;
};

#endif // SOCKETCLIENT_H
