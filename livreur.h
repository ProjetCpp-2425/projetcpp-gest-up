#ifndef LIVREUR_H
#define LIVREUR_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Livreur {
public:
    // Default constructor
    Livreur();

    // Parameterized constructor
    Livreur(QString idLivreur, QString nom, QString prenom, QString telephone, QString adresse);

    // Getters
    QString getIdLivreur();
    QString getNom();
    QString getPrenom();
    QString getTelephone();
    QString getAdresse();
    QSqlQueryModel* sortLivreurs(QString criterion);

    // Setters
    void setIdLivreur(QString idLivreur);
    void setNom(QString nom);
    void setPrenom(QString prenom);
    void setTelephone(QString telephone);
    void setAdresse(QString adresse);

    // Database operations
    bool addLivreur();
    QSqlQueryModel* displayLivreurs();
    bool deleteLivreur(QString idLivreur);
    bool editLivreur(QString idLivreur);

private:
    QString idLivreur;      // ID_LIVREUR: Unique ID for Livreur
    QString nom;            // NOM: Livreur's last name
    QString prenom;         // PRENOM: Livreur's first name
    QString telephone;      // TELEPHONE: Livreur's phone number
    QString adresse;        // ADRESSE: Livreur's address
};

#endif // LIVREUR_H
