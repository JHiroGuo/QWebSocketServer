#include "socketclient.h"
#include "ui_socketclient.h"
#include <QFileDialog>

SocketClient::SocketClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SocketClient),
    m_pWebSocketClientManager(0)

{
    ui->setupUi(this);
    setWindowTitle("Norden Client");
    ui->serverIp_edit->setText("ws://localhost:3333");
    updateConnectUI(false);
    m_pWebSocketClientManager = new WebSocketClientManager();
    connect(m_pWebSocketClientManager,&WebSocketClientManager::signal_connected,
            this, &SocketClient::slot_connected);
    connect(m_pWebSocketClientManager,&WebSocketClientManager::signal_disconnected,
            this, &SocketClient::slot_disconnected);
    connect(m_pWebSocketClientManager,&WebSocketClientManager::signal_error,
            this, &SocketClient::slot_error);
    connect(m_pWebSocketClientManager,&WebSocketClientManager::signal_textMessageReceived,
            this, &SocketClient::slot_textMessageReceived);


}

void SocketClient::updateConnectUI(bool bConnect)
{
    ui->connect_button->setEnabled(!bConnect);
    ui->stop_button->setEnabled(bConnect);
    ui->send_button->setEnabled(bConnect);
}

void SocketClient::slot_connected()
{
    updateConnectUI(true);
}

void SocketClient::slot_disconnected()
{
    updateConnectUI(false);
}

void SocketClient::slot_error(QString errorString)
{
//   QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
//   QString key = QString("%1-%2").arg(pWebSocket->peerAddress().toString())
//           .arg(pWebSocket->peerPort());
   qDebug()<<" error: "<<errorString;
}

void SocketClient::slot_textFrameReceived(const QString &frame, bool isLastFrame)
{
    ui->receive_edit->setText((ui->receive_edit->toPlainText().size() ==0 ? "":"\n")
                              + ui->receive_edit->toPlainText()
                              + QDateTime::currentDateTime().toString("yyyy-HH-mm hh:MM:ss:zzz")
                              + "\n"
                              + frame);
}



void SocketClient::slot_textMessageReceived(const QString &message)
{
    ui->receive_edit->setText((ui->receive_edit->toPlainText().size() ==0 ? "":"\n")
                              + ui->receive_edit->toPlainText()
                              + QDateTime::currentDateTime().toString("yyyy-HH-mm hh:MM:ss:zzz")
                              + "\n"
                              + message);
}

SocketClient::~SocketClient()
{

    m_pWebSocketClientManager->deleteLater();
    delete ui;
}

void SocketClient::on_send_button_clicked()
{
  m_pWebSocketClientManager->slot_sendTextMessage(ui->send_edit->toPlainText());
}

void SocketClient::on_stop_button_clicked()
{
    m_pWebSocketClientManager->slot_disconnected();
}

void SocketClient::on_connect_button_clicked()
{
    m_pWebSocketClientManager->slot_start();
    m_pWebSocketClientManager->slot_connectedTo(ui->serverIp_edit->text());
}



void SocketClient::on_send_file_button_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select File To Send","File(*)");
    if(path.length()<1) return;
    QFileInfo info(path);
    QString file_name = info.fileName();

    // 文件发送协议
    m_pWebSocketClientManager->slot_sendTextMessage("f:"+file_name);
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open file for writing";
        return;
    }
    qDebug() << "read ....... ";
    QByteArray byte = file.readAll();
    qDebug()<<" file: "<<byte;
    m_pWebSocketClientManager->slot_sendBinaryMessage(byte);
    qDebug() << "send..... end   ";
    file.close();
}
