#ifndef ADDLIVREUR_H
#define ADDLIVREUR_H

#include <QDialog>
#include "connection.h" // Include the connection class

namespace Ui {
class AddLivreur;
}

class AddLivreur : public QDialog
{
    Q_OBJECT

public:
    explicit AddLivreur(QWidget *parent = nullptr);
    ~AddLivreur();

    // Methods to get livreur data entered
    QString getIdLivreur() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getTelephone() const;
    QString getAdresse() const;
    int getTotalLivraisons() const;

    // Function to insert livreur into the database
    bool addLivreurToDatabase();

private:
    Ui::AddLivreur *ui;

    // Function to check if a field is unique in the database
    bool isUnique(const QString &field, const QString &value);
};

#endif // ADDLIVREUR_H
