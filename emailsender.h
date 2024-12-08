#ifndef EMAILSENDER_H
#define EMAILSENDER_H

#include <QMainWindow>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QRegularExpression>

namespace Ui {
class EMAILSENDER;
}

class EMAILSENDER : public QMainWindow
{
    Q_OBJECT

public:
    explicit EMAILSENDER(QWidget *parent = nullptr);
    ~EMAILSENDER();

private slots:
    void sendEmail(); // Fonction pour envoyer un email

private:
    Ui::EMAILSENDER *ui;                  // Interface utilisateur
    QNetworkAccessManager *networkManager; // Gestionnaire pour les requêtes HTTP
};

#endif // EMAILSENDER_H
