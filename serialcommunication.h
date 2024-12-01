// serialcommunication.h
#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>

class SerialCommunication : public QObject
{
    Q_OBJECT

public:
    explicit SerialCommunication(QObject *parent = nullptr);
    ~SerialCommunication();

    bool openPort(const QString &portName);
    void closePort();

signals:
    void dataReceived(const QByteArray &data);  // Déclaration du signal

public slots:
    void readAvailableData();

private:
    QSerialPort *serialPort;
};

#endif // SERIALCOMMUNICATION_H
