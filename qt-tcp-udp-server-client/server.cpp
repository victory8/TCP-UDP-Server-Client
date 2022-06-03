#include "server.h"
#include "ui_server.h"


Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);

    connect(ui->TCPcheckBox,&QCheckBox::stateChanged,[=](){    //TCP CheckBox clicked?
        if (ui->TCPcheckBox->isChecked() == true)
        {
            //ui->TCPcheckBox->setEnabled(false); //TCP CB DISABLE
            //ui->UDPcheckBox->setEnabled(false); //UDP CB DISABLE
            if (ui->UDPcheckBox->isChecked() == true)
            {
                UDPSocket->flush();
                UDPSocket2->flush();
                UDPSocket->close();
                UDPSocket2->close();
                delete UDPSocket;
                delete UDPSocket2;
                ui->UDPcheckBox->setChecked(false);
            }

            ui->ip->setText("127.0.0.1");
            ui->port->setText(QString::number(TCP_PORT));

            TCPlisten  = new QTcpServer(this);
            TCPlisten->listen(QHostAddress(ui->ip->text()), TCP_PORT);

            connect(TCPlisten,&QTcpServer::newConnection,[=](){
                if (ui->UDPcheckBox->isChecked() == false)
                {
                    //Returns the next pending connection as a connected QTcpSocket object.
                    TCPconn = TCPlisten->nextPendingConnection();

                    ui->record->append("Server Açıldı");

                    connect(TCPconn,&QTcpSocket::readyRead,[=](){
                        QByteArray array = TCPconn->readAll();
                        if (array[0] != '!')    //received normal message from client
                        {
                            ui->record->append("Client: "+ array);  //write received message from client
                        }
                        else    //received ACK from client
                        {
                            ui->record->append("Client: ACK");  //write received ACK from client
                            QPalette p = QPalette();
                            p.setColor(QPalette::Active, QPalette::Base, Qt::green);
                            ui->msgEdit->setPalette(p); // change to color for ACK

                            timer.setInterval(ACKTime);   //10s
                            connect(&timer, SIGNAL(timeout()), this, SLOT(updateColor()));
                            timer.start();
                        }
                    });
                }
            });
            connect(ui->sendBtn,&QPushButton::clicked,[=](){    //Gonder button
                if (ui->UDPcheckBox->isChecked() == false)
                {
                    QString msg = ui->msgEdit->toPlainText();
                    if (msg.isEmpty()==false)
                    {
                        TCPconn->write(msg.toUtf8());  //send message from server to client
                        ui->record->append("Server:" + msg);    //write sent message
                    }
                    ui->msgEdit->clear();
                }
            });
        }
        else
        {

        }
    });


    connect(ui->UDPcheckBox,&QCheckBox::stateChanged,[=](){    //UDP CheckBox clicked?
        if (ui->UDPcheckBox->isChecked() == true)
        {
            //ui->TCPcheckBox->setEnabled(false); //TCP CB DISABLE
            //ui->UDPcheckBox->setEnabled(false); //UDP CB DISABLE
            if (ui->TCPcheckBox->isChecked() == true)
            {
                TCPconn->flush();
                TCPconn->close();
                delete TCPconn;
                delete TCPlisten;
                ui->TCPcheckBox->setChecked(false);
            }

            UDPSocket = new QUdpSocket(this);
            UDPSocket2 = new QUdpSocket(this);
            ui->ip->setText("127.0.0.2");
            ui->port->setText("P1: " + QString::number(UDP1_PORT) + "   " + "P2: "+ QString::number(UDP2_PORT));
            UDPSocket->bind(QHostAddress::LocalHost, UDP2_PORT);
            connect(UDPSocket, SIGNAL(readyRead()),
                    this, SLOT(processPendingDatagrams()));

            connect(ui->sendBtn,&QPushButton::clicked,[=](){
                if (ui->TCPcheckBox->isChecked() == false)
                {
                    QString msg = ui->msgEdit->toPlainText();
                    if (msg.isEmpty()==false)
                    {
                        QByteArray datagram = msg.toUtf8();
                        UDPSocket2->writeDatagram(datagram.data(), datagram.size(),
                                                 QHostAddress::LocalHost, UDP1_PORT);
                        ui->record->append("Server: "+ msg);    //write sent message from client
                        ui->msgEdit->clear();
                    }
                }
            });
        }
        else
        {

        }
    });

}

void Server::processPendingDatagrams()
{
    if (ui->TCPcheckBox->isChecked() == false)
    {
        while (UDPSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(UDPSocket->pendingDatagramSize());
            UDPSocket->readDatagram(datagram.data(), datagram.size());
            QString msg = datagram.data();

            if (msg[0] != '!')    //received normal message from client
            {
                ui->record->append("Client: "+ msg);  //write received message from client
            }
            else    //received ACK from client
            {
                ui->record->append("Client: ACK");  //write received ACK from client
                QPalette p = QPalette();
                p.setColor(QPalette::Active, QPalette::Base, Qt::green);
                ui->msgEdit->setPalette(p); // change to color for ACK

                timer.setInterval(ACKTime);   //10s
                connect(&timer, SIGNAL(timeout()), this, SLOT(updateColor()));
                timer.start();
            }

        };
    }
}

void Server::updateColor()
{
    QPalette p = QPalette();
    p.setColor(QPalette::Active, QPalette::Base, Qt::white);
    ui->msgEdit->setPalette(p); // change to color for Default
    timer.stop();
}

Server::~Server()
{
    delete ui;
}

