#include "socketserver.h"
#include "ui_socketserver.h"
#include <QFile>
#include <QDir>


SocketServer::SocketServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SocketServer)
{
    ui->setupUi(this);
    setWindowTitle(" Norden Server");
    ui->port_Edit->setText("3333");
    updateUIListenRuning(false);

    m_pWebSocketServerManger = new WebSocketServerManger("Norden ");
    connect(m_pWebSocketServerManger, SIGNAL(signal_connected(QString,qint32)),
            this                    , SLOT(slot_connected(QString,qint32)));
    connect(m_pWebSocketServerManger, SIGNAL(signal_disconnected(QString,qint32)),
            this                    , SLOT(slot_disconnected(QString,qint32)));
    connect(m_pWebSocketServerManger, SIGNAL(signal_error(QString,quint32,QString)),
            this                    , SLOT(slot_error(QString,quint32,QString)));
    connect(m_pWebSocketServerManger, SIGNAL(signal_textFrameReceived(QString,quint32,QString,bool)),
            this                    , SLOT(slot_textFrameReceived(QString,quint32,QString,bool)));
    connect(m_pWebSocketServerManger, SIGNAL(signal_textMessageReceived(QString,quint32,QString)),
            this                    , SLOT(slot_textMessageReceived(QString,quint32,QString)));
//    connect(m_pWebSocketServerManger,&WebSocketServerManger::signal_binaryFrameReceived,
//            this                    ,&SocketServer::slot_binaryFrameReceived);

    connect(m_pWebSocketServerManger, &WebSocketServerManger::signal_binaryMessageReceived,
            this                    , &SocketServer::slot_binaryMessageReceived);


    connect(ui->listen_btn,&QPushButton::clicked,this,&SocketServer::pushButton_listen_clicked);
    connect(ui->stop_btn,&QPushButton::clicked,this,&SocketServer::pushButton_stop_clicked);
    connect(ui->send_btn,&QPushButton::clicked,this,&SocketServer::pushbutton_send_clicked);
    connect(ui->listView_ipPort,&QListView::clicked,this,&SocketServer::pushButton_ipPort_clicked);

    ui->ip_Edit->setText(m_pWebSocketServerManger->getLocalIp());
}

void SocketServer::slot_connected(QString ip, qint32 port)
{
    QString key = QString("%1-%2").arg(ip).arg(port);
    m_strList<<key;
    m_hashIpPort2Message.insert(key,QString());
    m_model.setStringList(m_strList);
    ui->listView_ipPort->setModel(&m_model);
    if(m_strList.size() == 1)
    {
        ui->listView_ipPort->setCurrentIndex(m_model.index(0));
    }
}

void SocketServer::slot_disconnected(QString ip, qint32 port)
{
     QString key = QString("%1-%2").arg(ip).arg(port);
     if(m_strList.contains(key))
     {
         m_strList.removeOne(key);
         m_model.setStringList(m_strList);
         ui->listView_ipPort->setModel(&m_model);
         updateTextEdit();
     }
}

void SocketServer::slot_sendTextMessageResult(QString ip, quint32 port, bool result)
{
    qDebug()<<" ip-port "<<QString("%1-%2").arg(ip).arg(port)<<" result :"<<QString::number(result);
}

void SocketServer::slot_sendBinaryMessageResult(QString ip, quint32 port, bool result)
{
    qDebug()<<" ip-port "<<QString("%1-%2").arg(ip).arg(port)<<" result :"<<QString::number(result);
}

void SocketServer::slot_error(QString ip, quint32 port, QString errorString)
{
    qDebug()<<" ip-port "<<QString("%1-%2").arg(ip).arg(port)<<" error :"<<errorString;
}

void SocketServer::slot_textFrameReceived(QString ip, quint32 port, QString frame, bool isLastFrame)
{
    QString key  = QString("%1-%2").arg(ip).arg(port);
    if(m_hashIpPort2Message.contains(key))
    {
        m_hashIpPort2Message[key] = m_hashIpPort2Message[key]+(m_hashIpPort2Message[key].size() == 0 ? "" :"\n")+
                QDateTime::currentDateTime().toString("yyyy-HH-mm hh:MM:ss:zzz")+"\n"+frame;
    }
    updateTextEdit();
}

void SocketServer::slot_binaryFrameReceived(QString ip, quint32 port, QByteArray frame, bool isLastFrame)
{



}

void SocketServer::slot_binaryMessageReceived(QString ip, quint32 port, QByteArray message)
{
    qDebug()<<" Received: "<<message.size()<<endl;
    QString appPath = QDir::currentPath(); //QCoreApplication::applicationDirPath();
    QString strPath = appPath+"/test.stl";

    QFile file(strPath);
    qDebug()<<"file path : "<<strPath;
    if(!file.open(QIODevice::ReadWrite|QIODevice::Truncate))
    {
      qDebug()<<__FUNCTION__<<" open file error! "<<endl;
      return;
    }
    file.write(message);
    file.close();
}

void SocketServer::slot_textMessageReceived(QString ip, quint32 port, QString message)
{
    QString key  = QString("%1-%2").arg(ip).arg(port);
    if(m_hashIpPort2Message.contains(key))
    {
        m_hashIpPort2Message[key] = m_hashIpPort2Message[key]+(m_hashIpPort2Message[key].size() == 0 ? "" :"\n")+
                QDateTime::currentDateTime().toString("yyyy-HH-mm hh:MM:ss:zzz")+"\n"+message;
    }
    updateTextEdit();
}

void SocketServer::slot_close()
{
   m_pWebSocketServerManger->slot_closed();
   m_strList.clear();
   m_model.setStringList(m_strList);
   ui->listView_ipPort->setModel(&m_model);
   ui->receive_edit->clear();
}

void SocketServer::updateTextEdit()
{
    int row =  ui->listView_ipPort->currentIndex().row();
    if(row < 0 )
    {
        ui->receive_edit->setText("");
        return;
    }

    if(m_hashIpPort2Message.contains(m_strList.at(row)))
    {
        ui->receive_edit->setText(m_hashIpPort2Message.value(m_strList.at(row)));
    }
}


void SocketServer::updateUIListenRuning(bool bRuning)
{
    ui->listen_btn->setEnabled(!bRuning);
    ui->stop_btn->setEnabled(bRuning);
    ui->send_btn->setEnabled(bRuning);
}

void SocketServer::pushButton_listen_clicked()
{
    m_pWebSocketServerManger->slot_start(QHostAddress::Any,ui->port_Edit->text().toInt());
    updateUIListenRuning(true);
}


void SocketServer::pushButton_stop_clicked()
{
    m_pWebSocketServerManger->slot_stop();
   updateUIListenRuning(false);
}

void SocketServer::pushButton_ipPort_clicked(const QModelIndex &index)
{
    updateTextEdit();
}

void SocketServer::pushbutton_send_clicked()
{
    int row = ui->listView_ipPort->currentIndex().row();
    if(m_hashIpPort2Message.contains(m_strList.at(row)))
    {
        QString str = m_strList.at(row);
        QStringList strList = str.split("-");
        if(strList.size() == 2)
        {
            m_pWebSocketServerManger->slot_sendDate(strList.at(0),
                                                    strList.at(1).toInt(),
                                                    ui->send_Edit->toPlainText());
        }
    }
}



SocketServer::~SocketServer()
{
    m_pWebSocketServerManger->deleteLater();
    delete ui;
}
