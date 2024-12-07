#include "commande.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QObject>
#include <QVariant>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include "commande.h"
#include <QSqlError>


// Constructeurs
Commande::Commande()
{
    idCommande = 0;
    total = 0.0;
    typeDePaiement = " ";
    nom = " ";
    description = " ";
    dateDePeremption = " ";
    etat = " ";  // Initialisation de 'etat'
}
Commande::Commande(int idCommande, QString dateDePeremption, double total, QString typeDePaiement, QString nom, QString description, QString etat)
{
    this->idCommande = idCommande;
    this->dateDePeremption = dateDePeremption;
    this->total = total;
    this->typeDePaiement = typeDePaiement;
    this->nom = nom;
    this->description = description;
    this->etat = etat;  // Initialisation de 'etat'
}

// Getters
int Commande::getIdCommande() const { return idCommande; }
QString Commande::getDateDePeremption() const { return dateDePeremption; }
double Commande::getTotal() const { return total; }
QString Commande::getTypeDePaiement() const { return typeDePaiement; }
QString Commande::getNom() const { return nom; }
QString Commande::getDescription() const { return description; }
QString Commande::getEtat() const { return etat; }  // Getter pour 'etat'


// Setters
void Commande::setIdCommande(int id) { this->idCommande = id; }
void Commande::setDateDePeremption(const QString& date) { this->dateDePeremption = date; }
void Commande::setTotal(double tot) { this->total = tot; }
void Commande::setTypeDePaiement(const QString& typePaiement) { this->typeDePaiement = typePaiement; }
void Commande::setNom(const QString& name) { this->nom = name; }
void Commande::setDescription(const QString& desc) { this->description = desc; }
void Commande::setEtat(const QString& etat) { this->etat = etat; }  // Setter pour 'etat'
// Méthodes CRUD
bool Commande::ajouter()
{
    QSqlQuery query;

    query.prepare("INSERT INTO commande (idCommande, dateDePeremption, total, typeDePaiement, nom, description, etat) "
                  "VALUES (:idCommande, TO_DATE(:dateDePeremption, 'YYYY-MM-DD HH24:MI:SS'), :total, :typeDePaiement, :nom, :description, :etat)");

    query.bindValue(":idCommande", idCommande);
    query.bindValue(":dateDePeremption", dateDePeremption);
    query.bindValue(":total", total);
    query.bindValue(":typeDePaiement", typeDePaiement);
    query.bindValue(":nom", nom);
    query.bindValue(":description", description);
    query.bindValue(":etat", etat);  // Liaison pour 'etat'

    // Exécution de la requête et gestion des erreurs
    if (query.exec())
       {
           qDebug() << "Commande ajoutée avec succès";
           return true;
       }
       else
       {
           qDebug() << "Erreur d'ajout: " << query.lastError().text();
           return false;
       }
   }

QSqlQueryModel* Commande::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM commande");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Commande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date de Peremption"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Total"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Type de Paiement "));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Nom "));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("État"));

    return model;
}

bool Commande::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM commande WHERE idCommande = :idCommande");
    query.bindValue(":idCommande", id);

    return query.exec();
}

bool Commande::rechercher(int id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM commande WHERE idCommande = :idCommande");
    query.bindValue(":idCommande", id);

    if (query.exec() && query.first()) {
        // Mettre à jour les attributs de l'objet courant
        this->setIdCommande(query.value("idCommande").toInt());
        this->setDateDePeremption(query.value("dateDePeremption").toString());
        this->setTotal(query.value("total").toDouble());
        this->setTypeDePaiement(query.value("typeDePaiement").toString());
        this->setNom(query.value("nom").toString());
        this->setDescription(query.value("description").toString());
        this->setEtat(query.value("etat").toString());
        return true;
    }
    return false;
}

bool Commande::modifier(int id)
{
    // Vérification de l'existence de l'ID dans la base de données
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM commande WHERE idCommande = :idCommande");//ya3mel mou9arna bil id eli ktebto weli bch tbadlo
    checkQuery.bindValue(":idCommande", id);

    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        QMessageBox::warning(nullptr, "Erreur", "L'identifiant n'existe pas dans la base de données !");
        return false; // Si l'ID n'existe pas, retourne false
    }

    // Préparation de la requête SQL avec TO_DATE pour la date au format correct
    QSqlQuery query;
    query.prepare("UPDATE commande SET "
                  "dateDePeremption = TO_DATE(:dateDePeremption, 'YYYY-MM-DD HH24:MI:SS'), "
                  "total = :total, "
                  "typeDePaiement = :typeDePaiement, "
                  "nom = :nom, "
                  "description = :description "
                  "etat = :etat "
                  "WHERE idCommande = :idCommande");

    // Lier les valeurs des paramètres
    query.bindValue(":dateDePeremption", dateDePeremption);
    query.bindValue(":total", total);
    query.bindValue(":typeDePaiement", typeDePaiement);
    query.bindValue(":nom", nom);
    query.bindValue(":description", description);
    query.bindValue(":etat", etat);
    query.bindValue(":idCommande", id);

    // Exécution de la requête de mise à jour
    if (!query.exec()) {
        qDebug() << "Erreur lors de la mise à jour : " << query.lastError().text();
        return false; // Si la requête échoue, retourne false
    }

    return true; // Mise à jour réussie
}


QSqlQueryModel* Commande::rechercher(QString valeur)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM commande WHERE idCommande LIKE :valeur OR dateDePeremption LIKE :valeur OR total LIKE :valeur OR "
                      "typeDePaiement LIKE :valeur OR nom LIKE :valeur OR description LIKE :valeur OR etat LIKE :valeur");  // Recherche sur 'etat'
    valeur = "%" + valeur + "%";
    query.bindValue(":valeur", valeur);
    query.exec();
    model->setQuery(query);

    return model;
}

QSqlQueryModel* Commande::rechercherParNom(QString valeur)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    // Préparer la requête SQL
    query.prepare("SELECT * FROM commandes WHERE nom LIKE :valeur");
    query.bindValue(":valeur", "%" + valeur + "%");

    // Exécuter la requête
    if (!query.exec()) {
        qDebug() << "Erreur dans la requête SQL: " << query.lastError().text();
        return nullptr;  // Retourner null en cas d'erreur
    }

    // Remplir le modèle avec les résultats de la requête
    model->setQuery(query);

    // Vérifier si le modèle contient des résultats
    if (model->rowCount() == 0) {
        qDebug() << "Aucun résultat trouvé pour la recherche.";
    }

    return model;  // Retourner le modèle
}



QSqlQueryModel* Commande::triID()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM commande ORDER BY idCommande");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Commande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date de Peremption"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Total"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type de Paiement"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Etat"));
    return model;
}

QSqlQueryModel* Commande::triNom()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM commande ORDER BY nom");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Commande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date de Peremption"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Total"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type de Paiement"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Etat"));
    return model;
}

QSqlQueryModel* Commande::triIDdesc()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM commande ORDER BY idCommande DESC");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Commande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date de Peremption"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Total"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type de Paiement"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Etat"));
    return model;
}

QSqlQueryModel* Commande::triNomDesc()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM commande ORDER BY nom DESC");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Commande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date de Peremption"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Total"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type de Paiement"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Etat"));
    return model;
}
