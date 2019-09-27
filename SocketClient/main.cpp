#include "socketclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SocketClient w;
    w.show();

    return a.exec();
}
