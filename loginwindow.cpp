#include "loginwindow.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);  // Initialise l'interface générée par Qt Designer

    // Par défaut, le mot de passe est masqué
    ui.passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Connexion du bouton pour basculer la visibilité du mot de passe
    connect(ui.togglePasswordButton, &QPushButton::clicked, this, &LoginWindow::togglePasswordVisibility);

    // Connexion du bouton de connexion
    connect(ui.loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

LoginWindow::~LoginWindow() {}

void LoginWindow::onLoginClicked()
{
    QString username = ui.usernameLineEdit->text();
    QString password = ui.passwordLineEdit->text();

    if(username == "a" && password == "a") {
        emit loginSuccessful();  // Emettre un signal de succès
        close();  // Fermer la fenêtre de connexion
    } else {
        QMessageBox::critical(this, "Login Failed", "Incorrect username or password.");
    }
}

void LoginWindow::togglePasswordVisibility()
{
    // Vérifier si le mot de passe est actuellement masqué ou visible
    if (ui.passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui.passwordLineEdit->setEchoMode(QLineEdit::Normal);  // Rendre le mot de passe visible
    } else {
        ui.passwordLineEdit->setEchoMode(QLineEdit::Password);  // Masquer le mot de passe
    }
}
