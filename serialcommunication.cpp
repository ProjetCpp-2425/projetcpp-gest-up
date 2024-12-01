// serialcommunication.cpp
#include "serialcommunication.h"
#include <QDebug>

SerialCommunication::SerialCommunication(QObject *parent)
    : QObject(parent),
      serialPort(new QSerialPort(this))
{
}

SerialCommunication::~SerialCommunication()
{
    closePort();
}

bool SerialCommunication::openPort(const QString &portName)
{
    serialPort->setPortName(portName);
    if (serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Port ouvert:" << portName;
        return true;
    } else {
        qDebug() << "Erreur d'ouverture du port";
        return false;
    }
}

void SerialCommunication::closePort()
{
    if (serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "Port fermé";
    }
}

void SerialCommunication::readAvailableData()
{
    QByteArray data = serialPort->readAll(); // Lire les données disponibles
    qDebug() << "Données reçues:" << data;
    emit dataReceived(data);  // Émettre le signal
}
