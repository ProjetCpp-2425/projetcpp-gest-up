#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow),
    serialPort(new QSerialPort(this))
{
    ui->setupUi(this); // Charger l'interface utilisateur

    // Masquer le mot de passe par défaut
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Connexions des signaux/slots
    connect(ui->togglePasswordButton, &QPushButton::clicked, this, &LoginWindow::togglePasswordVisibility);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(serialPort, &QSerialPort::readyRead, this, &LoginWindow::onDataReceived);

    // Initialisation du port série
    serialPort->setPortName("COM7"); // Remplacez par votre port série
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le port série COM7.");
    }

}

LoginWindow::~LoginWindow() {
    delete ui;

    if (serialPort->isOpen()) {
        serialPort->close();
    }
}

void LoginWindow::connectToDatabase() {
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("PROJET"); // Remplacez par votre source ODBC
    db.setUserName("projetcpp2a30");
    db.setPassword("esprit24");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur", "Impossible de se connecter à la base de données.\n" + db.lastError().text());
    }
}

void LoginWindow::onLoginClicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if(username == "a" && password == "a") {
        emit loginSuccessful();  // Emettre un signal de succès
        close();  // Fermer la fenêtre de connexion
    } else {
        QMessageBox::critical(this, "Login Failed", "Incorrect username or password.");
    }
}


void LoginWindow::togglePasswordVisibility() {
    if (ui->passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}

void LoginWindow::onDataReceived() {
    buffer.append(serialPort->readAll());

    if (buffer.contains('\n')) {
        QString receivedData = QString::fromUtf8(buffer).trimmed();
        buffer.clear(); // Réinitialiser le tampon

        qDebug() << "UID reçu :" << receivedData;

        QString name = getEmployeeName(receivedData);
        if (!name.isEmpty()) {
            sendToArduino("Bonjour " + name); // Envoi du message au LCD
            QMessageBox::information(this, "Authentification réussie", "Bonjour " + name + " !");
            emit loginSuccessful();
            close();
        } else {
            sendToArduino("UID inconnu"); // Envoi de message d'erreur
            QMessageBox::warning(this, "Erreur", "UID inconnu !");
        }
    }
}

QString LoginWindow::getEmployeeName(const QString &uid) {
    QSqlQuery query;
    query.prepare("SELECT PRENOM FROM EMPLOYES WHERE UID_ARD = :uid");
    query.bindValue(":uid", uid);

    if (query.exec() && query.next()) {
        return query.value(0).toString(); // Retourne le prénom
    }

    return QString(); // Retourne une chaîne vide si l'UID est inconnu
}

void LoginWindow::sendToArduino(const QString &message) {
    if (serialPort->isOpen()) {
        serialPort->write(message.toUtf8() + '\n'); // Envoyer le message avec un saut de ligne
    } else {
        qDebug() << "Erreur : Port série non ouvert.";
    }
}
