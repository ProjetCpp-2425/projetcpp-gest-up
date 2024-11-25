#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
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

signals:
    void loginSuccessful();  // Signal émis lorsque la connexion est réussie

private:
    Ui::LoginWindow ui;  // Interface générée par Qt
};

#endif // LOGINWINDOW_H
