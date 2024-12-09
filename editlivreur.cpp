#include "editlivreur.h"
#include "ui_editlivreur.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

EditLivreur::EditLivreur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditLivreur)
{
    ui->setupUi(this);

    // Connect the Cancel button to close the dialog
    connect(ui->cancelbutton, &QPushButton::clicked, this, &EditLivreur::onCancelButtonClicked);

    // Connect the OK button to update the database and close the dialog
    connect(ui->okbutton, &QPushButton::clicked, this, &EditLivreur::onOkButtonClicked);
}

EditLivreur::~EditLivreur()
{
    delete ui;
}

// Set the current livreur data into the dialog's fields
void EditLivreur::setLivreurData(const QString &idLivreur, const QString &nom, const QString &prenom,
                                 const QString &telephone, const QString &adresse, const QString &totalLivraisons)
{
    currentIdLivreur = idLivreur;  // Store the ID to identify the livreur during the update
    ui->idLivreurLineEdit->setText(idLivreur);
    ui->nomLineEdit->setText(nom);
    ui->prenomLineEdit->setText(prenom);
    ui->telephoneLineEdit->setText(telephone);
    ui->adresseLineEdit->setText(adresse);
    ui->totalLivraisonsLineEdit->setText(totalLivraisons);  // Set Total Livraisons
}

// Cancel button clicked
void EditLivreur::onCancelButtonClicked()
{
    this->reject();  // Close the dialog without saving changes
}

// OK button clicked
void EditLivreur::onOkButtonClicked()
{
    // Update the livreur in the database
    if (updateLivreurInDatabase()) {
        this->accept();  // Close the dialog and return success
    }
}

// Update the livreur's information in the database
bool EditLivreur::updateLivreurInDatabase()
{
    // Validate Total Livraisons (must be a non-negative integer)
    bool ok;
    int totalLivraisons = ui->totalLivraisonsLineEdit->text().toInt(&ok);
    if (!ok || totalLivraisons < 0) {
        QMessageBox::warning(this, "Validation Error", "Total Livraisons must be a non-negative integer.");
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE LIVREUR SET "
                  "NOM = :nom, PRENOM = :prenom, TELEPHONE = :telephone, ADRESSE = :adresse, TOTAL_LIVRAISONS = :totalLivraisons "
                  "WHERE IDLIVREUR = :id");

    query.bindValue(":nom", ui->nomLineEdit->text());
    query.bindValue(":prenom", ui->prenomLineEdit->text());
    query.bindValue(":telephone", ui->telephoneLineEdit->text());
    query.bindValue(":adresse", ui->adresseLineEdit->text());
    query.bindValue(":totalLivraisons", totalLivraisons);  // Bind the Total Livraisons value
    query.bindValue(":id", currentIdLivreur);  // Use the stored ID to update the correct livreur

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Livreur updated successfully.");
        return true;
    } else {
        QMessageBox::warning(this, "Error", "Failed to update livreur: " + query.lastError().text());
        return false;
    }
}
