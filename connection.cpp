#include "connection.h"
#include <QSqlError>
#include <QDebug>

Connection::Connection()
{}

bool Connection::createconnect()
{
    bool test = false;

    // Créer une connexion ODBC
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    // Paramètres de connexion
    db.setDatabaseName("asmasource"); // Le nom de la source de données ODBC
    db.setUserName("asmaprojet");      // Le nom d'utilisateur
    db.setPassword("esprit24");        // Le mot de passe

    // Tentative d'ouverture de la connexion
    if (db.open())
    {
        test = true;
        qDebug() << "Connexion réussie!";
    }
    else
    {
        qDebug() << "Échec de la connexion: " << db.lastError().text();
    }

    return test;
}
