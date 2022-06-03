#include "client.h"
#include "ui_client.h"


Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    connect(ui->TCPcheckBox,&QCheckBox::stateChanged,[=](){ //TCP CheckBox clicked?
        if (ui->TCPcheckBox->isChecked() == true)
        {
            //ui->TCPcheckBox->setEnabled(false); //TCP CB DISABLE
            //ui->UDPcheckBox->setEnabled(false); //UDP CB DISABLE
            if (ui->UDPcheckBox->isChecked() == true)
            {
                UDPclient->flush();
                UDPclient2->flush();
                UDPclient->close();
                UDPclient2->close();
                delete UDPclient;
                delete UDPclient2;
                ui->UDPcheckBox->setChecked(false);
            }

            ui->ip->setText("127.0.0.1");
            ui->port->setText(QString::number(TCP_PORT));

            TCPclient = new QTcpSocket(this);

            TCPclient->connectToHost(ui->ip->text(),TCP_PORT);

            connect(ui->sendBtn,&QPushButton::clicked,[=](){
                if (ui->UDPcheckBox->isChecked() == false)
                {
                    QString msg = ui->msgEdit->toPlainText();

                    if (msg.isEmpty()==false)
                    {
                        TCPclient->write(msg.toUtf8());
                        ui->record->append("Client: "+ msg);    //write sent message from client
                    }
                    ui->msgEdit->clear();

                }
            });

            connect(TCPclient,&QTcpSocket::readyRead,[=](){
                if (ui->UDPcheckBox->isChecked() == false)
                {
                    QByteArray array = TCPclient->readAll();
                    ui->record->append("Server: "+ array);  //write received message from server
                    TCPclient->write("!"); //send ACK
                }
            });
        }
    });

    connect(ui->UDPcheckBox,&QCheckBox::stateChanged,[=](){    //UDP CheckBox clicked?
        if (ui->UDPcheckBox->isChecked() == true)
        {
            //ui->TCPcheckBox->setEnabled(false); //TCP CB DISABLE
            //ui->UDPcheckBox->setEnabled(false); //UDP CB DISABLE
            if (ui->TCPcheckBox->isChecked() == true)
            {
                TCPclient->flush();
                TCPclient->close();
                delete TCPclient;
                ui->TCPcheckBox->setChecked(false);
            }

            UDPclient = new QUdpSocket(this);
            UDPclient2 = new QUdpSocket(this);
            ui->ip->setText("127.0.0.2");
            ui->port->setText("P1: " + QString::number(UDP1_PORT) + "   " + "P2: "+ QString::number(UDP2_PORT));
            UDPclient2->bind(QHostAddress::LocalHost, UDP1_PORT);

            connect(UDPclient2, SIGNAL(readyRead()),
                    this, SLOT(processPendingDatagrams()));

            connect(ui->sendBtn,&QPushButton::clicked,[=](){
                if (ui->TCPcheckBox->isChecked() == false)
                {
                    QString msg = ui->msgEdit->toPlainText();

                    if (msg.isEmpty()==false)
                    {
                        QByteArray datagram = ui->msgEdit->toPlainText().toUtf8();
                        UDPclient->writeDatagram(datagram.data(), datagram.size(),
                                                 QHostAddress::LocalHost, UDP2_PORT);
                        ui->record->append("Client: "+ msg);    //write sent message from client
                        ui->msgEdit->clear();
                    }
                }
            });
        }

    });

}

void Client::processPendingDatagrams()
{
    if (ui->TCPcheckBox->isChecked() == false)
    {
        while (UDPclient2->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(UDPclient2->pendingDatagramSize());
            UDPclient2->readDatagram(datagram.data(), datagram.size());
            QString msg = datagram.data();
            ui->record->append("Server: "+ msg);

            QByteArray datagramACK = "!";
            UDPclient->writeDatagram(datagramACK.data(), datagramACK.size(),    //send ACK
                                     QHostAddress::LocalHost, UDP2_PORT);
        };
    }
}

Client::~Client()
{
    delete ui;
}
