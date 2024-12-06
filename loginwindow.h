#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccessful(); // Signal pour indiquer un succès de connexion

private slots:
    void onLoginClicked(); // Login avec utilisateur et mot de passe
    void togglePasswordVisibility(); // Afficher/masquer le mot de passe
    void onDataReceived(); // Traitement des données reçues depuis Arduino

private:
    Ui::LoginWindow *ui;
    QSerialPort *serialPort; // Port série pour la communication avec l'Arduino
    QByteArray buffer; // Tampon pour stocker les données série reçues
    QSqlDatabase db; // Base de données

    void connectToDatabase(); // Fonction pour se connecter à la base de données
    QString getEmployeeName(const QString &uid); // Fonction pour récupérer le prénom basé sur l'UID
    void sendToArduino(const QString &message); // Envoi de message à l'Arduino
};

#endif // LOGINWINDOW_H
