#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

public slots:
    void updateColor();
    void processPendingDatagrams();

public:
    QTcpServer * TCPlisten;
    QTcpSocket * TCPconn;
    QUdpSocket * UDPSocket;
    QUdpSocket * UDPSocket2;

    int64_t TCP_PORT = 7000;
    int64_t UDP1_PORT = 8000;
    int64_t UDP2_PORT = 8001;
    int64_t ACKTime = 10000;

private:
    Ui::Server *ui;
    QTimer timer;
    QCheckBox TCPCB;
    QCheckBox UDPCB;

};
#endif // SERVER_H
