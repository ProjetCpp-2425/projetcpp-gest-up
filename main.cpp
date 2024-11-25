#include "mainwindow.h"
#include <QApplication>
#include "loginwindow.h"
#include "connection.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Créer l'objet de la fenêtre de connexion
    LoginWindow loginWindow;

    // Créer l'objet de la fenêtre principale
    MainWindow mainWindow;

    // Créer l'objet de connexion
    Connection c;

    // Vérifier la connexion à la base de données
    bool test = c.createconnect();

    // Si la connexion à la base de données est réussie
    if (test) {
        // Connexion du signal loginSuccessful à l'affichage de la fenêtre principale
        QObject::connect(&loginWindow, &LoginWindow::loginSuccessful, [&]() {
            mainWindow.show();
            QMessageBox::information(nullptr, QObject::tr("database is open"),
                                     QObject::tr("Connection successful.\n"
                                                 "Click Cancel to exit."),
                                     QMessageBox::Cancel);
        });

        // Afficher la fenêtre de connexion
        loginWindow.show();
    } else {
        // Si la connexion à la base de données échoue
        QMessageBox::critical(nullptr, QObject::tr("database is not open"),
                              QObject::tr("Connection failed.\n"
                                          "Click Cancel to exit."), QMessageBox::Cancel);
    }

    return a.exec();
}
