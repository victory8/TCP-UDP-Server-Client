#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QUdpSocket>

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

public slots:
    void processPendingDatagrams();

public:
    QTcpSocket *TCPclient;
    QUdpSocket *UDPclient;
    QUdpSocket *UDPclient2;

    int64_t TCP_PORT = 7000;
    int64_t UDP1_PORT = 8000;
    int64_t UDP2_PORT = 8001;

private:
    Ui::Client *ui;
};

#endif // CLIENT_H
