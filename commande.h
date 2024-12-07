#ifndef COMMANDE_H
#define COMMANDE_H

#include <QSqlQueryModel>
#include <QMessageBox>
#include <QApplication>
#include <QSqlQuery>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>
#include <QtWidgets/QMainWindow>

class Commande
{
public:
    Commande();  // Constructeur par défaut
    Commande(int idCommande, QString dateDePeremption, double total, QString typeDePaiement, QString nom, QString description, QString etat);  // Constructeur avec paramètres

    // Getters
    int getIdCommande() const;
    QString getDateDePeremption() const;
    double getTotal() const;
    QString getTypeDePaiement() const;
    QString getNom() const;
    QString getDescription() const;
    QString getEtat() const;  // Getter pour l'état

    // Setters
    void setIdCommande(int id);
    void setDateDePeremption(const QString& date);
    void setTotal(double tot);
    void setTypeDePaiement(const QString& typePaiement);
    void setNom(const QString& name);
    void setDescription(const QString& desc);
    void setEtat(const QString& etat);  // Setter pour l'état

    // Méthodes
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id);
    bool rechercher(int id);
    bool modifier(int id);
    QSqlQueryModel* rechercher(QString valeur);
    QSqlQueryModel* rechercherParNom(QString valeur);
    QSqlQueryModel* triID();
    QSqlQueryModel* triNom();
    QSqlQueryModel* triIDdesc();
    QSqlQueryModel* triNomDesc();

private:
    int idCommande;
    QString dateDePeremption;
    double total;
    QString typeDePaiement;
    QString nom;
    QString description;
    QString etat;  // L'attribut 'etat' ajouté
};

#endif // COMMANDE_H
