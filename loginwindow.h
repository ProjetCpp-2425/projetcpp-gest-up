#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QByteArray>
#include "ui_loginwindow.h"

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();  // Slot pour gérer le clic sur le bouton de connexion
    void togglePasswordVisibility();  // Slot pour basculer la visibilité du mot de passe
    void onDataReceived();  // Slot pour traiter les données reçues du lecteur RFID

signals:
    void loginSuccessful();  // Signal émis lorsque la connexion est réussie

private:
    Ui::LoginWindow ui;  // Interface générée par Qt
    QSerialPort *serialPort;  // Objet pour la communication avec le port série
    QByteArray buffer;  // Tampon pour stocker les données reçues
};

#endif // LOGINWINDOW_H
