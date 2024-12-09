#include "livreur.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>

// Default constructor
Livreur::Livreur() {}

// Parameterized constructor
Livreur::Livreur(QString idLivreur, QString nom, QString prenom, QString telephone, QString adresse) {
    this->idLivreur = idLivreur;
    this->nom = nom;
    this->prenom = prenom;
    this->telephone = telephone;
    this->adresse = adresse;
}

// Getters
QString Livreur::getIdLivreur() { return idLivreur; }
QString Livreur::getNom() { return nom; }
QString Livreur::getPrenom() { return prenom; }
QString Livreur::getTelephone() { return telephone; }
QString Livreur::getAdresse() { return adresse; }

// Setters
void Livreur::setIdLivreur(QString idLivreur) { this->idLivreur = idLivreur; }
void Livreur::setNom(QString nom) { this->nom = nom; }
void Livreur::setPrenom(QString prenom) { this->prenom = prenom; }
void Livreur::setTelephone(QString telephone) { this->telephone = telephone; }
void Livreur::setAdresse(QString adresse) { this->adresse = adresse; }

// Adding a new livreur
bool Livreur::addLivreur() {
    QSqlQuery query;
    query.prepare("INSERT INTO livreur (idLivreur, nom, prenom, telephone, adresse) "
                  "VALUES (:idLivreur, :nom, :prenom, :telephone, :adresse)");
    query.bindValue(":idLivreur", idLivreur);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":telephone", telephone);
    query.bindValue(":adresse", adresse);

    return query.exec();
}

// Displaying all livreurs
QSqlQueryModel* Livreur::displayLivreurs() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM livreur");
    return model;
}

// Deleting a livreur by ID
bool Livreur::deleteLivreur(QString idLivreur) {
    QSqlQuery query;
    query.prepare("DELETE FROM livreur WHERE idLivreur = :idLivreur");
    query.bindValue(":idLivreur", idLivreur);

    return query.exec();
}

// Editing a livreur by ID
bool Livreur::editLivreur(QString idLivreur) {
    QSqlQuery query;
    query.prepare("UPDATE livreur SET nom = :nom, prenom = :prenom, telephone = :telephone, adresse = :adresse "
                  "WHERE idLivreur = :idLivreur");
    query.bindValue(":idLivreur", idLivreur);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":telephone", telephone);
    query.bindValue(":adresse", adresse);

    return query.exec();
}

// Sorting livreurs by a specified criterion
QSqlQueryModel* Livreur::sortLivreurs(QString criterion) {
    QSqlQueryModel* model = new QSqlQueryModel();

    QString queryStr = "SELECT * FROM livreur ORDER BY " + criterion;
    model->setQuery(queryStr);

    return model;
}
