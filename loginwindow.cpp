#include "loginwindow.h"
#include <QMessageBox>
#include <QSerialPort>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent), serialPort(new QSerialPort(this))
{
    ui.setupUi(this);  // Initialise l'interface générée par Qt Designer

    // Par défaut, le mot de passe est masqué
    ui.passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Connexion du bouton pour basculer la visibilité du mot de passe
    connect(ui.togglePasswordButton, &QPushButton::clicked, this, &LoginWindow::togglePasswordVisibility);

    // Connexion du bouton de connexion
    connect(ui.loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);

    // Initialisation du port série pour le lecteur RFID
    connect(serialPort, &QSerialPort::readyRead, this, &LoginWindow::onDataReceived);

    // Configuration du port série (COM7)
    serialPort->setPortName("COM7");  // Utiliser COM7, à adapter si nécessaire
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    // Essayer d'ouvrir le port série
    if (!serialPort->open(QIODevice::ReadOnly)) {
        // Si l'ouverture échoue, afficher le code d'erreur
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le port série COM7.\nCode d'erreur : " + QString::number(serialPort->error()));
    }
}

LoginWindow::~LoginWindow() {
    if (serialPort->isOpen()) {
        serialPort->close();
    }
}

void LoginWindow::onLoginClicked() {
    // Code pour le login traditionnel (nom d'utilisateur et mot de passe) si nécessaire
    QString username = ui.usernameLineEdit->text();
    QString password = ui.passwordLineEdit->text();

    if(username == "a" && password == "a") {
        emit loginSuccessful();  // Emettre un signal de succès
        close();  // Fermer la fenêtre de connexion
    } else {
        QMessageBox::critical(this, "Login Failed", "Incorrect username or password.");
    }
}

void LoginWindow::togglePasswordVisibility() {
    // Vérifier si le mot de passe est actuellement masqué ou visible
    if (ui.passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui.passwordLineEdit->setEchoMode(QLineEdit::Normal);  // Rendre le mot de passe visible
    } else {
        ui.passwordLineEdit->setEchoMode(QLineEdit::Password);  // Masquer le mot de passe
    }
}

void LoginWindow::onDataReceived() {
    // Lire toutes les données disponibles dans le tampon
    QByteArray data = serialPort->readAll();
    buffer.append(data);  // Ajouter les nouvelles données au tampon

    // Vérifier si le tampon contient le caractère de fin (ex : '\n' ou '\r\n')
    if (buffer.contains('\n')) {
        // Convertir le tampon en QString et nettoyer les espaces inutiles
        QString receivedData = QString::fromStdString(buffer.trimmed().toStdString());

        // Nettoyer les caractères de retour chariot (\r) et de saut de ligne (\n) de l'UID
        receivedData.replace("\r", "").replace("\n", ""); // Supprimer les caractères indésirables

        qDebug() << "Données brutes reçues: " << receivedData;

        // Extraire l'UID (en supposant que l'UID commence après un certain préfixe ou séparateur)
        QStringList parts = receivedData.split(":");  // Séparer le préfixe de l'UID
        if (parts.size() > 1) {
            QString uid = parts[1].trimmed();  // Extraire l'UID et le nettoyer
            qDebug() << "UID extrait: " << uid;

            // Normaliser l'UID (mettre en majuscules et supprimer les espaces)
            uid = uid.trimmed();
            uid = uid.toUpper();  // Convertir en majuscules

            // Créer un tableau de correspondance UID -> ID
            QMap<QString, QString> uidToId;
            uidToId["23 49 BB D9"] = "001";  // Arslen
            uidToId["1D D9 97 3F"] = "002";  // Habib

            // Vérifier si l'UID est dans la liste
            if (uidToId.contains(uid)) {
                // Récupérer l'ID associé à l'UID
                QString id = uidToId[uid];
                qDebug() << "ID extrait: " << id;

                // Rechercher l'ID dans la base de données pour obtenir le nom
                QSqlQuery query;
                query.prepare("SELECT PRENOM, NOM FROM EMPLOYES WHERE ID = :id");
                query.bindValue(":id", id);

                if (query.exec()) {
                    if (query.next()) {
                        QString prenom = query.value(0).toString();  // Récupérer le prénom de l'employé
                        QString nom = query.value(1).toString();     // Récupérer le nom de l'employé
                        QMessageBox::information(this, "Authentification réussie", "Bonjour " + prenom + " " + nom + "!");
                        emit loginSuccessful();  // Emettre un signal de succès
                        close();  // Fermer la fenêtre de connexion
                    } else {
                        QMessageBox::warning(this, "Erreur d'authentification", "UID inconnu !");
                    }
                } else {
                    QMessageBox::critical(this, "Erreur", "Erreur de requête SQL : " + query.lastError().text());
                }
            }

            // Réinitialiser le tampon pour la prochaine lecture
            buffer.clear();
        }
    }
}
