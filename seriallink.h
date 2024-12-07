#ifndef SERIALLINK_H
#define SERIALLINK_H
#include <QSerialPort>
#include <QObject>

class seriallink : public QObject
{
    Q_OBJECT
public:
    explicit seriallink(QObject *parent = nullptr);
    ~seriallink();
    bool connectToArduino(const QString &portName, int baudRate = QSerialPort::Baud9600);
    void disconnectFromArduino();
    void sendCommand(const QString &command);
private slots :
    void readData();
signals:
    void dataReceived(const QString &data); // Déclaration du signal
private:
    QSerialPort *arduino;
};

#endif // SERIALLINK_H
